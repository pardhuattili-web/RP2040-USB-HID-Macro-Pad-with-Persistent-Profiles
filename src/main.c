#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "tusb.h"

#include "button.h"
#include "config_store.h"
#include "macro_engine.h"

#define BUTTON_DEBOUNCE_MS 25U

static const uint8_t button_gpio[MACRO_PAD_BUTTON_COUNT] = {2U, 3U, 4U, 5U, 6U, 7U};
static button_state_t button_states[MACRO_PAD_BUTTON_COUNT];
static macro_config_t app_config;

static void send_report(uint8_t modifiers, uint8_t keycode)
{
    if (!tud_hid_ready()) {
        return;
    }
    uint8_t keycodes[6] = {0U};
    if (keycode != 0U) {
        keycodes[0] = keycode;
    }
    (void)tud_hid_keyboard_report(0U, modifiers, keycodes);
}

static void hid_key_callback(uint8_t modifiers, uint8_t keycode, bool pressed, void *context)
{
    (void)context;
    send_report(pressed ? modifiers : 0U, pressed ? keycode : 0U);
}

static bool ascii_to_hid(char character, uint8_t *modifier, uint8_t *keycode)
{
    *modifier = 0U;
    if (character >= 'a' && character <= 'z') {
        *keycode = (uint8_t)(0x04U + (uint8_t)(character - 'a'));
    } else if (character >= 'A' && character <= 'Z') {
        *keycode = (uint8_t)(0x04U + (uint8_t)(character - 'A'));
        *modifier = 0x02U;
    } else if (character >= '1' && character <= '9') {
        *keycode = (uint8_t)(0x1EU + (uint8_t)(character - '1'));
    } else if (character == '0') {
        *keycode = 0x27U;
    } else if (character == ' ') {
        *keycode = 0x2CU;
    } else if (character == '\n') {
        *keycode = 0x28U;
    } else {
        return false;
    }
    return true;
}

static bool hid_text_callback(char character, void *context)
{
    (void)context;
    uint8_t modifier = 0U;
    uint8_t keycode = 0U;
    if (!ascii_to_hid(character, &modifier, &keycode) || !tud_hid_ready()) {
        return false;
    }
    send_report(modifier, keycode);
    sleep_ms(2U);
    send_report(0U, 0U);
    sleep_ms(2U);
    return true;
}

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type,
                           uint8_t const *buffer, uint16_t bufsize)
{
    (void)instance;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)bufsize;
}

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type,
                               uint8_t *buffer, uint16_t reqlen)
{
    (void)instance;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)reqlen;
    return 0U;
}

void tud_mount_cb(void) {}
void tud_umount_cb(void) {}

uint8_t const desc_hid_report[] = {TUD_HID_REPORT_DESC_KEYBOARD()};

uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance)
{
    (void)instance;
    return desc_hid_report;
}

int main(void)
{
    stdio_init_all();

    for (size_t i = 0U; i < MACRO_PAD_BUTTON_COUNT; ++i) {
        gpio_init(button_gpio[i]);
        gpio_set_dir(button_gpio[i], GPIO_IN);
        gpio_pull_up(button_gpio[i]);
        button_state_init(&button_states[i], BUTTON_DEBOUNCE_MS);
    }

    (void)config_store_load(&app_config);
    if (!macro_config_validate(&app_config)) {
        macro_config_defaults(&app_config);
    }

    tusb_init();

    while (true) {
        tud_task();
        const uint32_t now_ms = to_ms_since_boot(get_absolute_time());

        if (tud_hid_ready()) {
            for (size_t i = 0U; i < MACRO_PAD_BUTTON_COUNT; ++i) {
                const bool raw_pressed = gpio_get(button_gpio[i]) == 0;
                if (button_state_update(&button_states[i], raw_pressed, now_ms)) {
                    const macro_action_t *action =
                        &app_config.profiles[app_config.active_profile].actions[i];
                    (void)macro_execute_action(action, hid_key_callback, hid_text_callback, NULL);
                }
            }
        }

        sleep_ms(1U);
    }
}
