#ifndef _CONFIG_H
#define _CONFIG_H

#include <avr/io.h>

/* Frequency of the clock we receive */
#define MASTER_CLK_FREQ 10000 // hz
#define MASTER_CLK_DDR DDRB
#define MASTER_CLK_PIN PINB
#define MASTER_CLK_MASK _BV(PB2)

/* Frequency at which we want to query our neighbour */
#define J3P_POLL_FREQ 5 // hz
#define J3P_POLL_CNT (MASTER_CLK_FREQ / J3P_POLL_FREQ)

#define J3P_MASTER_DDR DDRA
#define J3P_MASTER_PIN PINA
#define J3P_MASTER_PORT PORTA
#define J3P_MASTER_MASK _BV(PA0)

#define J3P_SLAVE_DDR DDRA
#define J3P_SLAVE_PIN PINA
#define J3P_SLAVE_PORT PORTA
#define J3P_SLAVE_MASK _BV(PA1)

#endif /* _CONFIG_H */
