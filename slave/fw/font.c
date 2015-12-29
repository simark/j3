#include <stdint.h>

#include "config.h"

static uint8_t font[] = {
  0x0, 0x0, 0x0, 0x0, 0x0,
  0xe, 0x11, 0x1f, 0x11, 0x11,
  0x1e, 0x11, 0x1e, 0x11, 0x1e,
  0xf, 0x10, 0x10, 0x10, 0xf,
  0x1e, 0x11, 0x11, 0x11, 0x1e,
  0xf, 0x10, 0x1e, 0x10, 0x1f,
  0xf, 0x10, 0x1e, 0x10, 0x10,
  0xe, 0x10, 0x13, 0x11, 0xf,
  0x11, 0x11, 0x1f, 0x11, 0x11,
  0xe, 0x4, 0x4, 0x4, 0xe,
  0x7, 0x1, 0x1, 0x11, 0xe,
  0x12, 0x14, 0x1e, 0x11, 0x11,
  0x10, 0x10, 0x10, 0x10, 0xf,
  0x11, 0x1b, 0x15, 0x11, 0x11,
  0x11, 0x19, 0x15, 0x13, 0x11,
  0xe, 0x11, 0x11, 0x11, 0xe,
  0x1e, 0x11, 0x1e, 0x10, 0x10,
  0xc, 0x12, 0x12, 0x12, 0xf,
  0x1e, 0x11, 0x1e, 0x11, 0x11,
  0xf, 0x10, 0xe, 0x1, 0x1e,
  0x1f, 0x4, 0x4, 0x4, 0x4,
  0x11, 0x11, 0x11, 0x11, 0xe,
  0x11, 0x11, 0xa, 0xa, 0x4,
  0x11, 0x11, 0x15, 0x1b, 0x11,
  0x11, 0xa, 0x4, 0xa, 0x11,
  0x11, 0x11, 0xe, 0x4, 0x4,
  0x1f, 0x2, 0x4, 0x8, 0x1f,
  0xe, 0x1, 0xf, 0x11, 0xf,
  0x10, 0x1e, 0x11, 0x11, 0x1e,
  0xe, 0x11, 0x10, 0x11, 0xe,
  0x1, 0xf, 0x11, 0x11, 0xf,
  0xe, 0x11, 0x1f, 0x10, 0xf,
  0xe, 0x11, 0x1c, 0x10, 0x10,
  0xe, 0x11, 0xf, 0x1, 0x6,
  0x10, 0x1e, 0x11, 0x11, 0x11,
  0x4, 0x0, 0x4, 0x4, 0x4,
  0x2, 0x0, 0x2, 0x2, 0xc,
  0x8, 0x8, 0xa, 0xc, 0xa,
  0x4, 0x4, 0x4, 0x4, 0x2,
  0xa, 0x15, 0x11, 0x11, 0x11,
  0x1e, 0x11, 0x11, 0x11, 0x11,
  0xe, 0x11, 0x11, 0x11, 0xe,
  0x1e, 0x11, 0x11, 0x1e, 0x10,
  0xf, 0x11, 0x11, 0xf, 0x1,
  0x16, 0x19, 0x10, 0x10, 0x10,
  0xe, 0x10, 0xe, 0x1, 0xe,
  0x10, 0x1c, 0x10, 0x11, 0xe,
  0x11, 0x11, 0x11, 0x13, 0xd,
  0x11, 0x11, 0x11, 0xa, 0x4,
  0x11, 0x11, 0x11, 0x15, 0xa,
  0x11, 0x11, 0xe, 0x11, 0x11,
  0x11, 0x11, 0xf, 0x1, 0xe,
  0xf, 0x1, 0x2, 0x4, 0xf,
  0xe, 0x13, 0x15, 0x19, 0xe,
  0x4, 0xc, 0x4, 0x4, 0xe,
  0x1e, 0x1, 0xe, 0x10, 0x1f,
  0x1e, 0x1, 0x6, 0x1, 0x1e,
  0x11, 0x11, 0xf, 0x1, 0x1,
  0x1f, 0x10, 0x1e, 0x1, 0x1e,
  0xe, 0x10, 0x1e, 0x11, 0xe,
  0x1f, 0x1, 0x2, 0x4, 0x8,
  0xe, 0x11, 0xe, 0x11, 0xe,
  0xe, 0x11, 0xf, 0x1, 0xe,
  0xf, 0x14, 0xe, 0x5, 0x1e,
  0x4, 0xe, 0x8, 0xe, 0x4,
  0x6, 0x9, 0x1c, 0x8, 0x1f,
  0x11, 0xe, 0x4, 0xe, 0x4,
  0x11, 0xe, 0xa, 0xe, 0x11,
  0x0, 0x4, 0xe, 0x4, 0x0,
  0x0, 0x0, 0xe, 0x0, 0x0,
  0x4, 0x15, 0xe, 0x15, 0x4,
  0x1, 0x2, 0x4, 0x8, 0x10,
  0x0, 0xe, 0x0, 0xe, 0x0,
  0x19, 0x1a, 0x4, 0xb, 0x13,
  0xa, 0xa, 0x0, 0x0, 0x0,
  0x4, 0x4, 0x0, 0x0, 0x0,
  0xa, 0x1f, 0xa, 0x1f, 0xa,
  0xe, 0x11, 0x16, 0x10, 0xf,
  0x8, 0x14, 0x8, 0x15, 0xa,
  0x0, 0x0, 0x0, 0x0, 0x1f,
  0x2, 0x4, 0x4, 0x4, 0x2,
  0x8, 0x4, 0x4, 0x4, 0x8,
  0x0, 0x0, 0xc, 0xc, 0x4,
  0x0, 0x0, 0x0, 0x0, 0x4,
  0x0, 0x4, 0x0, 0x4, 0x4,
  0x0, 0x4, 0x0, 0x4, 0x0,
  0xe, 0x11, 0x6, 0x0, 0x4,
  0x4, 0x4, 0x4, 0x0, 0x4,
  0x10, 0x8, 0x4, 0x2, 0x1,
  0x4, 0x4, 0x4, 0x4, 0x4,
  0x6, 0x4, 0x8, 0x4, 0x6,
  0xc, 0x4, 0x2, 0x4, 0xc,
  0x2, 0x4, 0x8, 0x4, 0x2,
  0x8, 0x4, 0x2, 0x4, 0x8,
  0xe, 0x8, 0x8, 0x8, 0xe,
  0xe, 0x2, 0x2, 0x2, 0xe,
  0x4, 0xa, 0x0, 0x0, 0x0,
  0x5, 0xa, 0x0, 0x0, 0x0,
  0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
  0xa, 0x1f, 0x1f, 0xe, 0x4,
  0x4, 0xe, 0x1f, 0xe, 0x4,
  0xe, 0x15, 0x1f, 0x15, 0x4,
  0x4, 0xe, 0x1f, 0x1f, 0x4,
  0x6, 0x4, 0x4, 0xc, 0xc,
  0xf, 0x9, 0x9, 0x1b, 0x1b,
  0xe, 0xa, 0x4, 0xe, 0x4,
  0x7, 0x3, 0x1d, 0x14, 0x1c,
  0x4, 0xe, 0x4, 0xe, 0x4,
  0x0, 0xa, 0x1f, 0xa, 0x0,
  0x4, 0xe, 0x1f, 0x4, 0x4,
  0x4, 0x4, 0x1f, 0xe, 0x4,
  0x4, 0xc, 0x1f, 0xc, 0x4,
  0x4, 0x6, 0x1f, 0x6, 0x4,
  0x0, 0x4, 0xe, 0x1f, 0x0,
  0x0, 0x1f, 0xe, 0x4, 0x0,
  0x2, 0x6, 0xe, 0x6, 0x2,
  0x8, 0xc, 0xe, 0xc, 0x8,
  0x4, 0xa, 0x11, 0x11, 0x1f,
  0x1f, 0x11, 0x11, 0xa, 0x4,
  0x7, 0x9, 0x11, 0x9, 0x7,
  0x1c, 0x12, 0x11, 0x12, 0x1c,
  0x4, 0xe, 0x15, 0x4, 0x4,
  0x4, 0x4, 0x15, 0xe, 0x4,
  0x4, 0x8, 0x1f, 0x8, 0x4,
  0x4, 0x2, 0x1f, 0x2, 0x4,
  0x15, 0xa, 0x15, 0xa, 0x15,
  0xa, 0x15, 0xa, 0x15, 0xa,
  0x1f, 0x11, 0x11, 0x11, 0x1f,
  0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
  0x0, 0xe, 0xa, 0xe, 0x0,
  0x0, 0xe, 0xe, 0xe, 0x0,
  0x18, 0x18, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x18, 0x18,
  0x3, 0x3, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x3, 0x3,
  0x1f, 0x0, 0x0, 0x0, 0x0,
  0x10, 0x10, 0x10, 0x10, 0x10,
  0x1, 0x1, 0x1, 0x1, 0x1,
  0x1f, 0x1f, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x1f, 0x1f,
  0x18, 0x18, 0x18, 0x18, 0x18,
  0x3, 0x3, 0x3, 0x3, 0x3,
  0x11, 0xa, 0x0, 0xa, 0x11,
  0xe, 0x1f, 0x1f, 0xa, 0xe,
  0xe, 0x1b, 0x11, 0x1b, 0xe,
  0x1b, 0x9, 0x0, 0x1b, 0x9,
  0x4, 0x1f, 0xe, 0xe, 0x15,
  0x4, 0xa, 0xe, 0x1b, 0x15,
  0x11, 0x1f, 0x15, 0x1f, 0xe,
  0xe, 0x1f, 0x1f, 0x4, 0x6,
  0x1f, 0xa, 0x4, 0xa, 0x1f,
  0x12, 0x1b, 0xa, 0x0, 0xf,
  0xe, 0x1d, 0x17, 0x13, 0xe,
  0xa, 0x15, 0x1f, 0xa, 0x11,
  0x15, 0xe, 0x4, 0x6, 0x19,
  0x4, 0xa, 0x15, 0xa, 0x4,
  0x0, 0x1b, 0x15, 0x1b, 0x0,
  0x11, 0x0, 0x1f, 0x11, 0x1f,
  0x1c, 0x17, 0x1c, 0x1f, 0x1c,
  0xe, 0x11, 0x11, 0xa, 0xe,
  0xe, 0x1f, 0x15, 0x1f, 0x15,
  0xa, 0x0, 0x11, 0xe, 0x0,
  0xa, 0x0, 0xe, 0x11, 0x0,
  0x1c, 0x12, 0x11, 0x11, 0x1f,
  0xa, 0x15, 0x11, 0xa, 0x4,
  0x1f, 0x1b, 0x15, 0x1b, 0x1f,
  0x4, 0xe, 0x1f, 0xa, 0x11,
  0x1f, 0x15, 0x15, 0x15, 0x1f,
  0x1f, 0x11, 0x1f, 0x11, 0x1f,
  0x1f, 0x1b, 0x11, 0x1b, 0x1f,
  0x11, 0x0, 0x0, 0x0, 0x11,
  0x1b, 0x11, 0x0, 0x11, 0x1b,
  0x1c, 0x10, 0x11, 0x1, 0x7,
  0x7, 0x1, 0x11, 0x10, 0x1c,
  0x1c, 0x10, 0x15, 0x1, 0x7,
  0x7, 0x1, 0x15, 0x10, 0x1c,
  0x1f, 0x1f, 0x1b, 0x1f, 0x1f,
  0x9, 0x12, 0x4, 0x9, 0x12,
  0x12, 0x9, 0x4, 0x12, 0x9,
  0x15, 0x15, 0x15, 0x15, 0x15,
  0x1f, 0x0, 0x1f, 0x0, 0x1f,
  0x4, 0x4, 0x1b, 0x4, 0x4,
  0xa, 0x1b, 0x0, 0x1b, 0xa,
  0x0, 0x0, 0x0, 0x1f, 0x0,
  0x0, 0x0, 0x1f, 0x0, 0x0,
  0x0, 0x1f, 0x0, 0x0, 0x0,
  0x1f, 0x0, 0x0, 0x0, 0x0,
  0x10, 0x10, 0x10, 0x10, 0x10,
  0x8, 0x8, 0x8, 0x8, 0x8,
  0x2, 0x2, 0x2, 0x2, 0x2,
  0x1, 0x1, 0x1, 0x1, 0x1,
};

const uint8_t *font_get_char (uint8_t index)
{
  return &font[LED_ROWS * index];
}
