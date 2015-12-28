#ifndef MASTER_FW_BEEP_H_
#define MASTER_FW_BEEP_H_

#include <tick.h>

struct beep_note {
  tick_t duration;
  uint8_t freq_value;
};

struct beep_state {
  tick_t _start, _end;
  struct beep_note *_song;
  uint8_t _playing;

  void (*_on) (uint8_t freq_value);
  void (*_off) (void);
};


#define BEEP_NOTE_END {0,0}

void init_beep (volatile struct beep_state *state, void (*) (uint8_t), void (*) (void));
void beep (volatile struct beep_state *state, struct beep_note *song);
void beep_loop (volatile struct beep_state *state);

#endif /* MASTER_FW_BEEP_H_ */
