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

static volatile struct {
  // Comm with downstream
  uint8_t slave_has_answered;
  uint16_t slave_query_timer;

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
  j3p_master_on_rising (&g_volatile_state.j3p_master_ctx);
  j3p_slave_on_rising (&g_volatile_state.j3p_slave_ctx);

  g_volatile_state.slave_query_timer++;

  if (g_volatile_state.slave_query_timer >= SLAVE_QUERY_TIMER_MAX) {
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
    g_volatile_state.slave_query_timer = 0;
  }

  tick ();
}

static void isr_falling (void)
{

  j3p_master_on_falling (&g_volatile_state.j3p_master_ctx);
  j3p_slave_on_falling (&g_volatile_state.j3p_slave_ctx);
}

ISR(EXT_INT0_vect)
{
  PORTB |= _BV(PB1);

  if (MASTER_CLK_PIN & MASTER_CLK_MASK) {
    isr_rising ();
  } else {
    isr_falling ();
  }

  PORTB &= ~_BV(PB1);
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
