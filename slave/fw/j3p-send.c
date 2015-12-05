#include "j3p-send.h"

/* J3P send state transitions */

static void j3p_send_state_break (struct j3p_send_fsm *fsm) {
  fsm->state = J3P_SEND_STATE_BREAK;
  fsm->bits_left = J3P_SEND_BREAK_NUM_BITS;
}

static void j3p_send_state_mark_after_break (struct j3p_send_fsm *fsm) {
  fsm->state = J3P_SEND_STATE_MARK_AFTER_BREAK;
  fsm->bits_left = J3P_SEND_MARK_AFTER_BREAK_NUM_BITS;
}

static void j3p_send_state_byte_start_bit (struct j3p_send_fsm *fsm) {
  fsm->state = J3P_SEND_STATE_BYTE_START_BIT;
}

static void j3p_send_state_byte (struct j3p_send_fsm *fsm) {
  fsm->state = J3P_SEND_STATE_BYTE;
  fsm->bits_left = 8;
  fsm->cur_byte = 0x3b;
}

static void j3p_send_state_byte_stop_bit (struct j3p_send_fsm *fsm) {
  fsm->state = J3P_SEND_STATE_BYTE_STOP_BIT;
}

static void j3p_send_state_done (struct j3p_send_fsm *fsm) {
  fsm->state = J3P_SEND_STATE_DONE;
}

/* J3P send events */

static void j3p_send_on_rising_break (struct j3p_send_fsm *fsm) {
  fsm->line_down ();

  fsm->bits_left--;

  if (fsm->bits_left == 0) {
    j3p_send_state_mark_after_break (fsm);
  }
}

static void j3p_send_on_rising_mark_after_break (struct j3p_send_fsm *fsm) {
  fsm->line_up ();

  fsm->bits_left--;

  if (fsm->bits_left == 0) {
    j3p_send_state_byte_start_bit (fsm);
  }
}

static void j3p_send_on_rising_byte_start_bit (struct j3p_send_fsm *fsm) {
  fsm->line_down ();

  j3p_send_state_byte (fsm);
}

static void j3p_send_on_rising_byte (struct j3p_send_fsm *fsm) {
  if (fsm->cur_byte & 1) {
    fsm->line_up ();
  } else {
    fsm->line_down ();
  }

  fsm->cur_byte >>= 1;
  fsm->bits_left--;

  if (fsm->bits_left == 0) {
    j3p_send_state_byte_stop_bit (fsm);
  }
}

static void j3p_send_on_rising_byte_stop_bit (struct j3p_send_fsm *fsm) {
  fsm->line_up ();

  j3p_send_state_done (fsm);
}

static void j3p_send_on_rising_done (struct j3p_send_fsm *fsm) {
  fsm->line_up ();
}

void j3p_send_on_rising (struct j3p_send_fsm *fsm) {
  switch (fsm->state) {
  case J3P_SEND_STATE_BREAK:
    j3p_send_on_rising_break (fsm);
    break;

  case J3P_SEND_STATE_MARK_AFTER_BREAK:
    j3p_send_on_rising_mark_after_break (fsm);
    break;

  case J3P_SEND_STATE_BYTE_START_BIT:
    j3p_send_on_rising_byte_start_bit (fsm);
    break;

  case J3P_SEND_STATE_BYTE:
    j3p_send_on_rising_byte (fsm);
    break;

  case J3P_SEND_STATE_BYTE_STOP_BIT:
    j3p_send_on_rising_byte_stop_bit (fsm);
    break;

  case J3P_SEND_STATE_DONE:
    j3p_send_on_rising_done (fsm);
    break;
  }
}

/* Reset the FSM to its initial state */

void j3p_send_reset (struct j3p_send_fsm *fsm) {
  j3p_send_state_break (fsm);
}

/* One time initialization */

void j3p_send_init (struct j3p_send_fsm *fsm,
                    j3p_send_set_line_op line_up,
                    j3p_send_set_line_op line_down) {
  fsm->line_up = line_up;
  fsm->line_down = line_down;

  j3p_send_reset (fsm);
}
