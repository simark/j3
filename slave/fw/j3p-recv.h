#ifndef J3P_RECV_H
#define J3P_RECV_H

#include <avr/io.h>

typedef uint8_t (*j3p_recv_read_line_op)(void);

struct j3p_recv_fsm {
	j3p_recv_read_line_op read_line;
};

void j3p_recv_init (struct j3p_recv_fsm *fsm,
                    j3p_recv_read_line_op read_line);
void j3p_recv_reset (struct j3p_recv_fsm *fsm);
void j3p_recv_on_falling (struct j3p_recv_fsm *fsm);

#endif /* J3P_RECV_H */
