# LVGL 9.1 迁移报告

## 修改记录

### `platformio.ini`

- 将浮动的 `espressif32` 锁定为 `espressif32@7.0.1`。
- 将 `lvgl@8.3.11` 升级并锁定为 `lvgl@9.1.0`。
- 保留可验证通过的 `LovyanGFX@1.2.19`。
- 移除当前源码没有使用的 Adafruit SSD1306/GFX/BusIO 依赖。
- 移除重复控制 GT911 的 TAMC_GT911 依赖。
- 增加 `LV_CONF_INCLUDE_SIMPLE` 和工程 include 搜索路径，使 LVGL 库编译时读取项目配置。

### `include/lv_conf.h`

- 新增 LVGL 9.1 工程配置。
- 使用 RGB565 对应的 16 位颜色深度。
- 启用 SquareLine UI 使用的 Montserrat 20 字体。
- 使用无 OS 模式，由 Arduino 主循环调用 LVGL handler。

### `src/3_5LVGL.cpp`

- `lv_disp_drv_t` 驱动注册迁移到 `lv_display_create()` 和 display setter API。
- `lv_disp_draw_buf_t` 迁移到 `lv_display_set_buffers()`。
- flush 回调迁移到 LVGL 9 签名，并使用 `lv_display_flush_ready()`。
- 输入驱动迁移到 `lv_indev_create()`、`lv_indev_set_type()` 和 `lv_indev_set_read_cb()`。
- 输入状态迁移到 `LV_INDEV_STATE_PRESSED/RELEASED`。
- 通过 `lv_tick_set_cb()` 使用 Arduino `millis()` 作为 LVGL tick 来源。
- 使用 20 行 RGB565 partial 双缓冲；缓冲大小按 LVGL 9 的颜色格式字节数计算。
- 缓冲分配到内部 DMA 内存，并增加分配失败保护。
- 等待 LovyanGFX DMA 传输结束后再通知 LVGL，避免缓冲被过早复用。
- 统一使用 LovyanGFX 已配置的 GT911 输入，避免 TAMC_GT911 重复初始化 I2C。

### `src/ui.c`

- `lv_disp_t` 更换为 `lv_display_t`。
- `lv_disp_set_theme()` 更换为 `lv_display_set_theme()`。
- `lv_disp_load_scr()` 更换为 `lv_screen_load()`。

## LVGL 9.1 API 依据

迁移遵循 LVGL 9 的 display、input device、tick 和 screen API 模型：显示器与输入设备先创建，再用 setter 设置回调和缓冲区；flush 完成由 display API 通知；屏幕加载和 display 类型采用 `lv_screen_*`、`lv_display_*` 命名。

SquareLine 1.6.1 输出的 widget、image、event、style 和 animation helper 已使用 LVGL 9.1 API，编译检查未发现需要继续替换的 LVGL 8 widget API。

## 完整性检查

- PlatformIO 依赖图只包含 LVGL 9.1.0、LovyanGFX 1.2.19 和框架 SPI/Wire。
- 全部 `.c`、`.cpp`、UI 图片描述符和字体引用通过编译。
- ELF 链接通过，无 LVGL 版本冲突或未解析符号。
- `firmware.bin` 生成成功。

## 尚需实机确认

- 开发板定义显示 N8 无 PSRAM，但原工程配置为 `qio_opi` 和 `BOARD_HAS_PSRAM`。
- LCD 逻辑分辨率 480×320，面板物理配置 320×480；需确认旋转后的画面方向。
- GT911 触摸方向及坐标是否与 LCD 旋转一致。
- GPIO18 的业务设备及 ON/OFF 有效电平；原 `Lamp_on()`、`Lamp_off()` 为空，因此本次未自行定义电平。
