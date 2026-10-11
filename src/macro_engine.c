#include "macro_engine.h"
#include "crc32.h"

#include <string.h>

#define CONFIG_SCHEMA_VERSION 1U
#define MOD_LCTRL 0x01U
#define MOD_LSHIFT 0x02U
#define KEY_C 0x06U
#define KEY_V 0x19U
#define KEY_S 0x16U
#define KEY_Z 0x1DU
#define KEY_ESC 0x29U

static void set_combo(macro_action_t *action, const char *label, uint8_t modifiers, uint8_t keycode)
{
    memset(action, 0, sizeof(*action));
    action->type = MACRO_ACTION_KEY_COMBO;
    action->modifiers = modifiers;
    action->keycode = keycode;
    (void)strncpy(action->label, label, sizeof(action->label) - 1U);
}

void macro_config_defaults(macro_config_t *config)
{
    if (config == NULL) {
        return;
    }
    memset(config, 0, sizeof(*config));
    config->schema_version = CONFIG_SCHEMA_VERSION;
    config->active_profile = 0U;

    (void)strncpy(config->profiles[0].name, "Productivity", sizeof(config->profiles[0].name) - 1U);
    set_combo(&config->profiles[0].actions[0], "Copy", MOD_LCTRL, KEY_C);
    set_combo(&config->profiles[0].actions[1], "Paste", MOD_LCTRL, KEY_V);
    set_combo(&config->profiles[0].actions[2], "Save", MOD_LCTRL, KEY_S);
    set_combo(&config->profiles[0].actions[3], "Undo", MOD_LCTRL, KEY_Z);
    set_combo(&config->profiles[0].actions[4], "Save As", (uint8_t)(MOD_LCTRL | MOD_LSHIFT), KEY_S);
    set_combo(&config->profiles[0].actions[5], "Escape", 0U, KEY_ESC);

    (void)strncpy(config->profiles[1].name, "Development", sizeof(config->profiles[1].name) - 1U);
    set_combo(&config->profiles[1].actions[0], "Copy", MOD_LCTRL, KEY_C);
    set_combo(&config->profiles[1].actions[1], "Paste", MOD_LCTRL, KEY_V);
    set_combo(&config->profiles[1].actions[2], "Save", MOD_LCTRL, KEY_S);
    set_combo(&config->profiles[1].actions[3], "Undo", MOD_LCTRL, KEY_Z);
    set_combo(&config->profiles[1].actions[4], "Terminal", MOD_LCTRL, KEY_ESC);
    set_combo(&config->profiles[1].actions[5], "Escape", 0U, KEY_ESC);

    (void)strncpy(config->profiles[2].name, "General", sizeof(config->profiles[2].name) - 1U);
    set_combo(&config->profiles[2].actions[0], "Copy", MOD_LCTRL, KEY_C);
    set_combo(&config->profiles[2].actions[1], "Paste", MOD_LCTRL, KEY_V);
    set_combo(&config->profiles[2].actions[2], "Save", MOD_LCTRL, KEY_S);
    set_combo(&config->profiles[2].actions[3], "Undo", MOD_LCTRL, KEY_Z);
    set_combo(&config->profiles[2].actions[4], "Escape", 0U, KEY_ESC);
    set_combo(&config->profiles[2].actions[5], "Escape", 0U, KEY_ESC);

    config->crc32 = macro_config_crc(config);
}

uint32_t macro_config_crc(const macro_config_t *config)
{
    if (config == NULL) {
        return 0U;
    }
    return crc32_compute(config, offsetof(macro_config_t, crc32));
}

bool macro_config_validate(const macro_config_t *config)
{
    if (config == NULL || config->schema_version != CONFIG_SCHEMA_VERSION ||
        config->active_profile >= MACRO_PAD_PROFILE_COUNT ||
        config->crc32 != macro_config_crc(config)) {
        return false;
    }

    for (size_t p = 0U; p < MACRO_PAD_PROFILE_COUNT; ++p) {
        if (memchr(config->profiles[p].name, '\0', sizeof(config->profiles[p].name)) == NULL) {
            return false;
        }
        for (size_t i = 0U; i < MACRO_PAD_BUTTON_COUNT; ++i) {
            const macro_action_t *action = &config->profiles[p].actions[i];
            if (action->type != MACRO_ACTION_KEY_COMBO && action->type != MACRO_ACTION_TEXT) {
                return false;
            }
            if (memchr(action->label, '\0', sizeof(action->label)) == NULL ||
                memchr(action->text, '\0', sizeof(action->text)) == NULL) {
                return false;
            }
        }
    }
    return true;
}

bool macro_execute_action(const macro_action_t *action,
                          macro_key_callback_t key_callback,
                          macro_text_char_callback_t text_callback,
                          void *context)
{
    if (action == NULL) {
        return false;
    }

    if (action->type == MACRO_ACTION_KEY_COMBO) {
        if (key_callback == NULL || action->keycode == 0U) {
            return false;
        }
        key_callback(action->modifiers, action->keycode, true, context);
        key_callback(0U, 0U, false, context);
        return true;
    }

    if (action->type == MACRO_ACTION_TEXT) {
        if (text_callback == NULL) {
            return false;
        }
        for (size_t i = 0U; i < sizeof(action->text) && action->text[i] != '\0'; ++i) {
            if (!text_callback(action->text[i], context)) {
                return false;
            }
        }
        return true;
    }
    return false;
}
