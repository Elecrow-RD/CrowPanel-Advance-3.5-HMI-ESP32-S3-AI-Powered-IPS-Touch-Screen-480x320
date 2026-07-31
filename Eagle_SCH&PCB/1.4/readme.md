# 3.5-Inch ESP32-S3 Hardware Driver Guide

| Item | Details |
|---|---|
| Document Version | V1.0 |
| Date | 2026-07-30 |
| Author | OpenAI Codex (compiled from project materials) |
| Applicable Hardware | 3.5-inch ESP32 Display / CrowPanel Advance HMI boards |
| Hardware References | `1.4/ESP32 Display 3.5 inch V1.4.sch`, `.brd`, and `ESP32-Display-3.5-inch-V1.4.pdf` |
| Software References | Board-level examples from `lesson-01` through `lesson-08` under `3.5_Arduino` (excluding examples bundled with third-party libraries) |
| Determination Principle | When the schematic conflicts with code that has been successfully tested, the project’s board-level example code takes precedence, and the discrepancy is documented |

> **Important Version Notice**: The directories and filenames identify the hardware as **V1.4**, but the title block in the Eagle schematic and the internal title in the PDF identify it as **V1.2**. This document describes the materials currently available in the repository and does not claim that the two versions are electrically identical. Before production, repair, or porting, the board revision must be confirmed from the PCB silkscreen and/or physical measurements.

## 1. Evidence Levels and Usage

| Level | Meaning | Usage Recommendation |
|---|---|---|
| A - Code Verified | The project’s board-level examples contain explicit pin assignments and success/failure criteria or visible functionality | Use as the primary reference for maintenance and porting |
| B - Schematic and Code Consistent | The schematic nets match the code, but the examples do not cover all edge cases | Usable, but integration testing with the target firmware is still required |
| C - Schematic Confirmed | The component/net exists in the schematic, but the repository contains no corresponding board-level driver test case | Do not claim software verification; additional testing is required |
| D - External Module Example | The code verifies a module connected to an expansion port, not an onboard component | Enable only when the same external module is installed |

“Verified” in this document only means that the repository provides runnable examples and explicit configurations for this board. It does not mean that the physical board was reflashed during the creation of this document.

## 2. System Architecture and Software Stack

- MCU: ESP32-S3-WROOM-1, identified in the schematic as `ESP32-S3-N16R8` (board-level designation indicating 16 MB Flash and 8 MB PSRAM).
- Framework: Arduino-ESP32, supported at the lower level by ESP-IDF peripheral drivers (SPI/I2C/I2S/UART, DMA, and PSRAM). No bare-metal register operations are used.
- Graphics: LovyanGFX + LVGL 9.1.0; LVGL uses two full-screen RGB565 buffers in PSRAM.
- Storage: Arduino `FS`/`SD` on a dedicated SPI bus.
- Audio: ESP32-audioI2S 3.0.12 + Wi-Fi, with output to the onboard NS4168.
- Wireless Expansion: RF24 1.6.1 (nRF24L01) and RadioLib 7.1.0 (SX1262/LoRaWAN).
- Sensor Expansion: Crowbits_DHT20; separate GT911 driver code is also provided for touch input.
- RTOS: Arduino-ESP32 runs FreeRTOS internally, but the project examples do not create custom tasks, queues, or synchronization objects.

## 3. Peripheral Overview

