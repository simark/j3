#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "config.h"

#include "j3p.h"

#define max(a,b) (a > b ? a : b)

static struct j3p_master_ctx j3p_master_ctx_instance;
static struct j3p_slave_ctx j3p_slave_ctx_instance;

#define MAX_CUBES 6

struct master_to_slave_data {
  uint8_t idx;
  uint8_t word[6];
};

struct slave_to_master_data {
  uint8_t cubes[6];
};

struct {

} g_cur_state;

#define BUFSIZE max(sizeof (struct master_to_slave_data), \
                    sizeof (struct slave_to_master_data))
static uint8_t g_master_buf[BUFSIZE];
static uint8_t g_slave_buf[BUFSIZE];

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

static uint16_t neighbour_query_cnt = 0;

static void rising (void)
{
  j3p_master_on_rising (&j3p_master_ctx_instance);
  j3p_slave_on_rising (&j3p_slave_ctx_instance);

  neighbour_query_cnt++;

  if (neighbour_query_cnt == J3P_POLL_CNT) {
    j3p_master_query (&j3p_master_ctx_instance);
    neighbour_query_cnt = 0;
  }
}

static void falling (void)
{

  j3p_master_on_falling (&j3p_master_ctx_instance);
  j3p_slave_on_falling (&j3p_slave_ctx_instance);
}

ISR(EXT_INT0_vect)
{
  PORTB |= _BV(PB0);

  if (MASTER_CLK_PIN & MASTER_CLK_MASK) {
    rising ();
  } else {
    falling ();
  }

  PORTB &= ~_BV(PB0);
}

static void j3p_master_query_complete (uint8_t *buf __attribute__ ((unused)))
{

}

static void j3p_slave_query (uint8_t *buf __attribute__ ((unused)))
{

}

static void init_j3p (void)
{
  j3p_master_init (&j3p_master_ctx_instance,
                   j3p_master_line_up,
                   j3p_master_line_down,
                   j3p_master_read_line,
                   sizeof (struct master_to_slave_data),
                   sizeof (struct slave_to_master_data),
                   g_master_buf,
                   j3p_master_query_complete);
  j3p_slave_init (&j3p_slave_ctx_instance,
                  j3p_slave_line_up,
                  j3p_slave_line_down,
                  j3p_slave_read_line,
                  sizeof (struct master_to_slave_data),
                  sizeof (struct slave_to_master_data),
                  g_slave_buf,
                  j3p_slave_query);
}

static void init_master_clock_listen (void)
{
  MASTER_CLK_DDR &= ~MASTER_CLK_MASK;

  /* Interrupt on both edges */
  MCUCR |= _BV(ISC00);

  /* Enable interrupt on INT0 */
  GIMSK |= _BV(INT0);
}

static void loop (void)
{
  for (;;);
}

int main (void)
{
  /* Debug pin */
  DDRB |= _BV(PB0);

  init_master_clock_listen ();
  init_j3p ();

  sei();

  loop ();
  return 0;
}
