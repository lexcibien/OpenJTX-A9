# OpenJTX-A9

> [!IMPORTANT]
> OpenJTX-A9 is an independent open-source implementation. This project is not affiliated with, endorsed by, or sponsored by JTXTools. All trademarks belong to their respective owners.

Alternative open-source software in Qt(multi-platform) for multimeter with bluetooth or serial connection

> [!WARNING]
> This software is currently in active development, expect bugs and not implemented features.

## JTXTools A9

### Device Specifications

TTL to USB converter: CH340K
VID: 0x1A86
PID: 0x7522

Driver macOS: <https://www.wch-ic.com/downloads/CH341SER_MAC_ZIP.html>

### First message when boot

```bash
  ESP-ROM:esp32s3-20210327
  Build:Mar 27 2021
  rst:0xc (RTC_SW_CPU_RST),boot:0x8 (SPI_FAST_FLASH_BOOT)
  Saved PC:0x420472e6
  SPIWP:0xee
  mode:DIO, clock div:1
  load:0x3fce3808,len:0x44c
  load:0x403c9700,len:0xbd8
  load:0x403cc700,len:0x2a80
  entry 0x403c98d0
```

### Serial Message specification

Send Rate: 60 Hz
Basic signal value: 0000.000000
