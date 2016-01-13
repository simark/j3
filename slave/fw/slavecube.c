#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdlib.h>
#include <string.h>
#include <util/atomic.h>

#include <common-config.h>
#include <tick.h>

#include "config.h"
#include "frame.h"
#include "font.h"

#include <j3p.h>

#define max(a,b) (a > b ? a : b)

/* shift register signals on and off */
#define SR_SXCP_ON()  do { SR_SXCP_PORT |= SR_SXCP_MASK; } while (0)
#define SR_SXCP_OFF() do { SR_SXCP_PORT &= ~SR_SXCP_MASK; } while (0)
#define SR_DS_ON()    do { SR_DS_PORT |= SR_DS_MASK; } while (0)
#define SR_DS_OFF()   do { SR_DS_PORT &= ~SR_DS_MASK; } while (0)

/* display */
#define DISPLAY_TICKS_PER_ROW MS_TO_TICKS(2)

static volatile struct {
  // Comm with downstream
  uint8_t slave_has_answered;
  tick_t slave_query_start, slave_query_end;

  // Info we get from master (and have to give to slave)
  struct anim_word anim_word;
  uint8_t my_rank;  // zero-based rank of the slave in the word (first slave
                    // is zero)

  // Info we get from slave (and have to give to master)
  struct slave_seq slave_seq;

  struct j3p_master_ctx j3p_master_ctx;
  struct j3p_slave_ctx j3p_slave_ctx;
  union comm_buf master_buf;
  union comm_buf slave_buf;
} g_volatile_state;

static struct {
  // The built-in id, read from EEPRROM
  uint8_t my_id;

  /* display stuff */
  struct {
    uint8_t cur_row;
    tick_t row_tick_start, row_tick_end;
    struct frame cur_frame;
  } display;
} g_state;

static void j3p_master_line_up (void)
{
  COMM_MASTER_DDR &= ~COMM_MASTER_MASK;
}

static void j3p_master_line_down (void)
{
  COMM_MASTER_DDR |= COMM_MASTER_MASK;
}

static uint8_t j3p_master_read_line (void)
{
  return (COMM_MASTER_PIN & COMM_MASTER_MASK) != 0;
}

static void j3p_slave_line_up (void)
{
  COMM_SLAVE_DDR &= ~COMM_SLAVE_MASK;
}

static void j3p_slave_line_down (void)
{
  COMM_SLAVE_DDR |= COMM_SLAVE_MASK;
}

static uint8_t j3p_slave_read_line (void)
{
  return (COMM_SLAVE_PIN & COMM_SLAVE_MASK) != 0;
}

/*
 * Called in the master's code, when it has receive a response from the slave.
 */
static void master_query_complete ()
{
  g_volatile_state.slave_has_answered = 1;

  /* Copy downstream cube ids from slave message */
  memcpy ((void *) &g_volatile_state.slave_seq,
          (void *) &g_volatile_state.master_buf.s2m.slave_seq,
          sizeof (g_volatile_state.slave_seq));
}

/*
 * Called in the master's code, when we consider the slave is not present.
 */
static void slave_timeout (void)
{
  memset ((void *) &g_volatile_state.slave_seq, 0,
          sizeof (g_volatile_state.slave_seq));
}

/*
 * Called in the slave's code, when we received a query from the master and
 * should reply something.
 */
static void slave_query_impl ()
{
  /* First, read in info from the master */
  g_volatile_state.my_rank = g_volatile_state.slave_buf.m2s.rank;
  memcpy ((void *) &g_volatile_state.anim_word,
          (void *) &g_volatile_state.slave_buf.m2s.anim_word,
          sizeof (g_volatile_state.anim_word));

  /* Then, fill the buffer with our info. */
  g_volatile_state.slave_buf.s2m.slave_seq.ids[0] = g_state.my_id;
  memcpy((void *) g_volatile_state.slave_buf.s2m.slave_seq.ids + 1,
         (void *) g_volatile_state.slave_seq.ids,
         sizeof (g_volatile_state.slave_buf.s2m.slave_seq.ids) -
           sizeof(g_volatile_state.slave_buf.s2m.slave_seq.ids[0]));
}

