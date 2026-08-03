# PlatformIO + LVGL 9.1 升级说明

## 目标环境

- PlatformIO Core 6.1.19
- PlatformIO Espressif32 Platform 7.0.1
- Arduino-ESP32 2.0.17
- LVGL 9.1.0
- LovyanGFX 1.2.19
- ESP32-S3、ILI9488 SPI LCD、GT911 触摸

## 构建与下载

在本目录执行：

```powershell
pio run
pio run --target upload --upload-port COM3
pio device monitor --port COM3 --baud 115200
```

实际串口不是 COM3 时，请替换为设备管理器或 `pio device list` 显示的端口。

## 硬件配置

LCD、GT911、背光和业务 GPIO 引脚保留原工程配置：

- LCD SPI：SCLK 42、MOSI 39、DC 41、CS 40、RST 2
- GT911：SDA 15、SCL 16、INT 47、RST 48、地址 0x14
- 背光：GPIO38，高电平开启
- 业务输出：GPIO18（原 UI 事件函数仍为空，未猜测其有效电平）

`platformio.ini` 保留原工程的 8 MB Flash 和 OPI PSRAM 配置。下载前必须确认实际模组支持该内存模式。

## 验证结果

`pio run` 已完成源码编译、静态库链接、ELF 链接和固件镜像生成：

- RAM：86,816 / 327,680 bytes（26.5%）
- Flash：706,661 / 3,342,336 bytes（21.1%）
- 输出：`.pio/build/esp32-s3-devkitc-1/firmware.bin`

编译验证不能代替实机 LCD、触摸方向、PSRAM 模式和 GPIO 有效电平验证。
