#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "config.h"

#include "j3p.h"

#define max(a,b) (a > b ? a : b)

#define J3P_MASTER_TO_SLAVE_NUM_BYTES 5
#define J3P_SLAVE_TO_MASTER_NUM_BYTES 5
#define J3P_BUFSIZE max(J3P_MASTER_TO_SLAVE_NUM_BYTES, J3P_SLAVE_TO_MASTER_NUM_BYTES)

static struct j3p_master_ctx j3p_master_ctx_instance;
static struct j3p_slave_ctx j3p_slave_ctx_instance;
static uint8_t j3p_master_buf[J3P_BUFSIZE];
static uint8_t j3p_slave_buf[J3P_BUFSIZE];

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

static void rising (void) {
  j3p_master_on_rising (&j3p_master_ctx_instance);
  j3p_slave_on_rising (&j3p_slave_ctx_instance);

  neighbour_query_cnt++;

  if (neighbour_query_cnt == J3P_POLL_CNT) {
    j3p_master_buf[0] = 'A';
    j3p_master_buf[1] = 'B';
    j3p_master_buf[2] = 'C';
    j3p_master_buf[3] = 'D';
    j3p_master_buf[4] = 'E';
    j3p_master_query (&j3p_master_ctx_instance);
    neighbour_query_cnt = 0;
  }
}

static void falling (void) {

  j3p_master_on_falling (&j3p_master_ctx_instance);
  j3p_slave_on_falling (&j3p_slave_ctx_instance);
}

ISR(EXT_INT0_vect) {
  PORTB |= _BV(PB0);

  if (MASTER_CLK_PIN & MASTER_CLK_MASK) {
    rising ();
  } else {
    falling ();
  }

  PORTB &= ~_BV(PB0);
}

static void j3p_master_recv_complete (void) {

}

static void j3p_slave_query (uint8_t *buf) {
  uint8_t i;

  for (i = 0; i < J3P_MASTER_TO_SLAVE_NUM_BYTES; i++) {
    buf[i]++;
  }
}

/*
static void init_timers (void)
{
  // Interrupt every 100us
  TCNT1 = 0;
  TCCR1B |= _BV(WGM12) | _BV(CS10);
  OCR1A = 800;
  TIMSK1 |= _BV(OCIE1A);
}*/

static void init_j3p (void)
{
  j3p_master_init (&j3p_master_ctx_instance,
                   j3p_master_line_up,
                   j3p_master_line_down,
                   j3p_master_read_line,
                   J3P_MASTER_TO_SLAVE_NUM_BYTES,
                   J3P_SLAVE_TO_MASTER_NUM_BYTES,
                   j3p_master_buf,
                   j3p_master_recv_complete);
  j3p_slave_init (&j3p_slave_ctx_instance,
                  j3p_slave_line_up,
                  j3p_slave_line_down,
                  j3p_slave_read_line,
                  J3P_MASTER_TO_SLAVE_NUM_BYTES,
                  J3P_SLAVE_TO_MASTER_NUM_BYTES,
                  j3p_slave_buf,
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
  DDRA |= _BV(PA7);

  init_master_clock_listen ();
  init_j3p ();

  sei();

  loop ();
  return 0;
}
