#ifndef J3P_SEND_H
#define J3P_SEND_H

#include <avr/io.h>

typedef void (*j3p_send_set_line_op) (void);

#define J3P_SEND_BREAK_NUM_BITS 20
#define J3P_SEND_MARK_AFTER_BREAK_NUM_BITS 4

struct j3p_send_fsm {
  enum {
    J3P_SEND_STATE_BREAK,
    J3P_SEND_STATE_MARK_AFTER_BREAK,
    J3P_SEND_STATE_BYTE_START_BIT,
    J3P_SEND_STATE_BYTE,
    J3P_SEND_STATE_BYTE_STOP_BIT,
    J3P_SEND_STATE_DONE,
  } state;

  j3p_send_set_line_op line_up, line_down;
  uint8_t *buf;
  uint8_t bytes_left;

  uint8_t bits_left;
  uint8_t cur_byte;
};

void j3p_send_init (struct j3p_send_fsm *fsm,
                    j3p_send_set_line_op line_up,
                    j3p_send_set_line_op line_down,
                    uint8_t bytes_out,
                    uint8_t *send_buf);
void j3p_send_on_rising (struct j3p_send_fsm *fsm);

#endif /* J3P_SEND_H */