| Category | Peripheral/Component | Key Interface and GPIOs | Status |
|---|---|---|---|
| MCU | ESP32-S3-WROOM-1 | Native USB GPIO19/20; UART0; multiple multiplexed peripherals | B |
| Display | 3.5-inch TFT, configured as ILI9488 in code | SPI2: SCLK42, MOSI39, DC41, CS40; RST2; BL38 | A; RST conflict between schematic and code |
| Touch | Configured as GT911 in code | I2C0: SDA15, SCL16; INT47, RST48; 0x14/400 kHz | A; controller model not identified in schematic |
| Storage | microSD/TF (J5) | MISO4, SCLK5, MOSI6, CS7; SPI at 80 MHz | A/B |
| Audio Output | NS4168 + speaker connector J12 | I2S: LRCLK11, BCLK13, DOUT12; CTRL21 | A/B |
| Microphone | Digital I2S/PDM MIC (MIC1) | CLK9, DATA10; GPIO45 selects the switch path | C |
| Analog Switch | SGM3799 | GPIO9/10 multiplexed between MIC and wireless SPI; SEL=GPIO45 | B |
| Buzzer | Passive buzzer B1 + SS8050 | GPIO8 through NPN low-side driver | C |
| RTC | PCF8563/BM8563 + CR1220 | SDA15, SCL16; common address 0x51 (not verified by code) | C |
| USB-UART | CH340K + automatic download circuit | UART0; DTR/RTS drive EN/BOOT | C (hardware function) |
| Native USB/OTG | Type-C J2 | D-=GPIO19, D+=GPIO20 (through 0 Ω configuration positions) | C |
| USB-UART/Power | Type-C J1 | CH340K USB data, 5 V input | C (hardware function) |
| Battery Charging | TP4059 + PH2.0 battery connector J3 | Autonomous hardware operation; PROG=2 kΩ; CHRG/DONE indicators managed by U14 | C |
| Power Boost | RY3420/HM3416H | Autonomous hardware operation; 45.3 kΩ/10 kΩ feedback; EN connected to VIN | C |
| Charging Indicator Control | STC8G1K08 + red/green LEDs | Monitors CHRG/DONE; not an ESP32-programmable interface | C |
| Backlight/Display Power | NPN backlight switch, optional TFT power switch | BL=GPIO38, active high; GPIO14 path marked NC | A/C |
| Buttons | BOOT K3, RESET K4 | BOOT=GPIO0 active low; RESET=EN active low | C (hardware function) |
| I2C Expansion | J13 HY2.0 | SCL16, SDA15, 3V3, GND | B; shares bus with RTC/touch |
| UART1 Expansion | J15 HY2.0 | ESP TX=GPIO17, ESP RX=GPIO18, 3V3, GND | C |
| UART0 Expansion | J10 | RXD0_H, TXD0_H, 5V, GND | C; shared with download and logging |
| Wireless SPI Expansion | J9/J11 | SCLK10, MISO9, MOSI3, CS0, BUSY46, 3V3/GND | B/D |
| External UART Module | Zigbee example | RX2, TX1, 115200 8N1; GPIO45=LOW | D; multiplexing risk with LCD RST/wireless control |
| External Sensor | DHT20 | I2C SDA15/SCL16; 200 ms read interval | D |
| External Actuator | lesson-03/05 lamp or LED | GPIO18, push-pull output | D; conflicts with UART1 RX |
| External nRF24L01 | RF24 example | SCLK10/MISO9/MOSI3, CE1, CSN2; GPIO45=LOW | D |
| External SX1262 | LoRaWAN example | SCLK10/MISO9/MOSI3, NSS0, DIO1=1, RST2, BUSY46 | D |

## 4. Authoritative GPIO Mapping

