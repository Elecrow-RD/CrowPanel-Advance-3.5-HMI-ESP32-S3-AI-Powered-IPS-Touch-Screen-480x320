import base64
import json
import random
import time

import websockets
import asyncio
from tools.asr import AsrWsClient
from tools.config import CONNECTIONS, CLIENTS, TASKS
from tools.llm_openai import chat

from tools.tts import test_submit


# 异步任务的异常处理
async def handle_exception(task, *args, **kwargs):
    try:
        await task(*args, **kwargs)
    except websockets.exceptions.ConnectionClosedError as e:
        print(f"(in)客户端 {args[0]} 连接已关闭: {e}")
    except ConnectionResetError as e:
        print(f"(in)连接重置错误: {e}")
    except Exception as e:
        print(f"(in)处理音频时发生错误: {e}")


# 会话任务
async def process_audio(mac_address):
    # 第一步获取数据库或redis里面的LLM+TTS配置
    # get_config = await get_redis_config(mac_address)  # 获取个人的redis动态配置
    # 第二步ASR
    asr_word = await AsrWsClient().send_full_request(mac_address)
    await CLIENTS[mac_address].send(f"USER:{asr_word}")
    print(f"{mac_address}-问:", asr_word)

    # 第三步LLM+TTS
    await chat(asr_word, mac_address)

    # 第4步更新redis配置
    # await re.set(mac_address, json.dumps(per_config), ex=3600)  # 更新redis中的聊天记录


# 开场白任务
async def open_word(mac_address):

    sentence = random.choice([
       "好"
    ])

    await test_submit(sentence, mac_address)
    # await CLIENTS[mac_address].send("finish_tts")


# 唤醒任务
async def wake_up(mac_address):
    sentence = random.choice([
        "hello!"
    ])
    print("唤醒回复:", sentence)
    await test_submit(sentence, mac_address, wake_up=True)
    await CLIENTS[mac_address].send("finish_tts")





async def handler(websocket, path):
    client_id = websocket.remote_address
    print(f"客户端 {client_id} 已连接")
    try:
        async for message in websocket:
            # 接收到客户端发送的json数据
            rcv_data = json.loads(message)
            event = rcv_data["event"]
            mac_address = rcv_data["mac_address"]
            data = rcv_data["data"]

            # 接收单片机录音音频流
            if event == "record_stream":
                audio_data = base64.b64decode(data)
                await CONNECTIONS[mac_address].put(audio_data)
            # 重启process_audio
            elif event == "re_process_audio":
                print(mac_address,"启动啦!")
                TASKS[mac_address] = asyncio.create_task(handle_exception(process_audio, mac_address))

            # 开场白
            elif event == "open_word":
                CONNECTIONS[mac_address] = asyncio.Queue()
                CLIENTS[mac_address] = websocket
                print("开场白")
                TASKS[mac_address] = asyncio.create_task(handle_exception(process_audio, mac_address))


            # 唤醒对话
            elif event == "wake_up":
                print("唤醒对话")
                CONNECTIONS[mac_address] = asyncio.Queue()
                CLIENTS[mac_address] = websocket
                TASKS[mac_address] = asyncio.create_task(handle_exception(wake_up, mac_address))



            # 打断对话
            elif event == "interrupt_audio":
                print("打断对话")
                await cancel_process_audio_task(mac_address)
                time.sleep(2.2)
                TASKS[mac_address] = asyncio.create_task(handle_exception(process_audio, mac_address))


            # 10秒钟未有声音传输,则取消process_audio任务
            elif event == "timeout_no_stream":
                print("10秒内没有声音传输,取消process_audio任务")
                await cancel_process_audio_task(mac_address)
                CLIENTS.pop(mac_address)
                CONNECTIONS.pop(mac_address)
                TASKS.pop(mac_address)



    except websockets.exceptions.ConnectionClosedError as e:
        print(f"(out)客户端 {client_id} 断开连接: {e}")

    except ConnectionResetError as e:
        print(f"(out)连接重置错误: {e}")

    except Exception as e:
        print(f"(out)处理消息时出现错误: {e}")

    finally:
        # if mac_address in CONNECTIONS:
        #     del CONNECTIONS[mac_address]
        # if mac_address in CLIENTS:
        #     del CLIENTS[mac_address]
        print(f"客户端 {client_id} 连接已关闭")




#取消 process_audio 任务
async def cancel_process_audio_task(mac_address):
    task = TASKS.get(mac_address)
    if task and not task.done():
        task.cancel()


async def main():
    async with websockets.serve(handler, "0.0.0.0", 8765):
        print("服务器已在8765端口启动,等待连接...")
        await asyncio.Future()  # 永久运行


if __name__ == '__main__':
    asyncio.run(main())
