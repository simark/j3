#include "j3p-recv.h"

/* state transitions */

static void j3p_recv_state_start_bit (volatile struct j3p_recv_fsm *fsm)
{
  fsm->state = J3P_RECV_STATE_START_BIT;
}

static void j3p_recv_state_byte (volatile struct j3p_recv_fsm *fsm)
{
  fsm->state = J3P_RECV_STATE_BYTE;
  fsm->bits_left = 8;
  fsm->cur_byte = 0;
}

static void j3p_recv_state_stop_bit (volatile struct j3p_recv_fsm *fsm)
{
  fsm->state = J3P_RECV_STATE_STOP_BIT;
}

static void j3p_recv_state_done (volatile struct j3p_recv_fsm *fsm)
{
  fsm->state = J3P_RECV_STATE_DONE;
}

static void j3p_recv_state_error (volatile struct j3p_recv_fsm *fsm)
{
  fsm->state = J3P_RECV_STATE_ERR;
}

/* clock events */

static void j3p_recv_on_falling_start_bit (volatile struct j3p_recv_fsm *fsm)
{
  uint8_t line_value = fsm->read_line ();

  if (line_value) {
    /* Still on the mark */
  } else {
    j3p_recv_state_byte (fsm);
  }
}

static void j3p_recv_on_falling_byte (volatile struct j3p_recv_fsm *fsm)
{
  uint8_t line_value = fsm->read_line ();

  fsm->cur_byte >>= 1;

  if (line_value) {
    fsm->cur_byte |= 0x80;
  }

  fsm->bits_left--;

  if (fsm->bits_left == 0) {
    *fsm->buf = fsm->cur_byte;
    fsm->buf++;
    fsm->bytes_left--;
    j3p_recv_state_stop_bit (fsm);
  }
}

static void j3p_recv_on_falling_stop_bit (volatile struct j3p_recv_fsm *fsm)
{
  uint8_t line_value = fsm->read_line ();



  if (line_value) {
    if (fsm->bytes_left == 0) {
      j3p_recv_state_done (fsm);
    } else {
      j3p_recv_state_start_bit (fsm);
    }
  } else {
    j3p_recv_state_error (fsm);
  }
}

void j3p_recv_on_falling (volatile struct j3p_recv_fsm *fsm)
{
  switch (fsm->state) {
  case J3P_RECV_STATE_START_BIT:
    j3p_recv_on_falling_start_bit (fsm);
    break;

  case J3P_RECV_STATE_BYTE:
    j3p_recv_on_falling_byte (fsm);
    break;

  case J3P_RECV_STATE_STOP_BIT:
    j3p_recv_on_falling_stop_bit (fsm);
    break;

  case J3P_RECV_STATE_DONE:
  case J3P_RECV_STATE_ERR:
    break;
  }
}

/* initialization (once for each fsm run) */

void j3p_recv_init (volatile struct j3p_recv_fsm *fsm,
                    j3p_read_line_op read_line,
                    uint8_t bytes_in,
                    volatile uint8_t *recv_buf)
{
  fsm->read_line = read_line;
  fsm->bytes_left = bytes_in;
  fsm->buf = recv_buf;

  j3p_recv_state_start_bit (fsm);
}