| GPIO | Schematic Net/Function | Usage in Successfully Tested Code | Electrical/Multiplexing Notes |
|---:|---|---|---|
| 0 | Wireless CS, BOOT button | SX1262 NSS | Boot strapping pin; a low level from the button affects startup and SPI transactions |
| 1 | UART2 TX net | nRF24 CE; SX1262 DIO1; Zigbee TX | Mutually exclusive across examples; do not initialize simultaneously |
| 2 | UART2 RX net | LCD RST; nRF24 CSN; SX1262 RST; Zigbee RX | Primary conflict point; code takes precedence, but each function requires exclusive use |
| 3 | Wireless SPI MOSI | nRF24/SX1262 MOSI | GPIO45 must select the wireless path |
| 4 | SD MISO | SD MISO | Input with onboard 10 kΩ pull-up |
| 5 | SD SCLK | SD SCLK | Push-pull clock |
| 6 | SD MOSI | SD MOSI | Push-pull data with onboard 10 kΩ pull-up |
| 7 | SD CS | SD CS | Push-pull, idle high, with onboard 10 kΩ pull-up |
| 8 | BEEP | No code | PWM/LEDC recommended; driven through an NPN transistor; polarity and frequency require physical verification |
| 9 | MIC CLK / wireless MISO (SGM3799 COM) | nRF24/SX1262 MISO | Selected by GPIO45; audio recording and wireless SPI cannot be used simultaneously |
| 10 | MIC DATA / wireless SCLK (SGM3799 COM) | nRF24/SX1262 SCLK | Same as above |
| 11 | I2S LRCLK | Audio LRCLK | Push-pull output |
| 12 | I2S SDIN (amplifier data input) | Audio DOUT | Named DOUT in code from the ESP32’s perspective |
| 13 | I2S BCLK | Audio BCLK | Push-pull output |
| 14 | TFT_PWR (components marked NC) | No code | Do not rely on this unless BOM installation of Q11/Q12/R52/R53/R61 is confirmed |
| 15 | I2C SDA | GT911, DHT20, RTC, J13 | Open-drain; schematic shows a 4.7 kΩ pull-up to 3.3 V |
| 16 | I2C SCL | GT911, DHT20, RTC, J13 | Open-drain; schematic shows a 4.7 kΩ pull-up to 3.3 V |
| 17 | UART1 TX | No board-level UART1 example provided | J15 uses 3.3 V logic |
| 18 | UART1 RX | Push-pull output for external lamp/LED | Cannot be used as UART1 RX while GPIO18 is used for the lamp |
| 19 | USB D- | USB Serial/JTAG or OTG | Native USB differential line; do not use as a general-purpose GPIO |
| 20 | USB D+ | USB Serial/JTAG or OTG | Native USB differential line; do not use as a general-purpose GPIO |
| 21 | NS4168 control | Audio amplifier control: initially HIGH, then LOW before playback | Code proves LOW is the operating state; do not assume a conventional active-high EN when porting |
| 38 | LCD backlight | `OUTPUT` + `HIGH` turns it on | Active high through an SS8050 switch; PWM dimming is possible but the frequency must be verified |
| 39 | TFT SDA/MOSI | LCD MOSI | Write-only, no MISO |
| 40 | TFT CS | LCD CS | Push-pull, idle high |
| 41 | TFT RS/D-C | LCD DC | Push-pull |
| 42 | TFT SCK | LCD SCLK | SPI2, 40 MHz writes |
| 45 | SPI/MIC analog switch select | Wireless examples set it to `LOW` | LOW selects the wireless path and disconnects the microphone; stop the bus/microphone clock before changing the level |
| 46 | BOOT_BUSY | SX1262 BUSY; SS placeholder for the nRF24 SPI object | ESP32-S3 boot strapping pin; peripheral power-up levels must not interfere with startup |
| 47 | TP_INT | GT911 INT | Used by LovyanGFX; the auxiliary `touch.h` version sets it to -1 |
| 48 | TP_RST | GT911 RST | Used by LovyanGFX; the auxiliary `touch.h` version sets it to -1 |

## 5. Detailed Onboard Peripheral Descriptions

### 5.1 ESP32-S3 MCU, Flash/PSRAM, and Boot

The schematic component is an ESP32-S3-WROOM-1, with N16R8 shown in the drawing text. The project uses the Arduino API, ESP-IDF SPI/I2C driver types, and `heap_caps_malloc(..., MALLOC_CAP_SPIRAM)`, so the display firmware depends on functional PSRAM.

- Download UART: UART0 connects to the CH340K through a BSS138 level-shifting/isolation network; J10 also exposes `RXD0_H/TXD0_H`.
- Automatic Download: The CH340K’s DTR/RTS signals drive GPIO0/EN through U10.
- Manual Buttons: K3 pulls GPIO0 low (BOOT); K4 pulls EN low (RESET).
- Native USB: GPIO19/20 correspond to D-/D+ and connect to Type-C J2 through 0 Ω configuration positions.
- Serial Examples: lesson-01 uses `Serial.begin(9600)`; most other examples use 115200. Standardize the baud rate when porting to avoid mistaking garbled logs for a hardware failure.

```cpp
Serial.begin(115200);
// PSRAM for the display: each 480x320 RGB565 buffer is approximately 307200 bytes
buf = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * 480 * 320,
                                     MALLOC_CAP_SPIRAM);
```

### 5.2 ILI9488 TFT Display and Backlight

The code explicitly instantiates `lgfx::Panel_ILI9488` using SPI2_HOST, mode 0, and automatic DMA channel selection. The panel’s native memory orientation is 320x480, while the application uses a logical landscape resolution of 480x320.

