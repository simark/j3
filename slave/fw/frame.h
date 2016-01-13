#include <stdint.h>

#include "config.h"

#define FRAME_RED_MASK              _BV(2)
#define FRAME_GREEN_MASK            _BV(1)
#define FRAME_BLUE_MASK             _BV(0)
#define FRAME_PIXEL_GET_RED(_px)    ((_px) & FRAME_RED_MASK) ? 1 : 0)
#define FRAME_PIXEL_GET_GREEN(_px)  ((_px) & FRAME_GREEN_MASK) ? 1 : 0)
#define FRAME_PIXEL_GET_BLUE(_px)   ((_px) & FRAME_BLUE_MASK) ? 1 : 0)

struct frame_row {
  uint8_t cols[DISPLAY_COLS];
};

struct frame {
  struct frame_row rows[DISPLAY_ROWS];
};
