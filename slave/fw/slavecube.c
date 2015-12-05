#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "config.h"

#include "j3p.h"



static struct j3p_master_ctx j3p_master_ctx_instance;

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

static uint16_t neighbour_query_cnt = 0;

static void rising (void) {
	j3p_master_on_rising (&j3p_master_ctx_instance);

	neighbour_query_cnt++;

	if (neighbour_query_cnt == J3P_POLL_CNT) {
		j3p_master_query (&j3p_master_ctx_instance);
		neighbour_query_cnt = 0;
	}
}

static void falling (void) {
	j3p_master_on_falling (&j3p_master_ctx_instance);
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
								 	 j3p_master_read_line);
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
