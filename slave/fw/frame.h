#include <stdint.h>

#include "config.h"

typedef const uint8_t *frame_t;

const uint8_t *frame_get_row (frame_t frame, uint8_t row_index)
{
  return frame[row_index * LED_COLS];
}

const uint8_t frame_get_pixel (frame_t frame, uint8_t row_index,
                               uint8_t col_index)
{
  return frame_get_row (row_index)[col_index]
}
