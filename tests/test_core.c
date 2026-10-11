#include "button.h"
#include "config_store.h"
#include "crc32.h"
#include "macro_engine.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    unsigned presses;
    unsigned releases;
    uint8_t last_modifiers;
    uint8_t last_keycode;
} key_capture_t;

static void capture_key(uint8_t modifiers, uint8_t keycode, bool pressed, void *context)
{
    key_capture_t *capture = (key_capture_t *)context;
    if (pressed) {
        capture->presses++;
        capture->last_modifiers = modifiers;
        capture->last_keycode = keycode;
    } else {
        capture->releases++;
    }
}

static bool consume_char(char character, void *context)
{
    char *buffer = (char *)context;
    size_t length = strlen(buffer);
    if (length + 1U >= 64U) {
        return false;
    }
    buffer[length] = character;
    buffer[length + 1U] = '\0';
    return true;
}

static void test_crc32(void)
{
    const char *text = "123456789";
    assert(crc32_compute(text, strlen(text)) == 0xCBF43926u);
    assert(crc32_compute(NULL, 0U) == 0u);
}

static void test_debounce(void)
{
    button_state_t state;
    button_state_init(&state, 20U);
    assert(!button_state_update(&state, true, 100U));
    assert(!button_state_update(&state, true, 119U));
    assert(button_state_update(&state, true, 120U));
    assert(!button_state_update(&state, true, 121U));

    assert(!button_state_update(&state, false, 130U));
    assert(!button_state_update(&state, false, 149U));
    assert(!button_state_update(&state, false, 150U));
    assert(!state.stable_pressed);
    assert(!button_state_update(&state, false, 151U));
}

static void test_config_validation(void)
{
    macro_config_t config;
    macro_config_defaults(&config);
    assert(macro_config_validate(&config));

    config.active_profile = MACRO_PAD_PROFILE_COUNT;
    assert(!macro_config_validate(&config));

    macro_config_defaults(&config);
    config.profiles[0].actions[0].keycode ^= 1U;
    assert(!macro_config_validate(&config));

    assert(!macro_config_validate(NULL));
}

static void test_macro_dispatch(void)
{
    macro_config_t config;
    key_capture_t capture = {0};
    macro_config_defaults(&config);

    assert(macro_execute_action(&config.profiles[0].actions[0], capture_key, NULL, &capture));
    assert(capture.presses == 1U);
    assert(capture.releases == 1U);
    assert(capture.last_modifiers == 0x01U);
    assert(capture.last_keycode == 0x06U);

    macro_action_t text_action = {0};
    text_action.type = MACRO_ACTION_TEXT;
    (void)strncpy(text_action.text, "abc", sizeof(text_action.text) - 1U);
    char output[64] = {0};
    assert(macro_execute_action(&text_action, NULL, consume_char, output));
    assert(strcmp(output, "abc") == 0);
    assert(!macro_execute_action(&text_action, NULL, NULL, NULL));
    assert(!macro_execute_action(NULL, capture_key, consume_char, &capture));
}

static void test_store_defaults_safe(void)
{
    macro_config_t config;
    assert(config_store_load(&config) == CONFIG_STORE_UNAVAILABLE);
    assert(macro_config_validate(&config));
    assert(config_store_save(&config) == CONFIG_STORE_UNAVAILABLE);
    assert(config_store_load(NULL) == CONFIG_STORE_INVALID);
}

int main(void)
{
    test_crc32();
    test_debounce();
    test_config_validation();
    test_macro_dispatch();
    test_store_defaults_safe();
    puts("All core tests passed.");
    return 0;
}
