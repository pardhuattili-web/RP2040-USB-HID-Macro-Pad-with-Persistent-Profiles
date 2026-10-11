#ifndef MACRO_PAD_MACRO_ENGINE_H
#define MACRO_PAD_MACRO_ENGINE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MACRO_PAD_BUTTON_COUNT 6U
#define MACRO_PAD_PROFILE_COUNT 3U
#define MACRO_PAD_LABEL_LEN 16U
#define MACRO_PAD_TEXT_LEN 32U

typedef enum {
    MACRO_ACTION_KEY_COMBO = 0,
    MACRO_ACTION_TEXT = 1
} macro_action_type_t;

typedef struct {
    macro_action_type_t type;
    uint8_t keycode;
    uint8_t modifiers;
    char text[MACRO_PAD_TEXT_LEN];
    char label[MACRO_PAD_LABEL_LEN];
} macro_action_t;

typedef struct {
    char name[MACRO_PAD_LABEL_LEN];
    macro_action_t actions[MACRO_PAD_BUTTON_COUNT];
} macro_profile_t;

typedef struct {
    uint32_t schema_version;
    uint8_t active_profile;
    macro_profile_t profiles[MACRO_PAD_PROFILE_COUNT];
    uint32_t crc32;
} macro_config_t;

void macro_config_defaults(macro_config_t *config);
bool macro_config_validate(const macro_config_t *config);
uint32_t macro_config_crc(const macro_config_t *config);

typedef void (*macro_key_callback_t)(uint8_t modifiers, uint8_t keycode, bool pressed, void *context);
typedef bool (*macro_text_char_callback_t)(char character, void *context);

bool macro_execute_action(const macro_action_t *action,
                          macro_key_callback_t key_callback,
                          macro_text_char_callback_t text_callback,
                          void *context);

#endif
