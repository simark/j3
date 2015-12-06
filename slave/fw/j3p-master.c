#include "j3p-master.h"

/* state transitions */

static void j3p_master_state_idle (struct j3p_master_ctx *ctx)
{
  ctx->state = J3P_MASTER_STATE_IDLE;
}

static void j3p_master_state_break (struct j3p_master_ctx *ctx)
{
  ctx->state = J3P_MASTER_STATE_BREAK;
  ctx->break_.bits_left = J3P_BREAK_NUM_BITS;
}

static void j3p_master_state_mark_after_break (struct j3p_master_ctx *ctx)
{
  ctx->state = J3P_MASTER_STATE_MARK_AFTER_BREAK;
  ctx->mark_after_break.bits_left = J3P_MARK_AFTER_BREAK_NUM_BITS;
}

static void j3p_master_state_sending (struct j3p_master_ctx *ctx)
{
  ctx->state = J3P_MASTER_STATE_SENDING;
  j3p_send_init (&ctx->send,
                 ctx->line_up, ctx->line_down,
                 ctx->bytes_out, ctx->buf);
}

static void j3p_master_state_receiving (struct j3p_master_ctx *ctx)
{
  ctx->state = J3P_MASTER_STATE_RECEIVING;
  j3p_recv_init (&ctx->recv, ctx->read_line, ctx->bytes_in, ctx->buf);
}

/* clock events */

static void j3p_master_on_rising_idle (struct j3p_master_ctx *ctx)
{
  /* We're idle, just keep the pin in high-z. */
  ctx->line_up();
}

static void j3p_master_on_rising_break (struct j3p_master_ctx *ctx)
{
  ctx->line_down ();

  ctx->break_.bits_left--;

  if (ctx->break_.bits_left == 0) {
    j3p_master_state_mark_after_break (ctx);
  }
}

static void j3p_master_on_rising_mark_after_break (struct j3p_master_ctx *ctx)
{
  ctx->line_up ();

  ctx->mark_after_break.bits_left--;

  if (ctx->mark_after_break.bits_left == 0) {
    j3p_master_state_sending (ctx);
  }
}

static void j3p_master_on_rising_sending (struct j3p_master_ctx *ctx)
{
  struct j3p_send_fsm *fsm = &ctx->send;

  j3p_send_on_rising (fsm);

  if (j3p_send_is_done (fsm)) {
    j3p_master_state_receiving (ctx);
  }
}


void j3p_master_on_rising (struct j3p_master_ctx *ctx)
{
  switch (ctx->state) {
  case J3P_MASTER_STATE_IDLE:
    j3p_master_on_rising_idle (ctx);
    break;

  case J3P_MASTER_STATE_BREAK:
    j3p_master_on_rising_break (ctx);
    break;

  case J3P_MASTER_STATE_MARK_AFTER_BREAK:
    j3p_master_on_rising_mark_after_break (ctx);
    break;

  case J3P_MASTER_STATE_SENDING:
    j3p_master_on_rising_sending (ctx);
    break;

  case J3P_MASTER_STATE_RECEIVING:
    break;
  }
}

static void j3p_master_on_falling_receiving (struct j3p_master_ctx *ctx)
{
  struct j3p_recv_fsm *fsm = &ctx->recv;

  j3p_recv_on_falling (fsm);

  if (j3p_recv_is_done (fsm)) {
    ctx->query_complete (ctx->buf);
    j3p_master_state_idle (ctx);
  }
}

void j3p_master_on_falling (struct j3p_master_ctx *ctx)
{
  switch (ctx->state) {
   case J3P_MASTER_STATE_IDLE:
     break;

   case J3P_MASTER_STATE_BREAK:
     break;

   case J3P_MASTER_STATE_MARK_AFTER_BREAK:
     break;

   case J3P_MASTER_STATE_SENDING:
     break;

   case J3P_MASTER_STATE_RECEIVING:
     j3p_master_on_falling_receiving (ctx);
     break;
   }
}

/* initiate a query */

void j3p_master_query (struct j3p_master_ctx *ctx)
{
  j3p_master_state_break (ctx);
}

/* one-time initialization */

void j3p_master_init (struct j3p_master_ctx *ctx,
                      j3p_set_line_op line_up,
                      j3p_set_line_op line_down,
                      j3p_read_line_op read_line,
                      uint8_t bytes_out, uint8_t bytes_in,
                      uint8_t *send_recv_buf,
                      j3p_master_query_complete_op query_complete)
{
  ctx->line_up = line_up;
  ctx->line_down = line_down;
  ctx->read_line = read_line;
  ctx->bytes_out = bytes_out;
  ctx->bytes_in = bytes_in;
  ctx->buf = send_recv_buf;
  ctx->query_complete = query_complete;

  j3p_master_state_idle (ctx);
}
