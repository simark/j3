#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>


static void loop (void)
{
	for (;;) {
		_delay_us(50);
		PINB |= _BV(PB0);
	}
}



int main (void)
{
	DDRB |= _BV(PB0);
	loop();
	return 0;
}
