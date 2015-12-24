#ifndef J3P_SLAVE_H
#define J3P_SLAVE_H

#include <avr/io.h>
#include "j3p-send.h"
#include "j3p-recv.h"

typedef void (*j3p_slave_query_op) (uint8_t *buf);

struct j3p_slave_ctx {
  enum {
    J3P_SLAVE_STATE_IDLE,
    J3P_SLAVE_STATE_RECEIVING,
    J3P_SLAVE_STATE_SENDING,
  } state;

  union {
    struct j3p_recv_fsm recv;
    struct j3p_send_fsm send;
  };

  j3p_read_line_op read_line;
  j3p_set_line_op line_up, line_down;

  uint8_t bytes_in, bytes_out;
  uint8_t *buf;

  j3p_slave_query_op query;

  uint8_t break_len;
};

void j3p_slave_init (struct j3p_slave_ctx *ctx,
                     j3p_set_line_op line_up,
                     j3p_set_line_op line_down,
                     j3p_read_line_op read_line,
                     uint8_t bytes_in, uint8_t bytes_out,
                     uint8_t *send_recv_buf,
                     j3p_slave_query_op recv_complete);
void j3p_slave_on_rising (struct j3p_slave_ctx *ctx);
void j3p_slave_on_falling (struct j3p_slave_ctx *ctx);

#endif /* J3P_SLAVE_H */
