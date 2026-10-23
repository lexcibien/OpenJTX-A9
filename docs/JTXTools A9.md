# JTXTools A9

## Device Specifications

TTL to USB converter: CH340K
VID: 0x1A86
PID: 0x7522

Driver macOS: <https://www.wch-ic.com/downloads/CH341SER_MAC_ZIP.html>

## Linux serial port permissions

The USB serial converter usually appears as `/dev/ttyUSB0` (the number can
differ). Connect the device and check which port and group were assigned:

```sh
lsusb -d 1a86:7522
ls -l /dev/ttyUSB*
```

The `lsusb` output should include `1a86:7522`. In the `ls -l` output, the
group name is shown after the owner (commonly `dialout` on Debian/Ubuntu and
`uucp` on Arch Linux). Add your user to that group; for example:

```sh
sudo usermod -aG dialout "$USER"
```

Replace `dialout` with the group shown for the serial device if it differs.
Log out of your desktop session and log back in for the new group membership
to take effect, then unplug and reconnect the device. Verify that the group is
active with `id -nG` and that your user can read and write the device node:

```sh
test -r /dev/ttyUSB0 && test -w /dev/ttyUSB0 && echo "Serial access OK"
```

Replace `/dev/ttyUSB0` with the port found on your system. Then start
OpenJTX-A9 normally (do not run it with `sudo`), select that serial port, and
connect. The application offers `115200` baud among its baud-rate options.

If the device does not appear, try another USB data cable or port and check
`lsusb` again. If it appears but the application reports a permission error,
check the device node's group and repeat the group-membership steps. Close
other serial terminal programs that may already have the port open.

## First message when boot

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

## Serial Message specification

Send Rate: 60 Hz
Basic signal value: 0000.000000
