#include "j3p-master.h"

/* J3P master state transitions */

static void j3p_master_state_idle (struct j3p_master_ctx *ctx) {
  ctx->state = J3P_MASTER_STATE_IDLE;
}

static void j3p_master_state_sending (struct j3p_master_ctx *ctx) {
  ctx->state = J3P_MASTER_STATE_SENDING;
  j3p_send_init (&ctx->send,
                 ctx->line_up, ctx->line_down,
                 ctx->bytes_out, ctx->buf);
}

/*static void j3p_master_state_receiving (struct j3p_master_ctx *ctx) {
  ctx->state = J3P_MASTER_STATE_RECEIVING;
  j3p_recv_reset (&ctx->recv);
}*/

/* J3P master events */

static void j3p_master_on_rising_idle (struct j3p_master_ctx *ctx) {
  /* We're idle, just keep the pin in high-z. */
  ctx->line_up();
}

static void j3p_master_on_rising_sending (struct j3p_master_ctx *ctx) {
  j3p_send_on_rising (&ctx->send);
}

static void j3p_master_on_rising_receiving (struct j3p_master_ctx *ctx) {
  j3p_recv_on_falling (&ctx->recv);
  // Nothing to do?
  //j3p_recv_on_rising (&ctx->recv);
}

void j3p_master_on_rising (struct j3p_master_ctx *ctx) {
  switch (ctx->state) {
  case J3P_MASTER_STATE_IDLE:
    j3p_master_on_rising_idle (ctx);
    break;

  case J3P_MASTER_STATE_SENDING:
    j3p_master_on_rising_sending (ctx);
    break;

  case J3P_MASTER_STATE_RECEIVING:
    j3p_master_on_rising_receiving (ctx);
    break;
  }
}

void j3p_master_on_falling (struct j3p_master_ctx *ctx)
{
  ctx = ctx;
}


void j3p_master_query (struct j3p_master_ctx *ctx) {
  j3p_master_state_sending (ctx);
}

/* One time initialization of the master context. */

void j3p_master_init (struct j3p_master_ctx *ctx,
                      j3p_send_set_line_op line_up,
                      j3p_send_set_line_op line_down,
                      j3p_recv_read_line_op read_line,
                      uint8_t bytes_out, uint8_t bytes_in,
                      uint8_t *send_recv_buf,
                      j3p_master_query_complete_op query_complete) {
  ctx->line_up = line_up;
  ctx->line_down = line_down;
  ctx->read_line = read_line;
  ctx->bytes_out = bytes_out;
  ctx->bytes_in = bytes_in;
  ctx->buf = send_recv_buf;
  ctx->query_complete = query_complete;

  j3p_master_state_idle (ctx);
}
