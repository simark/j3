#include <stdint.h>
#include <string.h>
#include <avr/eeprom.h>
#include "config.h"
#include "settings.h"

static struct {
  struct {
    uint8_t menu, melody;
  } sound;

  struct settings_anim_word custom_anim_words[NUM_CUSTOM_ANIM_WORDS];
} settings;

void init_settings (void)
{
  eeprom_read_block (&settings, (const void *) 0, sizeof (settings));
}

static void settings_save (void)
{
  eeprom_update_block (&settings, (void *) 0, sizeof (settings));
}

uint8_t settings_get_sound_menu (void)
{
  return settings.sound.menu;
}

void settings_set_sound_menu (uint8_t value)
{
  settings.sound.menu = value;
  settings_save();
}

uint8_t settings_get_sound_melody (void)
{
  return settings.sound.melody;
}

void settings_set_sound_melody (uint8_t value)
{
  settings.sound.melody = value;
  settings_save();
}

const struct settings_anim_word *settings_get_custom_anim_word (uint8_t idx)
{
  return &settings.custom_anim_words[idx];
}

void settings_set_custom_anim_word (uint8_t idx, struct settings_anim_word *anim_word)
{
  memcpy (&settings.custom_anim_words[idx], anim_word, sizeof (*anim_word));
  settings_save();
}
