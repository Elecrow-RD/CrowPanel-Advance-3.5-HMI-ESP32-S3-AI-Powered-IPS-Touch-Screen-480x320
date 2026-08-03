# Libraries 升级方案

本次未修改任何第三方库源码。所有依赖均由 `platformio.ini` 从官方 PlatformIO Registry 获取。

| 库名称 | 原版本 | 升级版本 | 是否升级 | 原因 |
| --- | --- | --- | --- | --- |
| LVGL | 8.3.11 | 9.1.0 | 是 | SquareLine UI 已按 LVGL 9.1.0 生成，原 LVGL 8 驱动接口不兼容 |
| LovyanGFX | 1.2.19 | 1.2.19 | 否 | 当前版本可编译 ESP32-S3、ILI9488、GT911 和 DMA 路径 |
| TAMC_GT911 | 1.0.2 | 移除 | 是 | 工程实际通过 LovyanGFX 的 `Touch_GT911` 读取触摸，重复初始化同一 I2C 设备没有必要 |
| Adafruit SSD1306 | 2.5.13 | 移除 | 是 | 当前参与构建的源码没有 SSD1306 对象或调用，仅有未使用 include |
| Adafruit GFX | 1.12.0 | 移除 | 是 | 仅为 SSD1306 传递依赖，LCD 使用 LovyanGFX |
| Adafruit BusIO | 1.17.0 | 移除 | 是 | 仅为 Adafruit 库传递依赖 |

依赖版本已锁定，避免不同机器解析到不同主版本。`.pio` 是可再生缓存，不属于第三方源码交付内容。
