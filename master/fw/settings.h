/*
 * settings.h
 *
 *  Created on: Dec 28, 2015
 *      Author: simark
 */

#ifndef MASTER_FW_SETTINGS_H_
#define MASTER_FW_SETTINGS_H_

#include <stdint.h>

struct settings_anim_word {
  struct anim_word anim_word;
  struct slave_seq slave_seq;
  uint8_t enable;
};

void init_settings (void);

uint8_t settings_get_sound_menu (void);
void settings_set_sound_menu (uint8_t value);
uint8_t settings_get_sound_melody (void);
void settings_set_sound_melody (uint8_t value);
const struct settings_anim_word *settings_get_custom_anim_word (uint8_t idx);
void settings_set_custom_anim_word (uint8_t idx, struct settings_anim_word *anim_word);

#endif /* MASTER_FW_SETTINGS_H_ */