| Signal | GPIO | Configuration |
|---|---:|---|
| SCLK | 42 | SPI2, 40 MHz write, 16 MHz configured read |
| MOSI/SDA | 39 | Unidirectional write |
| MISO | -1 | Disabled, `readable=false` |
| D/C | 41 | GPIO push-pull |
| CS | 40 | GPIO push-pull |
| RST | 2 (code) | Does not match the schematic; see the conflict matrix |
| BL | 38 | Push-pull high turns it on; drives LEDK through an NPN transistor |

Key parameters: `spi_3wire=false`, `invert=true`, `rgb_order=false`, `offset_rotation=3`, `bus_shared=true`, `dummy_read_pixel=8`, and `dummy_read_bits=1`. LVGL uses RGB565, two full-screen buffers, and `LV_DISPLAY_RENDER_MODE_FULL`; refreshes are performed using LovyanGFX DMA.

Recommended initialization sequence: configure the driver object → `gfx.init()` → `gfx.initDMA()` → clear the screen to black → configure LVGL → finally drive GPIO38 high to prevent visual artifacts during power-up. Do not use GPIO38 to drive the backlight current directly; it only drives the onboard switching transistor.

```cpp
cfg.spi_host = SPI2_HOST;
cfg.freq_write = 40000000;
cfg.pin_sclk = 42; cfg.pin_mosi = 39; cfg.pin_dc = 41;
cfg.pin_cs = 40; cfg.pin_rst = 2;
pinMode(38, OUTPUT); digitalWrite(38, HIGH);
```

### 5.3 GT911 Capacitive Touch

The successfully tested LovyanGFX configuration uses the GT911 with I2C0, SDA15, SCL16, 400 kHz, address 0x14, INT47, RST48, and coordinate ranges of 0..319 / 0..479. The project also includes a `touch.h` adaptation layer in which the GT911 INT/RST definitions are set to -1. The 47/48 configuration in the complete board-level driver `LovyanGFX_Driver.h` is more consistent with the schematic and should be treated as the authoritative implementation.

I2C is an open-drain bus, and the schematic shows 4.7 kΩ pull-ups from both SDA and SCL to 3.3 V. The GT911 address can vary between 0x14 and 0x5D depending on the INT/RST power-up sequence. The current code fixes it at 0x14. When porting, preserve LovyanGFX’s reset/address-selection sequence or probe both addresses at startup.

```cpp
cfg.i2c_port = I2C_NUM_0;
cfg.pin_sda = GPIO_NUM_15; cfg.pin_scl = GPIO_NUM_16;
cfg.pin_int = 47; cfg.pin_rst = 48;
cfg.freq = 400000; cfg.i2c_addr = 0x14;
```

### 5.4 microSD/TF Card

J5 is used in 4-wire SPI mode: MISO4, SCLK5, MOSI6, and CS7, with a 3.3 V supply. MISO/MOSI/CS have 10 kΩ pull-ups. The code creates a dedicated `SPIClass(HSPI)` and requests an 80 MHz clock when mounting the FAT file system.

```cpp
SPIClass SD_SPI(HSPI);
SD_SPI.begin(5, 4, 6);
bool ok = SD.begin(7, SD_SPI, 80000000);
```

Note: 80 MHz is the value requested by the code; actual clock division and card quality may result in a lower frequency or unstable operation. For long traces, adapter cards, or an extended temperature range, first reduce the frequency to 40/20 MHz. A source-code comment for `SD_CS` states that it is “not connected to an IO,” but both the schematic and working code clearly assign it to GPIO7; the comment is incorrect.

### 5.5 I2S Audio Output, NS4168, and Speaker

The onboard NS4168 receives digital I2S and provides differential output to the speaker through J12. The verified pins are BCLK13, LRCLK11, and DATA12; GPIO21 controls the amplifier path. The code initially sets GPIO21=HIGH and changes it to LOW before playback, so **LOW is the operating state**.

```cpp
pinMode(21, OUTPUT);
digitalWrite(21, HIGH);       // Initial mute/off state
audio.setPinout(13, 11, 12);  // BCLK, LRCLK, DOUT
audio.setVolume(20);
digitalWrite(21, LOW);        // Enable the amplifier path
```

The sample rate and bit depth are dynamically configured by ESP32-audioI2S based on the MP3 stream; the project does not use fixed register values. J12 is a differential `ROUT+ / ROUT-` output. Neither speaker terminal may be connected to ground; do not connect standard single-ended headphones or attach an oscilloscope ground clip to either terminal.

