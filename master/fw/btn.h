/*
 * btn.h
 *
 *  Created on: Dec 28, 2015
 *      Author: simark
 */

#ifndef MASTER_FW_BTN_H_
#define MASTER_FW_BTN_H_

#include <tick.h>
#include "config.h"

struct btn_state {
  enum btn_state_value {
    BTN_UNPRESSED = 0,
    BTN_DEBOUNCING,
    BTN_PRESSED,
  } _value;

  tick_t _start, _end;

  uint8_t (*_read) (void);
};

void init_btn (volatile struct btn_state *state, uint8_t (*) (void));
void btn_loop (volatile struct btn_state *state);
enum btn_state_value btn_get_state (volatile struct btn_state *state);

#endif /* MASTER_FW_BTN_H_ */
