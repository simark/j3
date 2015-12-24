#ifndef _CONFIG_H
#define _CONFIG_H

#include <avr/io.h>

// The pin on which we receive the master clock.
#define MASTER_CLK_DDR DDRB
#define MASTER_CLK_PIN PINB
#define MASTER_CLK_MASK _BV(PB2)

// The pin we use to communicate to a slave, as a master.
#define COMM_MASTER_DDR DDRA
#define COMM_MASTER_PIN PINA
#define COMM_MASTER_PORT PORTA
#define COMM_MASTER_MASK _BV(PA0)

// The pin we use to communicate to a master, as a slave.
#define COMM_SLAVE_DDR DDRA
#define COMM_SLAVE_PIN PINA
#define COMM_SLAVE_PORT PORTA
#define COMM_SLAVE_MASK _BV(PA1)

#endif /* _CONFIG_H */
