#include <stdint.h>

#include "frame.h"

struct animator {
  void (* reset_cb) (void* data);
  frame_t (* get_next_frame_cb) (void *data);
  uint8_t char_index;
  void *data;
};

void animator_reset (struct animator *animator)
{
  animator->reset (animator->data);
}

frame_t animator_get_next_frame (struct animator *animator)
{
  return animator->get_next_frame_cb (animator->data);
}

void animator_set_char (struct animator *animator, uint8_t char_index)
{
  animator->char_index = char_index;
}
