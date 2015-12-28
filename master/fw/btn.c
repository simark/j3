#include "btn.h"

void init_btn (volatile struct btn_state *state, uint8_t (*read) (void))
{
  state->_value = BTN_UNPRESSED;
  state->_read = read;
}

void btn_loop (volatile struct btn_state *state)
{
  uint8_t pin_value = state->_read ();

  switch (state->_value) {
  case BTN_UNPRESSED:
    if (pin_value) {
      state->_value = BTN_DEBOUNCING;
      state->_start = get_tick ();
      state->_end = get_tick () + BTN_DEBOUNCE_TICKS;
    } else {
      state->_value = BTN_UNPRESSED;
    }
    break;

  case BTN_DEBOUNCING:
    if (pin_value) {
      if (tick_expired (state->_start, state->_end)) {
        // It looks like the real thing!
        state->_value = BTN_PRESSED;
      } else {
        // Wait some more.
        state->_value = BTN_DEBOUNCING;
      }
    } else {
      state->_value = BTN_UNPRESSED;
    }
    break;

  case BTN_PRESSED:
    if (pin_value) {
      state->_value = BTN_PRESSED;
    } else {
      state->_value = BTN_UNPRESSED;
    }
    break;
  }
}

enum btn_state_value btn_get_state (volatile struct btn_state *state)
{
  return state->_value;
}
