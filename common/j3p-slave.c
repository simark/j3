#include "j3p-slave.h"
#include "j3p-common.h"

/* state transitions */

static void j3p_slave_state_idle (volatile struct j3p_slave_ctx *ctx)
{
  ctx->state = J3P_SLAVE_STATE_IDLE;
  ctx->break_len = 0;
}

static void j3p_slave_state_receiving (volatile struct j3p_slave_ctx *ctx)
{
  ctx->state = J3P_SLAVE_STATE_RECEIVING;
  j3p_recv_init (&ctx->recv, ctx->read_line, ctx->bytes_in, ctx->buf);
  ctx->break_len = 0;
}

static void j3p_slave_state_sending (volatile struct j3p_slave_ctx *ctx)
{
  ctx->state = J3P_SLAVE_STATE_SENDING;
  j3p_send_init (&ctx->send, ctx->line_up, ctx->line_down, ctx->bytes_out, ctx->buf);
}

/* clock events */

static void j3p_slave_on_rising_sending (volatile struct j3p_slave_ctx *ctx)
{
  volatile struct j3p_send_fsm *fsm = &ctx->send;
  j3p_send_on_rising (fsm);

  if (j3p_send_is_done (fsm)) {
    j3p_slave_state_idle (ctx);
  }

}

void j3p_slave_on_rising(volatile struct j3p_slave_ctx *ctx)
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

static uint8_t j3p_slave_on_falling_detect_break (volatile struct j3p_slave_ctx *ctx)
{
  uint8_t line_value = ctx->read_line ();

  if (line_value) {
    if (ctx->break_len >= J3P_BREAK_NUM_BITS) {
      return 1;
    }

    ctx->break_len = 0;
  } else {
    ctx->break_len++;
  }

  return 0;
}

static void j3p_slave_on_falling_idle (volatile struct j3p_slave_ctx *ctx)
{
  if (j3p_slave_on_falling_detect_break (ctx)) {
    j3p_slave_state_receiving (ctx);
  }
}

static void j3p_slave_on_falling_receiving (volatile struct j3p_slave_ctx *ctx)
{
  volatile struct j3p_recv_fsm *fsm = &ctx->recv;

  if (j3p_slave_on_falling_detect_break (ctx)) {
    j3p_slave_state_receiving (ctx);
    return;
  }

  j3p_recv_on_falling (fsm);

  if (j3p_recv_is_done (fsm)) {
    ctx->query ();
    j3p_slave_state_sending (ctx);
  } else if (j3p_recv_is_err (fsm)) {
    j3p_slave_state_idle (ctx);
  }
}

void j3p_slave_on_falling (volatile struct j3p_slave_ctx *ctx)
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

/* one-time initialization */

void j3p_slave_init (volatile struct j3p_slave_ctx *ctx,
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
