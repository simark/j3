#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdlib.h>
#include <string.h>

#include <common-config.h>
#include <tick.h>

#include "config.h"

#include <j3p.h>

#define max(a,b) (a > b ? a : b)

static struct j3p_master_ctx g_j3p_master_ctx;
static struct j3p_slave_ctx g_j3p_slave_ctx;
static comm_buf g_master_buf;
static comm_buf g_slave_buf;

static struct {
  // The built-in id, read from EEPRROM
  uint8_t my_id;

  // Comm with downstream
  uint8_t slave_has_answered;
  uint16_t slave_query_timer;

  // Info we get from master (and have to give to slave)
  struct anim_word anim_word;
  uint8_t my_rank;  // zero-based rank of the slave in the word (first slave
                    // is zero)

  // Info we get from slave (and have to give to master)
  struct slave_seq slave_seq;
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
static void master_query_complete (uint8_t *_buf)
{
  comm_buf *buf = (comm_buf *) _buf;

  g_state.slave_has_answered = 1;

  /* Copy downstream cube ids from slave message */
  memcpy (&g_state.slave_seq, &buf->s2m.slave_seq, sizeof (g_state.slave_seq));
}

/*
 * Called in the master's code, when we consider the slave is not present.
 */
static void slave_timeout (void)
{
  memset (&g_state.slave_seq, 0,
          sizeof (g_state.slave_seq));
}

/*
 * Called in the slave's code, when we received a query from the master and
 * should reply something.
 */
static void slave_query_impl (uint8_t *_buf)
{
  comm_buf *buf = (comm_buf *) _buf;

  /* First, read in info from the master */
  g_state.my_rank = buf->m2s.rank;
  memcpy (&g_state.anim_word, &buf->m2s.anim_word, sizeof (struct anim_word));

  /* Then, fill the buffer with our info. */
  buf->s2m.slave_seq.ids[0] = g_state.my_id;
  memcpy(buf->s2m.slave_seq.ids + 1, g_state.slave_seq.ids,
         sizeof(buf->s2m.slave_seq.ids) - sizeof(buf->s2m.slave_seq.ids[0]));
}

static void rising (void)
{
  j3p_master_on_rising (&g_j3p_master_ctx);
  j3p_slave_on_rising (&g_j3p_slave_ctx);

  g_state.slave_query_timer++;

  if (g_state.slave_query_timer >= SLAVE_QUERY_TIMER_MAX) {
    /* If we are about to send another query and the slave hasn't
     * answered the previous one, assume he is not there. */
    if (!g_state.slave_has_answered) {
      slave_timeout ();
    }

    // Fill message to slave.
    g_master_buf.m2s.rank = g_state.my_rank + 1;
    memcpy (&g_master_buf.m2s.anim_word, &g_state.anim_word, sizeof (struct anim_word));

    // Send it!;
    j3p_master_query (&g_j3p_master_ctx);
    g_state.slave_has_answered = 0;
    g_state.slave_query_timer = 0;
  }

  tick ();
}

static void falling (void)
{

  j3p_master_on_falling (&g_j3p_master_ctx);
  j3p_slave_on_falling (&g_j3p_slave_ctx);
}

ISR(EXT_INT0_vect)
{
  PORTB |= _BV(PB1);

  if (MASTER_CLK_PIN & MASTER_CLK_MASK) {
    rising ();
  } else {
    falling ();
  }

  PORTB &= ~_BV(PB1);
}

static void init_j3p (void)
{
  j3p_master_init (&g_j3p_master_ctx,
                   j3p_master_line_up,
                   j3p_master_line_down,
                   j3p_master_read_line,
                   sizeof (struct m2s_data),
                   sizeof (struct s2m_data),
                   (uint8_t *) &g_master_buf,
                   master_query_complete);
  j3p_slave_init (&g_j3p_slave_ctx,
                  j3p_slave_line_up,
                  j3p_slave_line_down,
                  j3p_slave_read_line,
                  sizeof (struct m2s_data),
                  sizeof (struct s2m_data),
                  (uint8_t *)&g_slave_buf,
                  slave_query_impl);
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

static void loop (void)
{
  for (;;);
}

int main (void)
{
  /* Debug pin */
  DDRB |= _BV(PB0);
  DDRB |= _BV(PB1);
  DDRA |= _BV(PA2) | _BV(PA3);

  uint8_t my_id = eeprom_read_byte (0);

  init_state (my_id);
  init_master_clock_listen ();
  init_j3p ();


  sei();

  loop ();
  return 0;
}