### 5.6 Digital Microphone and SGM3799 Multiplexer

MIC1 is powered by a filtered 3.3 V supply, with CLK=GPIO9 and DATA=GPIO10. The SGM3799 switches GPIO9/10 between the microphone and the wireless expansion SPI interface (MISO/SCLK), with GPIO45 driving both select inputs.

- Verified Wireless Path: All nRF24/SX1262/Zigbee board-level examples set GPIO45=LOW, with comments explicitly stating “select wireless module/disable microphone.”
- Microphone Path: The repository contains no audio capture example. Circuit logic indicates that GPIO45=HIGH selects the MIC path, but this conclusion requires physical verification.
- Switching Sequence: First stop the SPI/I2S/PDM peripheral and set the associated pins to inputs or a stable idle state, then switch GPIO45, wait briefly, and initialize the target peripheral.

### 5.7 Passive Buzzer

B1 is driven by GPIO8 through a resistor and an SS8050 NPN transistor in a low-side switching configuration; the MCU does not directly supply the buzzer current. The repository does not provide a driver example. Use Arduino `tone()` or LEDC to output a square wave, starting with a low duty cycle for verification; output LOW when stopping. The actual volume, resonant frequency, and whether the output is inverted require physical verification.

```cpp
pinMode(8, OUTPUT);
tone(8, 2000);   // Recommended initial test value, not a verified product specification
noTone(8);
digitalWrite(8, LOW);
```

### 5.8 PCF8563/BM8563 RTC

U4 is a PCF8563/BM8563 using the shared I2C SDA15/SCL16 bus, a 32.768 kHz crystal, and a CR1220 backup battery. The repository contains no board-level RTC driver code. The device commonly uses the 7-bit address 0x51, but this must be confirmed through an I2C scan and physical seconds-counter verification before inclusion in a production driver.

Porting recommendation: first call `Wire.begin(15,16)`, then probe 0x51; check the VL (low-voltage) flag during initialization; use BCD conversion; and verify battery-backed time retention after power loss. The RTC shares the bus with the touch controller and DHT20. Do not change the pins or clock by repeatedly calling `Wire.begin()`.

### 5.9 Power, Charging, and Indicators

- Input: 5 V from Type-C J1/J2 and J10 is combined through Schottky diodes; each Type-C CC pin has a 5.1 kΩ pull-down for operation as a power sink.
- Battery: J3 is a single-cell lithium battery connector; VBAT feeds the TP4059 and the power-path MOSFET.
- Charging: The TP4059 PROG resistor is 2 kΩ. The actual charging current must be verified against the installed IC’s datasheet and thermal conditions; this document does not infer an absolute value from the resistor alone.
- Boost Conversion: RY3420/HM3416H, with a 4.7 µH/3 A inductor, 45.3 kΩ/10 kΩ feedback, and EN connected to VIN; it powers on automatically in hardware.
- Indicators: The STC8G1K08 monitors the TP4059 CHRG/DONE signals and controls the two-color LED. The ESP32 has no direct status GPIO. Software-accessible battery or charging status requires a hardware modification or an additional ADC/communication channel.
- Display Power: Several components in the GPIO14 control path are marked NC and must not be treated as production functionality. Backlight GPIO38 has been verified by code.

## 6. Expansion Interfaces and Verified External Modules

### 6.1 Interface Pin Assignments

| Interface | Pin Assignment | Levels/Notes |
|---|---|---|
| J13 I2C | 1=SCL16, 2=SDA15, 3=3V3, 4=GND | 3.3 V; onboard pull-ups are already present; peripheral addresses must not conflict with the GT911/RTC |
| J15 UART1 | 1=ESP RX18, 2=ESP TX17, 3=3V3, 4=GND | 3.3 V TTL; GPIO18 may be used as a lamp output by examples |
| J10 UART0 | 1=RXD0_H, 2=TXD0_H, 3=5V, 4=GND | Data logic is determined by the onboard CH340/level-shifting network; shared with download logging |
| J9 Wireless | 2=SCLK10, 3=MISO9, 4=MOSI3, 5=3V3, 6=GND, 7=IOT_5V | J9.1 connects to GPIO1 through a resistor; L3 at the 5 V position is marked NC, so power is not guaranteed |
| J11 Wireless | 2=SCL16, 3=SDA15, 5=BUSY46, 6=CS0 | J11.1 connects to GPIO2 through a resistor; verify the remaining power and ground pins against the physical silkscreen |
| J14 Auxiliary MCU | 1=TXD, 2=RXD, 3=VIN, 4=GND | Connects to the onboard STC8G1K08; it is not a general-purpose ESP32 UART breakout |

