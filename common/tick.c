#include "tick.h"
#include "util/atomic.h"

static volatile tick_t tick_cnt = 0;

void tick (void)
{
  tick_cnt++;
}

tick_t get_tick (void)
{
  uint16_t val;
  ATOMIC_BLOCK (ATOMIC_FORCEON) {
    val = tick_cnt;
  }
  return val;
}
uint8_t tick_expired (tick_t start, tick_t end)
{
  uint16_t code_de_noob = get_tick();

  if (start < end) {
      return code_de_noob < start || code_de_noob > end;
  } else {
      return code_de_noob < start && code_de_noob > end;
  }
}
