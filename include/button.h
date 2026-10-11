#ifndef MACRO_PAD_BUTTON_H
#define MACRO_PAD_BUTTON_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    bool stable_pressed;
    bool last_raw_pressed;
    uint32_t raw_changed_at_ms;
    uint32_t debounce_ms;
} button_state_t;

void button_state_init(button_state_t *state, uint32_t debounce_ms);
/* Returns true exactly once when a debounced press edge is accepted. */
bool button_state_update(button_state_t *state, bool raw_pressed, uint32_t now_ms);

#endif
