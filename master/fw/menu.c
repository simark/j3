#include "menu.h"

#include <stdlib.h>

#include "beep.h"
#include "settings.h"
#include "config.h"

struct menu menu_instance;

void init_menu (volatile struct menu *state, volatile struct beep_state *beep)
{
  state->_cur_page = NULL;
  state->_beep = beep;
}

uint8_t menu_active (volatile struct menu *state)
{
  return state->_cur_page != NULL;
}

static void _menu_beep (volatile struct menu *state, struct beep_note *song)
{
  if (settings_get_sound_menu ()) {
    beep (state->_beep, song);
  }
}

static struct beep_note song_down[] = {
  { MS_TO_TICKS(50), 5 },
  { 0, 0 },
};

static struct beep_note song_up[] = {
  { MS_TO_TICKS(50), 7 },
  { 0, 0 },
};

static struct beep_note song_back[] = {
  { MS_TO_TICKS(50), 5 },
  { MS_TO_TICKS(50), 7 },
  { 0, 0 },
};

static struct beep_note song_enter[] = {
  { MS_TO_TICKS(50), 7 },
  { MS_TO_TICKS(50), 5 },
  { 0, 0 },
};

static struct beep_note song_exit_menu[] = {
  { MS_TO_TICKS(50), 5 },
  { MS_TO_TICKS(50), 6 },
  { MS_TO_TICKS(50), 7 },
  { 0, 0 },
};

static struct beep_note song_enter_menu[] = {
  { MS_TO_TICKS(50), 7 },
  { MS_TO_TICKS(50), 6 },
  { MS_TO_TICKS(50), 5 },
  { 0, 0 },
};

extern struct page page_sound;

static void _menu_open (volatile struct menu *state)
{
  state->_cur_page = &page_sound;
  _menu_beep (state, song_enter_menu);
}

static void _menu_close (volatile struct menu *state)
{
  state->_cur_page = NULL;
  _menu_beep (state, song_exit_menu);
}

void menu_input_event (volatile struct menu *state, enum input_event ev)
{

  if (menu_active (state)) {
    if (ev == IE_EXIT) {
      _menu_close (state);
    } else {
      state->_cur_page->input_event(ev);
    }

    /* If the input event did not cause the menu to exit (and therefore the
     * menu exit sound to play), play the sound corresponding to the button. */
    if (menu_active (state)) {
      switch (ev) {
      case IE_UP:
        _menu_beep (state, song_up);
        break;

      case IE_ENTER:
        _menu_beep (state, song_enter);
        break;

      case IE_DOWN:
        _menu_beep (state, song_down);
        break;

      case IE_BACK:
        _menu_beep (state, song_back);
        break;

      case IE_EXIT:
        break;
      }
    }
  } else {
    if (ev == IE_MENU) {
      _menu_open (state);
    }
  }
}

#define PAGE_RENDER(name) \
static void page_##name##_render (void)

#define PAGE_INPUT_EVENT(name) \
static void page_##name##_input_event (enum input_event ev)

#define PAGE(name) \
extern struct page page_##name; \
PAGE_RENDER(name); \
PAGE_INPUT_EVENT(name); \
struct page page_##name = { \
  page_##name##_render, \
  page_##name##_input_event, \
}

PAGE(about);
PAGE(about_scroll);
PAGE(custom);
PAGE(custom_select);
PAGE(custom_show);
PAGE(custom_show_show);
PAGE(custom_set);
PAGE(custom_enable);
PAGE(custom_chars);
PAGE(custom_chars_edit);
PAGE(custom_anim);
PAGE(custom_anim_edit);
PAGE(sound);
PAGE(sound_menu);
PAGE(sound_melody);

#define PAGE_GOTO(input, dest) \
case input: \
  menu_instance._cur_page = &page_##dest; \
  break

#define MENU_QUIT(input) \
case input: \
  menu_instance._cur_page = NULL; \
  break

PAGE_RENDER(about)
{

}

PAGE_INPUT_EVENT(about)
{
  switch (ev) {
  PAGE_GOTO(IE_UP, custom);
  PAGE_GOTO(IE_DOWN, sound);
  PAGE_GOTO(IE_ENTER, about_scroll);
  MENU_QUIT(IE_BACK);

  default:
    break;
  }
}


PAGE_RENDER(about_scroll)
{

}

PAGE_INPUT_EVENT(about_scroll)
{
  switch (ev) {
  PAGE_GOTO(IE_BACK, about);
  PAGE_GOTO(IE_ENTER, about);
  PAGE_GOTO(IE_DOWN, about);
  PAGE_GOTO(IE_UP, about);

  default:
    break;
  }
}

PAGE_RENDER(custom)
{

}

PAGE_INPUT_EVENT(custom)
{
  switch (ev) {
  PAGE_GOTO(IE_UP, sound);
  PAGE_GOTO(IE_DOWN, about);
  MENU_QUIT(IE_BACK);

  case IE_ENTER:
    menu_instance._cur_page = &page_custom_select;
    menu_instance._cur_custom_anim_word_idx = 0;
    break;

  default:
    break;
  }
}

PAGE_RENDER(custom_select)
{

}

