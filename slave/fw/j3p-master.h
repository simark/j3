#ifndef J3P_MASTER_H
#define J3P_MASTER_H

#include <avr/io.h>
#include "j3p-send.h"
#include "j3p-recv.h"

typedef void (*j3p_master_query_complete_op)(void);

struct j3p_master_ctx {
  enum {
    J3P_MASTER_STATE_IDLE,
    J3P_MASTER_STATE_SENDING,
    J3P_MASTER_STATE_RECEIVING,
  } state;

  union {
    struct j3p_send_fsm send;
    struct j3p_recv_fsm recv;
  };

  j3p_read_line_op read_line;
  j3p_set_line_op line_up, line_down;

  uint8_t bytes_out, bytes_in;
  uint8_t *buf;

  j3p_master_query_complete_op query_complete;
};

void j3p_master_init (struct j3p_master_ctx *ctx,
                      j3p_set_line_op line_up,
                      j3p_set_line_op line_down,
                      j3p_read_line_op read_line,
                      uint8_t bytes_out, uint8_t bytes_in,
                      uint8_t *send_recv_buf,
                      j3p_master_query_complete_op recv_complete);
void j3p_master_query (struct j3p_master_ctx *ctx);
void j3p_master_on_rising (struct j3p_master_ctx *ctx);
void j3p_master_on_falling (struct j3p_master_ctx *ctx);

#endif /* J3P_MASTER_H */
