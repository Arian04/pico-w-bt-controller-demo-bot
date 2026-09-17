# Pico W BT Controller Demo Bot

A robotics demo bot running on the Raspberry Pi Pico W, controlled over Bluetooth using a controller via the Bluepad32
library.

## Setup & Prerequisites

1. **Pico SDK**:
   Follow [README_Bluepad32.md](README_Bluepad32.md) for instructions on installing the Pico SDK and setting your
   `PICO_SDK_PATH` environment variable.

2. **Submodules**:
   Clone or pull submodules (required for Bluepad32):
   ```bash
   git submodule update --init --recursive
   ```

3. **Tools**:
    - `cmake` and ARM GCC toolchain (`arm-none-eabi-gcc`)
    - `picotool` - for flashing binaries
    - Optional: [`just`](https://github.com/casey/just) - for running the recipes in our "justfile"
    - Optional: `picocom` - for serial monitoring

## Justfile Usage

Use `just` to run common project tasks:

- **Build project**:
  ```bash
  just build
  ```
  Generates build files and compiles the project to a build directory.

- **Upload to Pico**:
  ```bash
  just upload
  ```
  Flashes the compiled UF2 binary to the connected Pico via `picotool`.

- **Serial Monitor**:
  ```bash
  just monitor
  ```
  Opens the USB serial monitor on `/dev/ttyACM0` using `picocom`. NOTE: you may need to edit the port, or
  even the command if you're using a different serial port communication program.