### 6.2 DHT20 (External, I2C)

lesson-05 uses SDA15/SCL16 and reads temperature and humidity every 200 ms after `dht20.begin()`. The DHT20 commonly uses address 0x38, but the project call does not explicitly specify an address; use the library default. The example also uses GPIO18 as a threshold indicator output, driving it high when the temperature exceeds 30.

Risk: A 200 ms interval may be shorter than the recommended conversion interval for some temperature and humidity sensors. If readings repeat or self-heating becomes significant, increase the interval according to the datasheet for the specific DHT20 module. It shares the bus with the RTC and touch controller.

### 6.3 nRF24L01 (External, Wireless SPI)

| Signal | GPIO | Description |
|---|---:|---|
| SCLK/MISO/MOSI | 10/9/3 | Custom HSPI; GPIO45 must be LOW |
| CE | 1 | GPIO push-pull |
| CSN | 2 | GPIO push-pull; conflicts with functions such as LCD RST |
| IRQ | Not used | Example polls `radio.
available()` |

Shared parameters for both ends: address `"00001"`, PA=`RF24_PA_MAX`, data rate 250 kbps, and channel 50 (2.450 GHz); the receiver uses pipe 0 and calls `startListening()`, while the transmitter calls `stopListening()`. The RF24 library depends on Arduino GPIO/SPI.

High-power nRF24 modules draw significant transient current at PA_MAX. Add low-ESR decoupling close to the module and verify sufficient 3.3 V power margin. GPIO2 also serves as the LCD reset in the code, so do not inadvertently reset the display after wireless initialization.

### 6.4 SX1262 / LoRaWAN (External, Wireless SPI)

The RadioLib module is mapped as `Module(NSS=0, DIO1=1, RST=2, BUSY=46, SPI)`, with SPI pins SCLK10/MISO9/MOSI3/SS0; GPIO45=LOW. After initialization, the current limit is set to 140 mA and TCXO voltage to 3.3 V.

The code supports EU868/US915 (subBand=1), OTAA/ABP, ADR, data rate, transmit power, RX2 DR, duty cycle, and 400 ms dwell time. The region must comply with local regulations and match the actual module frequency band. Do not use the example credentials directly in production; DevEUI/AppKey/NwkSKey should be securely provisioned and stored.

```cpp
SPI.begin(10, 9, 3, 0);
SX1262 radio = new Module(0, 1, 2, 46, SPI);
radio.begin();
radio.setCurrentLimit(140.0);
radio.setTCXO(3.3);
```

### 6.5 Zigbee UART Module (External)

The serial bridge in lesson-08 uses `Serial1.begin(115200, SERIAL_8N1, RX=2, TX=1)`, with GPIO45=LOW. The code only receives text from the module and outputs it to the USB serial port; it does not implement transmission, reset, flow control, or protocol-layer initialization.

GPIO1/2 strongly conflict with nRF24, SX1262, and LCD RST; this use case must be treated as an exclusive mode. The other two `Zigbee_On_Off_*` examples use the native Zigbee API of the ESP32-H2/C6 and do not demonstrate onboard Zigbee support for this ESP32-S3 board (this board also has no onboard Zigbee RF chip).

## 7. Schematic and Code Conflict Matrix

