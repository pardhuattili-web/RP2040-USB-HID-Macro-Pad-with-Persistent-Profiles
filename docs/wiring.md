# Wiring Guide

## Six-button prototype

Each normally-open momentary switch goes between a GPIO pin and GND. The firmware enables the RP2040 internal pull-up, so no external pull-up resistor is required for the basic short-wire prototype.

| Input | RP2040 pin | Other switch terminal |
|---|---|---|
| Button 1 | GP2 | GND |
| Button 2 | GP3 | GND |
| Button 3 | GP4 | GND |
| Button 4 | GP5 | GND |
| Button 5 | GP6 | GND |
| Button 6 | GP7 | GND |

Use the board's GND pin as the common ground. Do not feed 5 V into RP2040 GPIO; GPIO is 3.3 V logic.

## Optional OLED (future extension)

An SSD1306 I²C OLED commonly uses VCC, GND, SDA, and SCL. Before connecting it, confirm the exact module's voltage requirements and I²C pinout. The current firmware does not initialize or use the OLED.

## Bring-up sequence

1. Build and flash firmware with buttons disconnected.
2. Confirm the Pico enumerates as a USB keyboard using a known-safe test computer.
3. Connect one button to GP2 and GND; confirm it sends Ctrl+C only after a debounced press.
4. Connect remaining buttons one at a time and verify the mapping table.
5. Record a wiring photo and add it to this directory when the real hardware is assembled.

## Safety and test hygiene

Test on a text editor or other benign application first. Key mappings can trigger actions in whichever application currently has focus; avoid testing with elevated terminals, destructive dialogs, or production systems.
