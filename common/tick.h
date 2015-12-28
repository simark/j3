#include <stdint.h>

typedef uint16_t tick_t;

void tick (void);
tick_t get_tick (void);
uint8_t tick_expired (tick_t start, tick_t end);
