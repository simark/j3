#ifndef J3P_MASTER_H
#define J3P_MASTER_H

#include "j3p-send.h"
#include "j3p-recv.h"

struct j3p_master_ctx {
	enum {
		J3P_MASTER_STATE_IDLE,
		J3P_MASTER_STATE_SENDING,
		J3P_MASTER_STATE_RECEIVING,
	} state;

	struct j3p_send_fsm send;
	struct j3p_recv_fsm recv;
};

void j3p_master_init (struct j3p_master_ctx *ctx,
                      j3p_send_set_line_op line_up,
                      j3p_send_set_line_op line_down,
                      j3p_recv_read_line_op read_line);
void j3p_master_query (struct j3p_master_ctx *ctx);
void j3p_master_on_rising (struct j3p_master_ctx *ctx);
void j3p_master_on_falling (struct j3p_master_ctx *ctx);

#endif /* J3P_MASTER_H */
