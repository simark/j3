#include "j3p-slave.h"
#include "j3p-common.h"

/* J3P slave state transitions */

static void j3p_slave_state_idle (struct j3p_slave_ctx *ctx)
{
  ctx->state = J3P_SLAVE_STATE_IDLE;
}

static void j3p_slave_state_receiving (struct j3p_slave_ctx *ctx)
{
  ctx->state = J3P_SLAVE_STATE_RECEIVING;
  j3p_recv_init (&ctx->recv, ctx->read_line, ctx->bytes_in, ctx->buf);
}

static void j3p_slave_state_sending (struct j3p_slave_ctx *ctx)
{
  ctx->state = J3P_SLAVE_STATE_SENDING;
  j3p_send_init (&ctx->send, ctx->line_up, ctx->line_down, ctx->bytes_out, ctx->buf);
}

/* J3P slave events */

static void j3p_slave_on_rising_sending (struct j3p_slave_ctx *ctx)
{
  struct j3p_send_fsm *fsm = &ctx->send;
  j3p_send_on_rising (fsm);

  if (j3p_send_is_done (fsm)) {
    j3p_slave_state_idle (ctx);
  }

}

void j3p_slave_on_rising(struct j3p_slave_ctx *ctx)
{
  switch (ctx->state) {
  case J3P_SLAVE_STATE_IDLE:
    break;

  case J3P_SLAVE_STATE_RECEIVING:
    break;

  case J3P_SLAVE_STATE_SENDING:
    j3p_slave_on_rising_sending (ctx);
    break;
  }
}

static void j3p_slave_on_falling_idle (struct j3p_slave_ctx *ctx)
{
  uint8_t line_value = ctx->read_line ();

  if (line_value) {
    if (ctx->break_len >= J3P_BREAK_NUM_BITS) {

      j3p_slave_state_receiving (ctx);
    } else {
      ctx->break_len = 0;
    }
  } else {
    ctx->break_len++;
  }
}

static void j3p_slave_on_falling_receiving (struct j3p_slave_ctx *ctx)
{
  struct j3p_recv_fsm *fsm = &ctx->recv;

  j3p_recv_on_falling (fsm);

  if (j3p_recv_is_done (fsm)) {
    ctx->query (ctx->buf);
    j3p_slave_state_sending (ctx);
  } else if (j3p_recv_is_err (fsm)) {
    j3p_slave_state_idle (ctx);
  }
}

void j3p_slave_on_falling (struct j3p_slave_ctx *ctx)
{
 switch (ctx->state) {
 case J3P_SLAVE_STATE_IDLE:
   j3p_slave_on_falling_idle (ctx);
   break;

 case J3P_SLAVE_STATE_RECEIVING:
   j3p_slave_on_falling_receiving (ctx);
   break;

 case J3P_SLAVE_STATE_SENDING:
   break;
 }
}

/* One time initialization of the slave context. */

void j3p_slave_init (struct j3p_slave_ctx *ctx,
                     j3p_set_line_op line_up,
                     j3p_set_line_op line_down,
                     j3p_read_line_op read_line,
                     uint8_t bytes_in, uint8_t bytes_out,
                     uint8_t *send_recv_buf,
                     j3p_slave_query_op query)
{
  ctx->line_up = line_up;
  ctx->line_down = line_down;
  ctx->read_line = read_line;
  ctx->bytes_in = bytes_in;
  ctx->bytes_out = bytes_out;
  ctx->buf = send_recv_buf;
  ctx->query = query;
  ctx->break_len = 0;

  j3p_slave_state_idle (ctx);
}
