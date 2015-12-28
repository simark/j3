#include <stdint.h>
#include "common-config.h"

#define MS_TO_TICKS(ms) ((MASTER_CLK_FREQ * (ms)) / 1000)

typedef uint16_t tick_t;

void tick (void);
tick_t get_tick (void);
uint8_t tick_expired (tick_t start, tick_t end);
