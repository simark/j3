#ifndef MASTER_FW_CONFIG_H_
#define MASTER_FW_CONFIG_H_

// TODO put the right values
#define MASTER_CLK_OUTPUT_DDR DDRA
#define MASTER_CLK_OUTPUT_PORT PORTA
#define MASTER_CLK_OUTPUT_MASK _BV(0)

// The pin we use to communicate to a slave, as a master.
#define COMM_MASTER_DDR DDRA
#define COMM_MASTER_PIN PINA
#define COMM_MASTER_PORT PORTA
#define COMM_MASTER_MASK _BV(PA1)

#endif /* MASTER_FW_CONFIG_H_ */
