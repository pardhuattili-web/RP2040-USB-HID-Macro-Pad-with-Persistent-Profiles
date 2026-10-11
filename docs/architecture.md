# Firmware Architecture

## Module boundaries

- **`main.c`** initializes GPIO and TinyUSB, services the USB device task, polls inputs, and dispatches selected macro actions.
- **`button.c`** contains a platform-independent debouncing state machine. It emits an event only on a debounced transition to pressed.
- **`macro_engine.c`** owns the macro/profile data model, defaults, validation, CRC coverage, and generic execution callbacks.
- **`crc32.c`** computes IEEE CRC-32 for detecting damaged configuration records.
- **`config_store.c`** isolates persistence. The current backend intentionally returns `CONFIG_STORE_UNAVAILABLE` rather than erasing an unreserved flash sector.

## Event flow

1. The main loop services TinyUSB with `tud_task()`.
2. GPIO inputs are sampled using pull-ups; pressed switches read low.
3. A stable press is accepted after the debounce interval.
4. The active profile action is dispatched.
5. The callback emits HID reports through TinyUSB.

USB report callbacks are wired in `main.c`; the report descriptor is a standard keyboard descriptor from TinyUSB.

## Configuration integrity

The stored structure contains a schema version, active-profile index, profile names, actions, and CRC-32. The CRC covers the structure bytes before the CRC field. Validation checks the schema, profile bounds, string termination, action type, and CRC.

**Portability note:** the structure is currently built and tested with the same compiler/ABI as the firmware. If it becomes a long-lived on-flash format, replace raw structure storage with explicit byte serialization so compiler padding and endianness do not become persistent-format dependencies.

## Flash design decision

RP2040 firmware generally executes from external QSPI flash. Safe persistent storage must reserve an aligned erase sector and ensure the linker-generated application image never overlaps it. This repository does not guess a partition offset; hardware flash writes are disabled until a board-specific partition is documented and validated. A production version should use two records or a journal to recover from power loss while updating settings.

## Current limitations

- Profiles are defined in firmware defaults; there is no OLED/encoder menu yet.
- The text path supports a small ASCII subset and assumes a US keyboard layout.
- The demonstrator has no host-side profile editor.
- Physical USB enumeration, HID behavior, and electrical wiring need hands-on validation.
