#include "input.h"

void init_input (volatile struct input_state *state,
                 enum btn_state_value (*btn0_state) (void),
                 enum btn_state_value (*btn1_state) (void),
                 void (*input_event) (enum input_event ev))
{
  state->_value = INPUT_ALL_UNPRESSED;
  state->_btn0_state = btn0_state;
  state->_btn1_state = btn1_state;
  state->_input_event = input_event;
}

void input_loop (volatile struct input_state *state)
{
  enum btn_state_value btn0 = state->_btn0_state ();
  enum btn_state_value btn1 = state->_btn1_state ();

  switch (state->_value) {
  case INPUT_ALL_UNPRESSED:
    if (btn0 == BTN_PRESSED) {
      state->_start = get_tick ();
      state->_end = get_tick () + INPUT_LONG_PRESS_TICKS;
      state->_value = INPUT_BTN0_PENDING;
    } else if (btn1 == BTN_PRESSED) {
      state->_start = get_tick ();
      state->_end = get_tick () + INPUT_LONG_PRESS_TICKS;
      state->_value = INPUT_BTN1_PENDING;
    } else {
      state->_value = INPUT_ALL_UNPRESSED;
    }
    break;

  case INPUT_BTN0_PENDING:
    if (btn0 == BTN_UNPRESSED) {
      state->_input_event (IE_BTN0_SHORT);
      state->_value = INPUT_DONE;
    } else if (tick_expired (state->_start,
                             state->_end)) {
      if (btn1 == BTN_PRESSED) {
        state->_input_event (IE_BOTH);
        state->_value = INPUT_DONE;
      } else {
        state->_input_event  (IE_BTN0_LONG);
        state->_value = INPUT_DONE;
      }
    } else {
      state->_value = INPUT_BTN0_PENDING;
    }
    break;

  case INPUT_BTN1_PENDING:
    if (btn1 == BTN_UNPRESSED) {
      state->_input_event  (IE_BTN1_SHORT);
      state->_value = INPUT_DONE;
    } else if (tick_expired (state->_start,
                             state->_end)) {
      if (btn0 == BTN_PRESSED) {
        state->_input_event  (IE_BOTH);
        state->_value = INPUT_DONE;
      } else {
        state->_input_event  (IE_BTN1_LONG);
        state->_value = INPUT_DONE;
      }
    } else {
      state->_value = INPUT_BTN1_PENDING;
    }
    break;

  case INPUT_DONE:
    if (btn0 == BTN_UNPRESSED
        && btn1 == BTN_UNPRESSED) {
      state->_value = INPUT_ALL_UNPRESSED;
    } else {
      state->_value = INPUT_DONE;
    }
    break;
  }
}
