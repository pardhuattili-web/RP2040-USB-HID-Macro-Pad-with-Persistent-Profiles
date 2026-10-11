#include "button.h"

#include <stddef.h>

void button_state_init(button_state_t *state, uint32_t debounce_ms)
{
    if (state == NULL) {
        return;
    }
    state->stable_pressed = false;
    state->last_raw_pressed = false;
    state->raw_changed_at_ms = 0U;
    state->debounce_ms = debounce_ms;
}

bool button_state_update(button_state_t *state, bool raw_pressed, uint32_t now_ms)
{
    if (state == NULL) {
        return false;
    }

    if (raw_pressed != state->last_raw_pressed) {
        state->last_raw_pressed = raw_pressed;
        state->raw_changed_at_ms = now_ms;
    }

    if ((raw_pressed != state->stable_pressed) &&
        ((uint32_t)(now_ms - state->raw_changed_at_ms) >= state->debounce_ms)) {
        state->stable_pressed = raw_pressed;
        return state->stable_pressed;
    }

    return false;
}
