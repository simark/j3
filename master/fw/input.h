#ifndef MASTER_FW_INPUT_H_
#define MASTER_FW_INPUT_H_

#include "btn.h"
#include <tick.h>

enum input_event {
  IE_BTN0_SHORT,
  IE_BTN0_LONG,
  IE_BTN1_SHORT,
  IE_BTN1_LONG,
  IE_BOTH,
};

struct input_state {
  enum {
    INPUT_ALL_UNPRESSED = 0,
    INPUT_BTN0_PENDING,
    INPUT_BTN1_PENDING,
    INPUT_DONE,
  } _value;

  enum btn_state_value (*_btn0_state) (void);
  enum btn_state_value (*_btn1_state) (void);
  void (*_input_event) (enum input_event ev);

  tick_t _start, _end;
};

void init_input (volatile struct input_state *state,
                 enum btn_state_value (*btn0_state) (void),
                 enum btn_state_value (*btn1_state) (void),
                 void (*input_event) (enum input_event ev));
void input_loop (volatile struct input_state *state);

#endif /* MASTER_FW_INPUT_H_ */
