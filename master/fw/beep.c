#include "beep.h"

void init_beep (volatile struct beep_state *state, void (*on) (uint8_t), void (*off) (void))
{
  state->_playing = 0;
  state->_off = off;
  state->_on = on;
}

void beep_loop (volatile struct beep_state *state)
{

  if (state->_playing &&
      tick_expired (state->_start, state->_end)) {
    if (state->_song->duration != 0) {
      state->_start = get_tick ();
      state->_end = get_tick () + state->_song->duration;
      state->_playing = 1;

      state->_on (state->_song->freq_value);

      state->_song++;
    } else {
      state->_playing = 0;
      state->_off ();
    }
  }
}

void beep (volatile struct beep_state *state, struct beep_note *song)
{
  state->_start = get_tick ();
  state->_end = get_tick () + song->duration;
  state->_playing = 1;

  state->_on (song->freq_value);

  state->_song = song + 1;
}