| Item | Schematic/Labeling | Verified Working Code | Final Selection | Resolution and Possible Cause |
|---|---|---|---|---|
| Hardware version | Filename V1.4; title block/PDF V1.2 | Example directory only identifies 3.5 | Do not forcibly reconcile | The title block may not have been updated, or materials from different revisions may have been mixed; verify against physical silkscreen/BOM |
| LCD controller | FPC does not explicitly specify ILI9488 | `Panel_ILI9488` | ILI9488 | The code works, so it takes precedence |
| LCD RST | `TFT_RST` is an RC/diode network and is not directly connected to GPIO2 | `pin_rst=2` | GPIO2 (software) | The board revision may not be reflected in the schematic, or GPIO2 may be connected through an unmarked assembly option; verify with an oscilloscope before porting |
| Touch controller | Only the FPC is shown; no controller model is specified | `Touch_GT911` | GT911 | The touch IC is likely located on the display module FPC |
| Touch INT/RST | GPIO47/GPIO48 | LovyanGFX=47/48; `touch.h`=-1/-1 | 47/48 | The complete board-level driver matches the schematic; the auxiliary library configuration may use a polling-compatible mode |
| SD CS comment | GPIO7 connects to J5.CS | Comment says “not connected to IO,” but the actual macro is 7 and mounting works | GPIO7 | Source-code comment is incorrect |
| Amplifier control polarity | Net name does not establish the active level | HIGH during initialization, LOW before playback | LOW=operating | Follow the verified audio sequence |
| Wireless/MIC selection | SGM3799 is controlled by GPIO45 | All wireless examples set it LOW | LOW=wireless | The HIGH-level MIC path still requires recording tests |
| GPIO9/10 naming | Both MIC and W_* nets appear in the schematic | Wireless uses MISO9/SCLK10 | Time-multiplexed according to GPIO45 | This is analog-switch multiplexing, not a net-naming error |
| nRF24 SS | `HSPI_SS=46` | Actual CSN is GPIO2 | CSN=2; 46 is only a placeholder for SPI begin | Do not mistakenly connect GPIO46 to nRF24 CSN |
| Wireless CS/BOOT | GPIO0 is also connected to K3 | SX1262 NSS=0 | GPIO0 | Peripheral power-up/button state may disrupt flashing and booting |
| UART expansion | J15 is TX17/RX18 | Zigbee example uses TX1/RX2 | 1/2 from the Zigbee example | This indicates that Zigbee connects to the wireless multiplexed port/a specific adapter, not J15 |
| TFT power GPIO14 | Multiple components are marked NC | No example | Do not use | This is an assembly option and cannot be inferred as an available function from the net name |

## 8. Recommended Initialization Sequence

1. Initialize the debug serial port after power-up; check the reset reason and PSRAM capacity.
2. Place shared/high-risk outputs in safe states: backlight GPIO38=LOW and amplifier GPIO21=HIGH; set GPIO45 according to the target mode.
3. Initialize the shared I2C bus: SDA15/SCL16 at 400 kHz; probe GT911 at 0x14/0x5D, RTC at 0x51, and external sensors in sequence.
4. Initialize LCD SPI2 and DMA, clear the screen, and then drive GPIO38 HIGH.
5. Initialize the dedicated SD SPI bus; validate first at 20/40 MHz, then test whether it can be increased to 80 MHz based on the card and routing.
6. Initialize wireless, microphone, or UART exclusively according to the product mode; GPIO1/2/9/10 must not be used concurrently.
7. Enable the amplifier last, after the audio data stream is established. When stopping playback, mute the amplifier before disabling I2S.
8. Start the application/network layer; all peripheral failures should return after a timeout rather than remain permanently in `while(1)` in production firmware.

## 9. Risks and Precautions

### High Risk

- **Version confusion**: The mismatch between the V1.4 filename and V1.2 title block is the primary maintenance risk. Every PCB revision must include synchronized updates to the schematic title block, BOM, board-level pin map, and this document.
- **Multiple uses of GPIO2**: LCD RST, nRF24 CSN, SX1262 RST, and Zigbee RX share this pin. Combined functionality requires pin reassignment or hardware isolation; the examples cannot simply be merged.
- **GPIO0/46 boot strapping**: If the external SX1262 NSS/BUSY signals or a button drives the wrong level during reset, the board may fail to boot or enter flashing mode normally.
- **GPIO9/10 analog switch**: The microphone and wireless SPI cannot operate simultaneously; stop the bus before switching GPIO45.
- **Differential speaker output**: Both J12 terminals are amplifier outputs; neither terminal may be connected to ground.

### Medium Risk

