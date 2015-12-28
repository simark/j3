/*
 * menu.h
 *
 *  Created on: Dec 28, 2015
 *      Author: simark
 */

#ifndef MASTER_FW_MENU_H_
#define MASTER_FW_MENU_H_

#include <stdint.h>
#include "input.h"
#include "beep.h"

struct page {
  void (*render)(void);
  void (*input_event)(enum input_event);
};

struct menu {
  struct page *_cur_page;
  uint8_t _cur_custom_anim_word_idx;
  volatile struct beep_state *_beep;
};

extern struct menu menu_instance;

void init_menu (volatile struct menu *, volatile struct beep_state *_beep);
uint8_t menu_active (volatile struct menu *);
void menu_input_event (volatile struct menu *state, enum input_event ev);


#endif /* MASTER_FW_MENU_H_ */
