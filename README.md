# RP2040 USB HID Macro Pad with Persistent Profiles

![Platform](https://img.shields.io/badge/platform-RP2040-6b4fbb)
![Language](https://img.shields.io/badge/language-C11-blue)
![USB](https://img.shields.io/badge/USB-HID%20Keyboard-2ea44f)
![Build](https://img.shields.io/badge/build-Pico%20SDK-orange)

A programmable, USB-powered macro pad built around the Raspberry Pi Pico / RP2040. The firmware reads physical buttons, maps them to HID keyboard actions, and can store profile configuration in flash so the selected mappings survive a reboot.

> **Project status:** firmware foundation and host-testable core logic. USB behavior, flash layout, and the physical wiring must still be verified on the actual board before describing the device as hardware-tested.

## Features

- USB HID keyboard using the [Pico SDK](https://github.com/raspberrypi/pico-sdk) and [TinyUSB](https://github.com/hathach/tinyusb).
- Non-blocking GPIO button polling with per-button debounce.
- Six example macro buttons and profile-aware macro definitions.
- A small macro engine for key presses, modifier combinations, and short text strings.
- Configuration format with version, bounds checks, and CRC-32 integrity checking.
- Flash persistence interface isolated behind a module (the board-specific flash write is intentionally disabled until a safe, reserved flash region is configured).
- Host-side unit tests for CRC, validation, and debounce state logic.
- CMake build, compiler warnings, and GitHub Actions CI.

## Hardware

| Item | Purpose | Notes |
|---|---|---|
| Raspberry Pi Pico / compatible RP2040 board | MCU + USB device | Use a data-capable USB cable |
| 6 momentary push buttons | Macro inputs | One side to GPIO, the other to GND |
| Optional SSD1306 OLED | Status/profile display | I²C module; display driver is a future extension |
| Optional rotary encoder | Profile navigation | Not required by the current firmware foundation |
| Breadboard and jumpers | Prototype wiring | Keep wiring short and secure |

### Default button wiring

Firmware uses the RP2040 internal pull-ups; each button connects its selected GPIO to GND when pressed.

| Button | GPIO | Default example action |
|---|---:|---|
| 1 | GP2 | Ctrl+C |
| 2 | GP3 | Ctrl+V |
| 3 | GP4 | Ctrl+S |
| 4 | GP5 | Ctrl+Z |
| 5 | GP6 | Ctrl+Shift+S |
| 6 | GP7 | Escape |

The USB data connection is through the Pico's USB connector. Do not connect any GPIO directly to a voltage above 3.3 V.

## Repository layout

```text
.
├── .github/workflows/ci.yml
├── docs/
│   ├── architecture.md
│   ├── testing.md
│   └── wiring.md
├── include/
│   ├── button.h
│   ├── config_store.h
│   ├── crc32.h
│   └── macro_engine.h
├── src/
│   ├── button.c
│   ├── config_store.c
│   ├── crc32.c
│   ├── main.c
│   └── macro_engine.c
├── tests/
│   └── test_core.c
├── CMakeLists.txt
├── pico_sdk_import.cmake
├── tusb_config.h
├── .gitignore
└── LICENSE
```

## Build firmware

### Prerequisites

- Raspberry Pi Pico SDK (v2.x recommended)
- CMake and a supported C/C++ toolchain for RP2040
- Ninja or Make
- Git

The SDK must be available at `PICO_SDK_PATH`, or you can set the environment variable before configuring the project.

```bash
git clone https://github.com/pardhuattili-web/RP2040-USB-HID-Macro-Pad-with-Persistent-Profiles.git
cd RP2040-USB-HID-Macro-Pad-with-Persistent-Profiles
export PICO_SDK_PATH=/path/to/pico-sdk
git submodule update --init --recursive
cmake -S . -B build -DPICO_BOARD=pico
cmake --build build
```

The firmware build produces a `.uf2` file in the build directory. Hold BOOTSEL while plugging in the Pico, then copy the UF2 file to the mounted `RPI-RP2` drive.

> The build expects TinyUSB through the Pico SDK's `tinyusb_device` target. The standalone `pico_sdk_import.cmake` is included for a standard SDK checkout.

## Run host-side tests

The core tests use only standard C and can run without the Pico SDK or hardware:

```bash
cmake -S . -B build-host -DBUILD_HOST_TESTS=ON
cmake --build build-host
ctest --test-dir build-host --output-on-failure
```

See [docs/testing.md](docs/testing.md) for a practical validation matrix and hardware-specific checks.

## Configuration and flash safety

Persistent settings require careful flash partitioning. The RP2040 executes code from external QSPI flash; erasing a sector that contains the running application or an active boot image can corrupt firmware. This repository therefore separates configuration validation from physical flash programming. **Flash writes remain disabled by default** until a reserved sector, alignment, firmware image size limit, and power-loss strategy have been chosen and validated for the exact board/build.

The current configuration module provides:
- Versioned configuration data.
- Capacity and field validation.
- CRC-32 integrity checking.
- A backend boundary where a board-specific storage implementation can be added safely.

Do not enable a flash backend by guessing an offset.

## USB HID and macro behavior

The firmware uses the standard USB HID keyboard interface. The example firmware sends key press/release reports; it does not install drivers on the host. Macro strings are sent as USB keyboard usages and are intentionally limited to a conservative ASCII subset. Keyboard layout differences mean punctuation and symbols may appear differently on non-US host layouts.

This device emulates ordinary keyboard input. Use it only on machines and accounts where you are authorized to enter input.

## Roadmap

- [x] Define portable button, macro, CRC, and config-validation modules.
- [x] Add Pico SDK/TinyUSB project structure.
- [x] Add default six-button mapping.
- [x] Add host unit tests and CI.
- [ ] Validate USB descriptors and enumeration on a physical Pico.
- [ ] Implement a reserved-sector flash backend with power-loss-safe records.
- [ ] Add SSD1306 OLED and rotary encoder UI.
- [ ] Add a configuration utility for profile editing.
- [ ] Capture photos/video and attach a wiring diagram from the actual build.

## Troubleshooting

- **Pico not detected:** use a USB data cable, enter BOOTSEL mode, and check the board is powered.
- **Firmware won't build:** confirm `PICO_SDK_PATH` and initialize submodules.
- **Hotkeys are wrong:** verify the host keyboard layout; the sample mappings target common US-layout behavior.
- **Settings do not survive a restart:** persistent flash programming is intentionally not enabled in the default firmware.
- **Buttons trigger repeatedly:** adjust `BUTTON_DEBOUNCE_MS` and validate switch wiring to GND.

## License

MIT — see [LICENSE](LICENSE).
