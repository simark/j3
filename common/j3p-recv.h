#ifndef J3P_RECV_H
#define J3P_RECV_H

#include <avr/io.h>

#include "j3p-common.h"

struct j3p_recv_fsm {
  enum {
    J3P_RECV_STATE_START_BIT,
    J3P_RECV_STATE_BYTE,
    J3P_RECV_STATE_STOP_BIT,
    J3P_RECV_STATE_DONE,
    J3P_RECV_STATE_ERR,
  } state;

  j3p_read_line_op read_line;

  volatile uint8_t *buf;
  uint8_t bytes_left;

  uint8_t bits_left;
  uint8_t cur_byte;
};

void j3p_recv_init (volatile struct j3p_recv_fsm *fsm,
                    j3p_read_line_op read_line,
                    uint8_t bytes_in,
                    volatile uint8_t *recv_buf);
void j3p_recv_on_falling (volatile struct j3p_recv_fsm *fsm);

static inline uint8_t j3p_recv_is_done (volatile struct j3p_recv_fsm *fsm)
{
  return fsm->state == J3P_RECV_STATE_DONE;
}

static inline uint8_t j3p_recv_is_err (volatile struct j3p_recv_fsm *fsm)
{
  return fsm->state == J3P_RECV_STATE_ERR;
}

#endif /* J3P_RECV_H */
