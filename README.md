
# ATOMS3 HEOS Control

This project is an example to demonstrate HEOS device control with ATOMS3 / ATOMS3 Lite. Following commands in HEOS CLI Protocol are assignable to the button on ATOMS3 / ATOMS3 Lite.

* SetVolume
* VolumeUp
* VolumeDown
* SetMute (Mute/Unmute)
* ToggleMute
* PlayInputSource

## Notes

Please modify 'ssid', 'password' and 'heosdevice' in main.cpp before build.

## Supported Hardware

* [ATOMS3](https://docs.m5stack.com/en/core/AtomS3)
* [ATOMS3 Lite](https://docs.m5stack.com/en/core/AtomS3%20Lite)

## Build Environment

* PlatformIO

## Dependencies

* bblanchon/ArduinoJson@^6.21.1
* m5stack/M5AtomS3 @ ^0.0.2
* fastled/FastLED @ ^3.5.0

## Reference

* HEOS CLI Protocol Specification v1.17
  * Please find pdf files from Denon, Marantz or B&W webpages
