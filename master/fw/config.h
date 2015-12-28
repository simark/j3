#ifndef MASTER_FW_CONFIG_H_
#define MASTER_FW_CONFIG_H_

#include <common-config.h>

#define MS_TO_TICKS(ms) ((MASTER_CLK_FREQ * (ms)) / 1000)

typedef uint16_t tick_t;
#define BTN_DEBOUNCE_MS 10UL
#define BTN_DEBOUNCE_TICKS MS_TO_TICKS(BTN_DEBOUNCE_MS)
#define INPUT_LONG_PRESS_MS 1000UL
#define INPUT_LONG_PRESS_TICKS MS_TO_TICKS(INPUT_LONG_PRESS_MS)

#define NUM_CUSTOM_ANIM_WORDS 32

// TODO put the right values
#define MASTER_CLK_OUTPUT_DDR DDRD
#define MASTER_CLK_OUTPUT_PORT PORTD
#define MASTER_CLK_OUTPUT_MASK _BV(PD5)

// The pin we use to communicate to a slave, as a master.
#define COMM_MASTER_DDR DDRD
#define COMM_MASTER_PIN PIND
#define COMM_MASTER_PORT PORTD
#define COMM_MASTER_MASK _BV(PD4)

#define STATUS_LED_RED_DDR DDRC
#define STATUS_LED_RED_PORT PORTC
#define STATUS_LED_RED_MASK _BV(PC4)
#define STATUS_LED_BLUE_DDR DDRC
#define STATUS_LED_BLUE_PORT PORTC
#define STATUS_LED_BLUE_MASK _BV(PC3)
#define STATUS_LED_GREEN_DDR DDRC
#define STATUS_LED_GREEN_PORT PORTC
#define STATUS_LED_GREEN_MASK _BV(PC2)
#define STATUS_LED_ANODE_DDR DDRC
#define STATUS_LED_ANODE_PORT PORTC
#define STATUS_LED_ANODE_MASK _BV(PC5)

#define BTN0_DDR DDRA
#define BTN0_PIN PINA
#define BTN0_MASK _BV(PA0)
#define BTN1_DDR DDRA
#define BTN1_PIN PINA
#define BTN1_MASK _BV(PA1)

#define PIEZO_PASSIVE_DDR DDRC
#define PIEZO_PASSIVE_PORT PORTC
#define PIEZO_PASSIVE_MASK _BV(PC0)
#define PIEZO_ACTIVE_DDR DDRD
#define PIEZO_ACTIVE_MASK _BV(PD7)

#endif /* MASTER_FW_CONFIG_H_ */