- Treat all ESP32 and expansion data lines as 3.3 V logic; do not connect them directly to 5 V TTL. The 5 V/VIN pins on J10/J14 are power rails and do not indicate that the data pins are 5 V tolerant.
- The I2C bus already has 4.7 kΩ pull-ups; adding strong pull-ups in parallel on external modules increases low-level sink current. Calculate the effective parallel resistance and check bus capacitance.
- SD at 80 MHz and LCD at 40 MHz are sensitive to trace length and power integrity; production testing must cover different cards, temperatures, and battery voltages.
- RF24 PA_MAX and the SX1262 140 mA setting create power-supply transients; provide adequate decoupling near the modules and test voltage drop/ripple on the 3.3 V rail.
- LVGL dual full-screen buffers consume approximately 614400 bytes of PSRAM. Allocation return values must be checked; if PSRAM is not enabled, a null-pointer crash will occur.
- The code examples contain plaintext Wi-Fi credentials and LoRaWAN parameter structures; they must be removed/replaced before release and managed through a secure configuration mechanism.

### Pending Validation

- RTC read/write operation, power-loss retention, and 0x51 address scan.
- Digital microphone capture, GPIO45=HIGH polarity, sample rate, and channel selection.
- Buzzer active polarity, appropriate frequency/duty cycle, and sound pressure level.
- GPIO14 TFT power assembly status; confirm whether the NC components are populated in the actual BOM.
- Mechanical orientation and physical silkscreen for every pin on J9/J11; schematic connections alone cannot prevent mirrored pin ordering.
- Native USB/OTG mode of Type-C J2 and the actual population of the 0 Ω configuration options.

## 10. Porting and Maintenance Checklist

- [ ] Record the physical PCB silkscreen version, module markings, display FPC model, and BOM revision.
- [ ] Use a multimeter to verify GPIO2→LCD RST, GPIO45→SGM3799, and J9/J11 pin order.
- [ ] Establish a single authoritative `board_pins.h`; do not maintain different duplicate pin macros in each example.
- [ ] Scan I2C at startup and print detected addresses; provide fallback behavior for missing devices.
- [ ] Provide self-tests and timeouts for PSRAM, LCD, touch, SD, audio, and wireless.
- [ ] Check the default reset levels of GPIO0/2/45/46 with an oscilloscope.
- [ ] Measure ripple and voltage drop on 3.3 V, VIN, and VBAT with the display at full brightness, audio at maximum output, and wireless transmission active.
- [ ] Combined-function testing must cover LCD+touch+SD, LCD+audio, and LCD+wireless; do not validate only individual lessons.
- [ ] Update the version and conflict matrix in this document whenever the schematic, BOM, or code is revised.

## 11. Minimum Board-Level Configuration Reference

```cpp
// LCD: SPI2 mode 0, SCLK42, MOSI39, DC41, CS40, RST2, 40 MHz
// Touch: I2C0 SDA15/SCL16, INT47, RST48, 400 kHz, address 0x14
// SD: SPI SCLK5/MISO4/MOSI6/CS7 (start conservatively at 20/40 MHz)
// Audio out: I2S BCLK13/LRCLK11/DOUT12, GPIO21 LOW enables path
// Backlight: GPIO38 HIGH = on
// Wireless mux: GPIO45 LOW = wireless; GPIO9/10 become MISO/SCLK
```

## 12. Source Index

- Schematic source: `1.4/ESP32 Display 3.5 inch V1.4.sch`
- PCB source: `1.4/ESP32 Display 3.5 inch V1.4.brd`
- Schematic PDF: `1.4/ESP32-Display-3.5-inch-V1.4.pdf`
- LCD/touch/LVGL: `3.5_Arduino/lesson-03/3_5LVGL/`
- SD: `3.5_Arduino/lesson-04/SD_CrowPanel_ESP32_Advance_HMI_3_5/`
- External I2C sensors: `3.5_Arduino/lesson-05/Port_CrowPanel_ESP32_Advance_HMI_3_5/`
- nRF24: `3.5_Arduino/lesson-06/READ/`, `lesson-06/WRITE/`
- SX1262/LoRaWAN: `3.5_Arduino/lesson-07/code/sendATcommands_3.5/`
- Zigbee UART bridge: `3.5_Arduino/lesson-08/zigbee_3.5/`
- Audio: `3.5_Arduino/lesson-02/OnlineAudio_small/`

---

**Maintenance Conclusion**: The current materials are sufficient to establish portable configurations for the display, touch, SD, audio output, and three types of external communication modules. However, project-level validation on actual hardware is still lacking for the RTC, digital microphone, buzzer, certain power controls, and native USB. Future maintenance should prioritize resolving uncertainty in the hardware version identifiers and GPIO2 connectivity before integrating firmware for concurrent operation of multiple peripherals.