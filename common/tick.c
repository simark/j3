#include "tick.h"

static volatile tick_t tick_cnt = 0;

void tick (void)
{
  tick_cnt++;
}

tick_t get_tick (void)
{
  return tick_cnt;
}
uint8_t tick_expired (tick_t start, tick_t end)
{
  if (start < end) {
      return tick_cnt < start || tick_cnt > end;
  } else {
      return tick_cnt < start && tick_cnt > end;
  }
}
