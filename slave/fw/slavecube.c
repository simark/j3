#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

#include "j3p.h"

#define max(a,b) (a > b ? a : b)

static struct j3p_master_ctx j3p_master_ctx_instance;
static struct j3p_slave_ctx j3p_slave_ctx_instance;

#define MAX_CUBES 6

struct master_to_slave_data {
  uint8_t idx;
  uint8_t word[MAX_CUBES];
};

struct slave_to_master_data {
  uint8_t cubes[MAX_CUBES];
};

static struct {
  uint8_t known_downstream_cubes[MAX_CUBES];
  uint8_t slave_has_answered;
  uint8_t my_id;
  uint16_t slave_query_timer;
} g_state;

union {
  struct master_to_slave_data out;
  struct slave_to_master_data in;
} g_master_buf;

union {
  struct master_to_slave_data in;
  struct slave_to_master_data out;
} g_slave_buf;


static void j3p_master_line_up (void)
{
  J3P_MASTER_DDR &= ~J3P_MASTER_MASK;
}

static void j3p_master_line_down (void)
{
  J3P_MASTER_DDR |= J3P_MASTER_MASK;
}

static uint8_t j3p_master_read_line (void)
{
  return (J3P_MASTER_PIN & J3P_MASTER_MASK) != 0;
}

static void j3p_slave_line_up (void)
{
  J3P_SLAVE_DDR &= ~J3P_SLAVE_MASK;
}

static void j3p_slave_line_down (void)
{
  J3P_SLAVE_DDR |= J3P_SLAVE_MASK;
}

static uint8_t j3p_slave_read_line (void)
{
  return (J3P_SLAVE_PIN & J3P_SLAVE_MASK) != 0;
}

/*
 * Called in the master's code, when it has receive a response from the slave.
 */
static void slave_query_complete (uint8_t *buf)
{
  struct slave_to_master_data *s = (struct slave_to_master_data *) buf;

  g_state.slave_has_answered = 1;

  /* Copy downstream cube ids from slave message */
  memcpy (g_state.known_downstream_cubes, s->cubes, MAX_CUBES);
}

/*
 * Called in the master's code, when we consider the slave is not present.
 */
static void slave_timeout (void)
{
  memset (g_state.known_downstream_cubes, 0,
          sizeof (g_state.known_downstream_cubes));
  PORTB |= _BV(PB0);
}

/*
 * Called in the slave's code, when we received a query from the master and
 * should reply something.
 */
static void slave_query_impl (uint8_t *buf)
{
  /* First, read in info from the master (TODO) */
  //struct master_to_slave_data *m2s = (struct master_to_slave_data *) buf;

  /* Then, fill the buffer with our info. */
  struct slave_to_master_data *s2m = (struct slave_to_master_data *) buf;
  s2m->cubes[0] = g_state.my_id;
  memcpy (s2m->cubes + 1, g_state.known_downstream_cubes, MAX_CUBES - 1);
}

static void rising (void)
{
  j3p_master_on_rising (&j3p_master_ctx_instance);
  j3p_slave_on_rising (&j3p_slave_ctx_instance);

  g_state.slave_query_timer++;

  if (g_state.slave_query_timer >= SLAVE_QUERY_TIMER_VALUE) {
    /* If we are about to send another query and the slave hasn't
     * answered the previous one, assume he is not there. */
    if (!g_state.slave_has_answered) {
      slave_timeout ();
    } else {
      PORTB &= ~_BV(PB0);
    }

    g_state.slave_has_answered = 0;
    j3p_master_query (&j3p_master_ctx_instance);
    g_state.slave_query_timer = 0;
  }
}

static void falling (void)
{

  j3p_master_on_falling (&j3p_master_ctx_instance);
  j3p_slave_on_falling (&j3p_slave_ctx_instance);
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
  j3p_master_init (&j3p_master_ctx_instance,
                   j3p_master_line_up,
                   j3p_master_line_down,
                   j3p_master_read_line,
                   sizeof (struct master_to_slave_data),
                   sizeof (struct slave_to_master_data),
                   (uint8_t *) &g_master_buf,
                   slave_query_complete);
  j3p_slave_init (&j3p_slave_ctx_instance,
                  j3p_slave_line_up,
                  j3p_slave_line_down,
                  j3p_slave_read_line,
                  sizeof (struct master_to_slave_data),
                  sizeof (struct slave_to_master_data),
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
