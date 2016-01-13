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
#define COMM_MASTER_MASK _BV(PA6)

// The pin we use to communicate to a master, as a slave.
#define COMM_SLAVE_DDR DDRA
#define COMM_SLAVE_PIN PINA
#define COMM_SLAVE_PORT PORTA
#define COMM_SLAVE_MASK _BV(PA7)

// Number of rows and columns
#define DISPLAY_ROWS  5
#define DISPLAY_COLS  5

// Rows pins
#define ROW0_DDR      DDRA
#define ROW1_DDR      DDRA
#define ROW2_DDR      DDRA
#define ROW3_DDR      DDRA
#define ROW4_DDR      DDRA
#define ROW0_PORT     PORTA
#define ROW1_PORT     PORTA
#define ROW2_PORT     PORTA
#define ROW3_PORT     PORTA
#define ROW4_PORT     PORTA
#define ROW0_MASK     _BV(PA1)
#define ROW1_MASK     _BV(PA2)
#define ROW2_MASK     _BV(PA3)
#define ROW3_MASK     _BV(PA4)
#define ROW4_MASK     _BV(PA5)

// Shift register pins
#define SR_SXCP_DDR   DDRB
#define SR_SXCP_PORT  PORTB
#define SR_SXCP_MASK  _BV(PB1)
#define SR_DS_DDR     DDRB
#define SR_DS_PORT    PORTB
#define SR_DS_MASK    _BV(PB0)

#endif /* _CONFIG_H */