static void isr_rising (void)
{
  tick ();

  j3p_master_on_rising (&g_volatile_state.j3p_master_ctx);
  j3p_slave_on_rising (&g_volatile_state.j3p_slave_ctx);

  if (tick_expired (g_volatile_state.slave_query_start,
                      g_volatile_state.slave_query_end)) {
    /* If we are about to send another query and the slave hasn't
     * answered the previous one, assume he is not there. */
    if (!g_volatile_state.slave_has_answered) {
      slave_timeout ();
    }

    // Fill message to slave.
    g_volatile_state.master_buf.m2s.rank = g_volatile_state.my_rank + 1;
    memcpy ((void *) &g_volatile_state.master_buf.m2s.anim_word,
            (void *) &g_volatile_state.anim_word,
            sizeof (g_volatile_state.master_buf.m2s.anim_word));

    // Send it!;
    j3p_master_query (&g_volatile_state.j3p_master_ctx);
    g_volatile_state.slave_has_answered = 0;

    g_volatile_state.slave_query_start = get_tick ();
    g_volatile_state.slave_query_end =
      g_volatile_state.slave_query_start + MS_TO_TICKS(SLAVE_POLL_MS);
  }
}

static void isr_falling (void)
{

  j3p_master_on_falling (&g_volatile_state.j3p_master_ctx);
  j3p_slave_on_falling (&g_volatile_state.j3p_slave_ctx);
}

ISR(EXT_INT0_vect)
{
  if (MASTER_CLK_PIN & MASTER_CLK_MASK) {
    isr_rising ();
  } else {
    isr_falling ();
  }
}

static void init_j3p (void)
{
  j3p_master_init (&g_volatile_state.j3p_master_ctx,
                   j3p_master_line_up,
                   j3p_master_line_down,
                   j3p_master_read_line,
                   sizeof (struct m2s_data),
                   sizeof (struct s2m_data),
                   (volatile uint8_t *) &g_volatile_state.master_buf,
                   master_query_complete);
  j3p_slave_init (&g_volatile_state.j3p_slave_ctx,
                  j3p_slave_line_up,
                  j3p_slave_line_down,
                  j3p_slave_read_line,
                  sizeof (struct m2s_data),
                  sizeof (struct s2m_data),
                  (volatile uint8_t *) &g_volatile_state.slave_buf,
                  slave_query_impl);

  g_volatile_state.slave_query_start = get_tick ();
  g_volatile_state.slave_query_end =
    g_volatile_state.slave_query_start + MS_TO_TICKS(SLAVE_POLL_MS);
}


static void init_master_clock_listen (void)
{
  MASTER_CLK_DDR &= ~MASTER_CLK_MASK;

  /* Interrupt on both edges */
  MCUCR |= _BV(ISC00);

  /* Enable interrupt on INT0 */
  GIMSK |= _BV(INT0);
}

static void init_state (uint8_t my_id)
{
  memset (&g_state, 0, sizeof (g_state));

  g_state.my_id = my_id;
}

static void sr_sxcp_tick (void)
{
  SR_SXCP_ON();
  SR_SXCP_OFF();
}

static void sr_push_one_bit (uint8_t bit)
{
  if (bit & 1) {
    /* off because the pin is sinking the current for the LED to be on */
    SR_DS_OFF();
  } else {
    SR_DS_ON();
  }

  sr_sxcp_tick ();
}

static void sr_push_16_bits (uint16_t word)
{
  uint8_t i;

  for (i = 0; i < 16; ++i) {
    sr_push_one_bit (word & 1);
    word >>= 1;
  }

  /* one more SHCP/STCP since both are wired together */
  sr_sxcp_tick ();
}

