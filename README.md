# RP2040 USB HID Macro Pad with Persistent Profiles

![Platform](https://img.shields.io/badge/platform-RP2040-6b4fbb)
![Language](https://img.shields.io/badge/language-C11-blue)
![USB](https://img.shields.io/badge/USB-HID%20Keyboard-2ea44f)
![Tests](https://img.shields.io/badge/tests-host%20unit%20tests-informational)

A programmable USB macro pad built around the Raspberry Pi Pico / RP2040. Six GPIO buttons trigger keyboard shortcuts through a standard USB HID keyboard interface. The firmware foundation includes a profile data model, input debouncing, configuration integrity checks, and portable host-side tests.

> **Status:** firmware foundation and host-side core tests are implemented. Physical board bring-up, USB enumeration, and a real flash-persistence backend remain hardware validation tasks. The project intentionally does not erase an assumed flash address.

## Implemented features

- USB HID keyboard using the Pico SDK and TinyUSB.
- Six active-low momentary button inputs with internal pull-ups and a non-blocking debounce state machine.
- Example productivity shortcuts: Copy, Paste, Save, Undo, Save As, and Escape.
- Three profile data structures with default mappings.
- Macro dispatch for key combinations and conservative ASCII text callbacks.
- Configuration schema/version checks and CRC-32 validation.
- A storage abstraction that safely falls back to defaults; flash writing is disabled until a proper reserved-sector strategy is implemented.
- CMake build targets, host unit tests, test documentation, wiring documentation, and GitHub Actions CI.

## Hardware requirements

| Part | Quantity | Purpose |
|---|---:|---|
| Raspberry Pi Pico or compatible RP2040 board | 1 | MCU and USB device |
| Normally-open momentary push buttons | 6 | Macro triggers |
| Breadboard and jumper wires | As needed | Prototyping |
| USB data cable | 1 | Power, programming, and USB HID |

An SSD1306 OLED and rotary encoder are planned extensions; current firmware does not use them yet.

## Default wiring

Each switch connects between its GPIO and GND. Internal pull-ups are enabled in firmware, so an unpressed switch reads high and a pressed switch reads low.

| Button | GPIO | Default action |
|---|---:|---|
| 1 | GP2 | Ctrl+C |
| 2 | GP3 | Ctrl+V |
| 3 | GP4 | Ctrl+S |
| 4 | GP5 | Ctrl+Z |
| 5 | GP6 | Ctrl+Shift+S |
| 6 | GP7 | Escape |

Do not apply 5 V directly to RP2040 GPIO. Check the exact board pinout before wiring.

## Repository layout

```text
.github/workflows/ci.yml  Host test CI
docs/architecture.md      Module design and data flow
docs/testing.md           Automated and hardware validation plan
docs/wiring.md             Wiring and bring-up guide
include/                   Public module interfaces
src/                       Firmware and portable C modules
tests/test_core.c          Host-side unit tests
CMakeLists.txt             Pico SDK + host test build targets
tusb_config.h              TinyUSB configuration
pico_sdk_import.cmake      Pico SDK import
```

## Host-side tests

No Pico board or Pico SDK is needed to run the portable core tests. Install CMake and GCC/Clang, then run:

```bash
cmake -S . -B build-host -DBUILD_HOST_TESTS=ON
cmake --build build-host
ctest --test-dir build-host --output-on-failure
```

The suite covers CRC-32, debounced input edges, configuration validity and tamper detection, macro dispatch callbacks, and safe storage fallback behavior.

## Build firmware for Pico

### Prerequisites

- Raspberry Pi Pico SDK (v2.x recommended)
- Supported ARM embedded toolchain
- CMake and Ninja or Make

Set `PICO_SDK_PATH` to your local SDK checkout. Ensure TinyUSB is available through the Pico SDK's normal submodule setup.

```bash
git clone https://github.com/pardhuattili-web/RP2040-USB-HID-Macro-Pad-with-Persistent-Profiles.git
cd RP2040-USB-HID-Macro-Pad-with-Persistent-Profiles
export PICO_SDK_PATH=/path/to/pico-sdk
cmake -S . -B build -DPICO_BOARD=pico
cmake --build build
```

The build should generate a `.uf2` output in the build directory. To flash it, hold BOOTSEL while connecting the Pico, then copy the UF2 to the mounted `RPI-RP2` volume.

## Flash persistence: important limitation

The configuration schema and validation layer are ready for a storage backend, but `config_store_save()` currently returns `CONFIG_STORE_UNAVAILABLE` and `config_store_load()` installs defaults. This is deliberate: the RP2040 runs code from external QSPI flash, so erasing a guessed sector can corrupt the firmware image.

A proper persistence implementation must reserve an erase-aligned sector outside the executable image, enforce a maximum application size or linker partition, and use a recoverable update strategy (for example, two CRC-protected records). Do not describe persistence as operational until that backend has been implemented and power-loss tested.

## Current limitations

- Physical USB enumeration and key behavior must be verified on a real board.
- Profiles are currently defined in code; no OLED/rotary UI or host profile editor yet.
- Text input maps only lowercase/uppercase letters, digits, space, and newline. It assumes a US keyboard layout.
- The current demonstrator's development-profile “Terminal” mapping is Ctrl+Escape, a placeholder rather than a universal terminal shortcut.
- Flash save/load is not implemented beyond safe defaults and validation.

## Validation roadmap

- [x] Portable button debounce logic
- [x] Three-profile configuration structure and defaults
- [x] CRC-32 and schema validation
- [x] Macro dispatch callbacks and host unit tests
- [x] GitHub Actions CI for host core
- [ ] Cross-build with a supported Pico SDK/toolchain
- [ ] Verify descriptor, enumeration, and shortcuts on hardware
- [ ] Implement safe reserved-sector persistence
- [ ] Add OLED and rotary encoder profile navigation
- [ ] Attach photos/video of physical prototype

## Documentation

- [Architecture](docs/architecture.md)
- [Wiring guide](docs/wiring.md)
- [Testing plan](docs/testing.md)

## License

MIT — see [LICENSE](LICENSE).
