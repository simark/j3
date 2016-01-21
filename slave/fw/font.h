#include <stdint.h>

#include "frame.h"

const uint8_t *font_get_char (uint8_t index);
void font_char_to_frame (uint8_t index, struct frame *frame);
uint8_t font_count (void);
uint8_t font_is_valid_char (uint8_t c);
