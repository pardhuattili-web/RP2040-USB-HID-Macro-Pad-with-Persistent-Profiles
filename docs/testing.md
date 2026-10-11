# Testing and Validation

## Automated tests

Run host-side tests with GCC or Clang:

```bash
cmake -S . -B build-host -DBUILD_HOST_TESTS=ON
cmake --build build-host
ctest --test-dir build-host --output-on-failure
```

The current suite covers:
- IEEE CRC-32 known vector (`123456789`).
- Debounced press edge behavior.
- Default configuration validity and CRC tamper detection.
- Key-combo press/release callback sequence.
- Text callback dispatch.
- Safe persistence fallback behavior.

## Hardware acceptance matrix

Do not mark these items as passed until tested with a physical RP2040 board.

| Test | Procedure | Expected result |
|---|---|---|
| Build | Build with supported Pico SDK and toolchain | UF2 generated without warnings/errors |
| Enumeration | Connect USB to a host computer | Device enumerates as a USB HID keyboard |
| Single key | Press button 6 in a text editor | Escape key is emitted once |
| Shortcut | Press button 1 in a text editor with selected text | Ctrl+C is emitted |
| Debounce | Tap a switch rapidly; inspect event count | One event per stable press |
| Reconnect | Unplug and reconnect USB | Device enumerates again |
| Input held at boot | Hold a button while powering up | No repeated reports until a new accepted press |
| Invalid config | Corrupt the config CRC in a host unit test | Validation rejects the record |
| Flash writes | Only after a storage backend is implemented | Writes stay inside reserved sector and recover from interrupted update |

## CI

GitHub Actions runs the host test target on each push and pull request. Cross-compilation is a separate check because the Pico SDK toolchain and SDK checkout must be installed in the CI environment.
