#include <stdint.h>

#include "frame.h"

const uint8_t *font_get_char (uint8_t index);
void font_char_to_frame (uint8_t index, struct frame *frame);