PAGE_INPUT_EVENT(custom_select)
{
  switch (ev) {
  PAGE_GOTO(IE_ENTER, custom_show);
  PAGE_GOTO(IE_BACK, custom);

  case IE_UP:
    if (menu_instance._cur_custom_anim_word_idx == 0) {
      menu_instance._cur_custom_anim_word_idx = NUM_CUSTOM_ANIM_WORDS - 1;
    } else {
      menu_instance._cur_custom_anim_word_idx--;
    }
    break;

  case IE_DOWN:
    if (menu_instance._cur_custom_anim_word_idx == (NUM_CUSTOM_ANIM_WORDS - 1)) {
      menu_instance._cur_custom_anim_word_idx = 0;
    } else {
      menu_instance._cur_custom_anim_word_idx++;
    }
    break;

  default:
    break;
  }
}

PAGE_RENDER(custom_show)
{

}

PAGE_INPUT_EVENT(custom_show)
{
  switch (ev) {
  PAGE_GOTO(IE_BACK, custom_select);
  PAGE_GOTO(IE_UP, custom_anim);
  PAGE_GOTO(IE_DOWN, custom_set);
  PAGE_GOTO(IE_ENTER, custom_show_show);

  default:
    break;
  }
}

PAGE_RENDER(custom_show_show)
{

}

PAGE_INPUT_EVENT(custom_show_show)
{
  switch (ev) {
  PAGE_GOTO(IE_BACK, custom_show);
  PAGE_GOTO(IE_ENTER, custom_show);
  PAGE_GOTO(IE_DOWN, custom_show);
  PAGE_GOTO(IE_UP, custom_show);

  default:
    break;
  }
}

PAGE_RENDER(custom_set)
{

}

PAGE_INPUT_EVENT(custom_set)
{
  switch (ev) {

  case IE_ENTER:
    // TODO save slave_seq / custom anim
    break;

  PAGE_GOTO(IE_BACK, custom_select);
  PAGE_GOTO(IE_DOWN, custom_enable);
  PAGE_GOTO(IE_UP, custom_show);

  default:
    break;
  }
}

PAGE_RENDER(custom_enable)
{

}

PAGE_INPUT_EVENT(custom_enable)
{
  switch (ev) {
  PAGE_GOTO(IE_BACK, custom_select);
  PAGE_GOTO(IE_DOWN, custom_chars);
  PAGE_GOTO(IE_UP, custom_set);

  case IE_ENTER:
    // TODO: save settings
    break;

  default:
    break;
  }
}

PAGE_RENDER(custom_chars)
{

}

PAGE_INPUT_EVENT(custom_chars)
{
  switch (ev) {
  PAGE_GOTO(IE_BACK, custom_select);
  PAGE_GOTO(IE_ENTER, custom_chars_edit);
  PAGE_GOTO(IE_DOWN, custom_anim);
  PAGE_GOTO(IE_UP, custom_enable);

  default:
    break;
  }
}

PAGE_RENDER(custom_chars_edit)
{

}

PAGE_INPUT_EVENT(custom_chars_edit)
{
  switch (ev) {
  PAGE_GOTO(IE_BACK, custom_chars);

  case IE_ENTER:
    break;
  case IE_DOWN:
    break;
  case IE_UP:
    break;
    // All 3 todo

  default:
    break;
  }
}

PAGE_RENDER(custom_anim)
{

}

PAGE_INPUT_EVENT(custom_anim)
{
  switch (ev) {
  PAGE_GOTO(IE_BACK, custom_select);
  PAGE_GOTO(IE_ENTER, custom_anim_edit);
  PAGE_GOTO(IE_DOWN, custom_show);
  PAGE_GOTO(IE_UP, custom_chars);

  default:
    break;
  }
}

PAGE_RENDER(custom_anim_edit)
{

}

PAGE_INPUT_EVENT(custom_anim_edit)
{
  switch (ev) {
  PAGE_GOTO(IE_BACK, custom_anim);

  case IE_ENTER:
    break;
  case IE_DOWN:
    break;
  case IE_UP:
    break;
    // All 3 todo

  default:
    break;
  }
}

PAGE_RENDER(sound)
{

}

PAGE_INPUT_EVENT(sound)
{
  switch (ev) {
  PAGE_GOTO(IE_UP, about);
  PAGE_GOTO(IE_DOWN, custom);
  PAGE_GOTO(IE_ENTER, sound_menu);
  MENU_QUIT(IE_BACK);

  default:
    break;
  }
}

PAGE_RENDER(sound_menu)
{

}

PAGE_INPUT_EVENT(sound_menu)
{
  switch (ev) {
  PAGE_GOTO(IE_UP, sound_melody);
  PAGE_GOTO(IE_DOWN, sound_melody);
  PAGE_GOTO(IE_BACK, sound);

  case IE_ENTER:
    settings_set_sound_menu (!settings_get_sound_menu ());
    break;

  default:
    break;
  }
}

PAGE_RENDER(sound_melody)
{

}

PAGE_INPUT_EVENT(sound_melody)
{
  switch (ev) {
  PAGE_GOTO(IE_UP, sound_menu);
  PAGE_GOTO(IE_DOWN, sound_menu);
  PAGE_GOTO(IE_BACK, sound);

  case IE_ENTER:
    settings_set_sound_melody (!settings_get_sound_melody ());
    break;

  default:
    break;
  }
}
