#include "j3p-recv.h"

void j3p_recv_on_falling (struct j3p_recv_fsm *fsm)
{
  fsm = fsm;
}

void j3p_recv_reset (struct j3p_recv_fsm *fsm)
{
  fsm = fsm;
}

void j3p_recv_init (struct j3p_recv_fsm *fsm,
                    j3p_recv_read_line_op read_line)
{
  fsm->read_line = read_line;
}
