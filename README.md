# Pill-FW

## Requirements
- run and tested on M-series tahoe 26.3.1, will need someone to possibly test it on windows
- PlatformIO
- PlatformIO Cursor/VSCode extension
  - Cursor's PlatformIO extension is broken as fuck, so just use pioarduino IDE instead (its a fork)

## How to Build
- use the PlatformIO GUI's build function under General > Build
- do it manually using: `platformio run --environment seeed_xiao_esp32c6`