static uint16_t display_row_to_sr_word (uint8_t row_index)
{
  uint16_t srword = 0;
  const struct frame_row *row = &g_state.display.cur_frame.rows[row_index];

  if (row->cols[0] & FRAME_RED_MASK) {
    srword |= _BV(1);
  }

  if (row->cols[0] & FRAME_GREEN_MASK) {
    srword |= _BV(0);
  }

  if (row->cols[0] & FRAME_BLUE_MASK) {
    srword |= _BV(2);
  }

  if (row->cols[1] & FRAME_RED_MASK) {
    srword |= _BV(3);
  }

  if (row->cols[1] & FRAME_GREEN_MASK) {
    srword |= _BV(4);
  }

  if (row->cols[1] & FRAME_BLUE_MASK) {
    srword |= _BV(5);
  }

  if (row->cols[2] & FRAME_RED_MASK) {
    srword |= _BV(8);
  }

  if (row->cols[2] & FRAME_GREEN_MASK) {
    srword |= _BV(7);
  }

  if (row->cols[2] & FRAME_BLUE_MASK) {
    srword |= _BV(6);
  }

  if (row->cols[3] & FRAME_RED_MASK) {
    srword |= _BV(10);
  }

  if (row->cols[3] & FRAME_GREEN_MASK) {
    srword |= _BV(12);
  }

  if (row->cols[3] & FRAME_BLUE_MASK) {
    srword |= _BV(9);
  }

  if (row->cols[4] & FRAME_RED_MASK) {
    srword |= _BV(14);
  }

  if (row->cols[4] & FRAME_GREEN_MASK) {
    srword |= _BV(11);
  }

  if (row->cols[4] & FRAME_BLUE_MASK) {
    srword |= _BV(13);
  }

  return srword;
}

static void display_write_row (uint8_t row_index)
{
  uint16_t sr_word = display_row_to_sr_word (row_index);

  sr_push_16_bits (sr_word);
}

struct display_row_io {
  volatile uint8_t *port;
  uint8_t mask;
};

static struct display_row_io display_row_ios[] = {
  { &ROW0_PORT, ROW0_MASK },
  { &ROW1_PORT, ROW1_MASK },
  { &ROW2_PORT, ROW2_MASK },
  { &ROW3_PORT, ROW3_MASK },
  { &ROW4_PORT, ROW4_MASK },
};

static void display_row_off (uint8_t row_index)
{
  *(display_row_ios[row_index].port) |= display_row_ios[row_index].mask;
}

static void display_row_on (uint8_t row_index)
{
  /* off because we're controlling the base of a PNP here */
  *(display_row_ios[row_index].port) &= ~display_row_ios[row_index].mask;
}

static void display_next_row (void)
{
  uint8_t prev_row = g_state.display.cur_row;

  /* find next row index */
  g_state.display.cur_row++;

  if (g_state.display.cur_row == DISPLAY_ROWS) {
    g_state.display.cur_row = 0;
  }

  ATOMIC_BLOCK (ATOMIC_FORCEON) {
    /* turn previous row off */
    display_row_off (prev_row);

    /* set columns as fast as possible */
    display_write_row (g_state.display.cur_row);

    /* turn next row on */
    display_row_on (g_state.display.cur_row);
  }
}

static void display_loop (void)
{
  if (tick_expired (g_state.display.row_tick_start,
                    g_state.display.row_tick_end)) {
    /* reset row timer */
    g_state.display.row_tick_start = get_tick ();
    g_state.display.row_tick_end =
      g_state.display.row_tick_start + DISPLAY_TICKS_PER_ROW;

    /* next row */
    display_next_row ();
  }
}

static void init_display (void)
{
  /* rows (all off) */
  ROW0_DDR |= ROW0_MASK;
  ROW1_DDR |= ROW1_MASK;
  ROW2_DDR |= ROW2_MASK;
  ROW3_DDR |= ROW3_MASK;
  ROW4_DDR |= ROW4_MASK;
  display_row_off(0);
  display_row_off(1);
  display_row_off(2);
  display_row_off(3);
  display_row_off(4);

  /* shift register */
  SR_SXCP_OFF ();
  SR_DS_OFF ();
  SR_SXCP_DDR |= SR_SXCP_MASK;
  SR_DS_DDR |= SR_DS_MASK;

  /* initialize row timer */
  g_state.display.row_tick_start = get_tick ();
  g_state.display.row_tick_end =
    g_state.display.row_tick_start + DISPLAY_TICKS_PER_ROW;

  font_char_to_frame(1, &g_state.display.cur_frame);
}

static void loop (void)
{
  for (;;) {
    display_loop ();
  }
}

int main (void)
{
  uint8_t my_id = eeprom_read_byte (0);

  init_state (my_id);
  init_master_clock_listen ();
  init_j3p ();
  init_display ();

  sei();

  loop ();
  return 0;
}
