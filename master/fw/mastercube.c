#include <j3p.h>
#include <common-config.h>
#include <string.h>
#include <util/atomic.h>
#include <util/delay.h>
#include "config.h"

static struct j3p_master_ctx g_j3p_master_ctx;

static comm_buf g_master_buf;

struct btn_state {
  enum btn_state_value {
    BTN_UNPRESSED = 0,
    BTN_DEBOUNCING,
    BTN_PRESSED,
  } value;

  tick_t start, end;

  void (*pressed_cb)(uint8_t long_press);
};


static struct {
  // The word we currently display
  uint8_t word[MAX_CUBES];
  enum anim_pattern patterns[MAX_CUBES];

  // Our idea of the present slaves.  The slave ids start at 1, so 0 means
  // "no slave present".
  uint8_t slave_sequence[MAX_CUBES];

  // Incremented once per clock cycle.  Once this reaches SLAVE_QUERY_TIMER_MAX,
  // we initiate a slave query.
  uint16_t slave_query_timer;
  uint8_t slave_has_answered;

  tick_t tick;

  struct btn_state btn0;
  struct btn_state btn1;
  struct input_state {
    enum {
      INPUT_ALL_UNPRESSED = 0,
      INPUT_BTN0_PENDING,
      INPUT_BTN1_PENDING,
      INPUT_DONE,
    } value;

    tick_t start, end;
  } input_fsm;
} g_state;

uint8_t tick_expired(tick_t start, tick_t end)
{
    tick_t current = g_state.tick;
    if (start < end) {
        return current < start || current > end;
    } else {
        return current < start && current > end;
    }
}

/*
 * Get the current sequence of slaves present.
 *
 * @param slave_sequence Pointer to an array of MAX_CUBES uint8_t's.
 */
void get_slave_sequence (uint8_t *slave_sequence)
{
  ATOMIC_BLOCK (ATOMIC_FORCEON) {
    memcpy (slave_sequence, g_state.slave_sequence, MAX_CUBES);
  }
}

/*
 * Get the number of slaves present.
 */
uint8_t get_slave_count (void)
{
  uint8_t cnt = 0;

  ATOMIC_BLOCK (ATOMIC_FORCEON) {
    for (cnt = 0;
         cnt < MAX_CUBES && g_state.slave_sequence[cnt] != 0;
         cnt++);
  }

  return cnt;
}

/*
 * Set the currently displayed word and animation pattern.  The information is<
 * so that it will be propagated the next time we send a message to the
 * slaves.
 *
 * @param word Pointer to an array of MAX_CUBES characters.  Unused characters
 *             should be set to 0.
 * @param patterns Pointer to an array of MAX_CUBES animation patterns.  */

void set_current_word (uint8_t *word, enum anim_pattern *patterns)
{
  ATOMIC_BLOCK (ATOMIC_FORCEON) {
    memcpy (g_state.word, word, sizeof (*word) * MAX_CUBES);
    memcpy (g_state.patterns, patterns, sizeof (*patterns) * MAX_CUBES);
  }
}

static void status_led_red (void)
{
  STATUS_LED_RED_PORT &= ~STATUS_LED_RED_MASK;
  STATUS_LED_BLUE_PORT |= STATUS_LED_BLUE_MASK;
  STATUS_LED_GREEN_PORT |= STATUS_LED_GREEN_MASK;
}

static void status_led_green (void)
{
  STATUS_LED_RED_PORT |= STATUS_LED_RED_MASK;
  STATUS_LED_BLUE_PORT |= STATUS_LED_BLUE_MASK;
  STATUS_LED_GREEN_PORT &= ~STATUS_LED_GREEN_MASK;
}

static void status_led_blue (void)
{
  STATUS_LED_RED_PORT |= STATUS_LED_RED_MASK;
  STATUS_LED_BLUE_PORT &= ~STATUS_LED_BLUE_MASK;
  STATUS_LED_GREEN_PORT |= STATUS_LED_GREEN_MASK;
}

static void btn_state_change(struct btn_state *state,
                             uint8_t read_value)
{
  switch (state->value) {
  case BTN_UNPRESSED:
    if (read_value) {
      state->value = BTN_DEBOUNCING;
      state->start = g_state.tick;
      state->end = g_state.tick + BTN_DEBOUNCE_TICKS;
    } else {
      state->value = BTN_UNPRESSED;
    }
    break;

  case BTN_DEBOUNCING:
    if (read_value) {
      if (tick_expired (state->start, state->end)) {
        // It looks like the real thing!
        state->value = BTN_PRESSED;
      } else {
        // Wait some more.
        state->value = BTN_DEBOUNCING;
      }
    } else {
      state->value = BTN_UNPRESSED;
    }
    break;

  case BTN_PRESSED:
    if (read_value) {
      state->value = BTN_PRESSED;
    } else {
      state->value = BTN_UNPRESSED;
    }
    break;
  }
}

enum input_event {
  IE_BTN0_SHORT,
  IE_BTN0_LONG,
  IE_BTN1_SHORT,
  IE_BTN1_LONG,
  IE_BOTH,
};

static void input_event (enum input_event ev)
{
  switch (ev) {
  case IE_BTN0_SHORT:
    break;
  case IE_BTN0_LONG:
    status_led_green();
    break;
  case IE_BTN1_SHORT:
    break;
  case IE_BTN1_LONG:
    status_led_red();
    break;
  case IE_BOTH:
    status_led_blue();
    break;
  }
}

static void input_state_change (struct input_state *state,
                                enum btn_state_value btn0,
                                enum btn_state_value btn1)
{
  switch (state->value) {
  case INPUT_ALL_UNPRESSED:
    if (btn0 == BTN_PRESSED) {
      state->start = g_state.tick;
      state->end = g_state.tick + INPUT_LONG_PRESS_TICKS;
      state->value = INPUT_BTN0_PENDING;
    } else if (btn1 == BTN_PRESSED) {
      state->start = g_state.tick;
      state->end = g_state.tick + INPUT_LONG_PRESS_TICKS;
      state->value = INPUT_BTN1_PENDING;
    } else {
      state->value = INPUT_ALL_UNPRESSED;
    }
    break;

  case INPUT_BTN0_PENDING:
    if (btn0 == BTN_UNPRESSED) {
      input_event (IE_BTN0_SHORT);
      state->value = INPUT_DONE;
    } else if (tick_expired (state->start,
                             state->end)) {
      if (btn1 == BTN_PRESSED) {
        input_event (IE_BOTH);
        state->value = INPUT_DONE;
      } else {
        input_event (IE_BTN0_LONG);
        state->value = INPUT_DONE;
      }
    } else {
      state->value = INPUT_BTN0_PENDING;
    }
    break;

  case INPUT_BTN1_PENDING:
    if (btn1 == BTN_UNPRESSED) {
      input_event (IE_BTN1_SHORT);
      state->value = INPUT_DONE;
    } else if (tick_expired (state->start,
                             state->end)) {
      if (btn0 == BTN_PRESSED) {
        input_event (IE_BOTH);
        state->value = INPUT_DONE;
      } else {
        input_event (IE_BTN1_LONG);
        state->value = INPUT_DONE;
      }
    } else {
      state->value = INPUT_BTN1_PENDING;
    }
    break;

  case INPUT_DONE:
    if (btn0 == BTN_UNPRESSED
        && btn1 == BTN_UNPRESSED) {
      state->value = INPUT_ALL_UNPRESSED;
    } else {
      state->value = INPUT_DONE;
    }
    break;
  }
}

static uint8_t btn0_val (void)
{
  return BTN0_PIN & BTN0_MASK;
}

static uint8_t btn1_val (void)
{
  return BTN1_PIN & BTN1_MASK;
}

static void check_inputs (void)
{
  btn_state_change (&g_state.btn0, btn0_val ());
  btn_state_change (&g_state.btn1, btn1_val ());
  input_state_change (&g_state.input_fsm,
                      g_state.btn0.value,
                      g_state.btn1.value);
}

/* Called when a slave query times out. */
static void slave_timeout (void)
{
  //status_led_red ();
}

static void slave_is_alive (void)
{
  //status_led_green();
  //status_led_blue ();
}

static void rising (void)
{
  j3p_master_on_rising (&g_j3p_master_ctx);

  g_state.slave_query_timer++;

  if (g_state.slave_query_timer >= SLAVE_QUERY_TIMER_MAX) {
    /* If we are about to send another query and the slave hasn't
     * answered the previous one, assume he is not there. */
    if (!g_state.slave_has_answered) {
      slave_timeout ();
    }

    // Fill message to slave.
    g_master_buf.m2s.rank = 0;
    memcpy (g_master_buf.m2s.word, g_state.word, MAX_CUBES);
    memcpy (g_master_buf.m2s.patterns, g_state.patterns, MAX_CUBES);

    // Send it!
    j3p_master_query (&g_j3p_master_ctx);
    g_state.slave_has_answered = 0;
    g_state.slave_query_timer = 0;
  }

  g_state.tick++;
  check_inputs ();
}

static void falling (void)
{
  j3p_master_on_falling (&g_j3p_master_ctx);
}

ISR(TIMER0_COMP_vect)
{
  // FIXME: I think we could read back from PORT, but I am not sure.  I put this
  // just to be sure.
  static uint8_t clock_value = 0;

  if (clock_value == 0) {
    clock_value = 1;
    MASTER_CLK_OUTPUT_PORT |= MASTER_CLK_OUTPUT_MASK;
    rising ();
  } else {
    clock_value = 0;
    MASTER_CLK_OUTPUT_PORT &= ~MASTER_CLK_OUTPUT_MASK;
    falling ();
  }
}

static void master_line_up (void)
{
  COMM_MASTER_DDR &= ~COMM_MASTER_MASK;
}

static void master_line_down (void)
{
  COMM_MASTER_DDR |= COMM_MASTER_MASK;
}

static uint8_t master_read_line (void)
{
  return (COMM_MASTER_PIN & COMM_MASTER_MASK) != 0;
  return 0;
}

static void master_query_complete (uint8_t *_buf)
{
  comm_buf *buf = (comm_buf *) _buf;

  g_state.slave_has_answered = 1;
  slave_is_alive();

  memcpy (g_state.slave_sequence, buf->s2m.cubes, MAX_CUBES);
}

static void init_state (void)
{
  memset (&g_state, 0, sizeof (g_state));
  g_state.word[0] = 'j';
  g_state.word[1] = 'e';
  g_state.word[2] = 'r';
  g_state.word[3] = 'o';
  g_state.word[4] = 'm';
  g_state.word[5] = 'e';
}

static void init_comm (void)
{
  // TODO: init comm pins
  j3p_master_init (&g_j3p_master_ctx,
                   master_line_up, master_line_down,
                   master_read_line,
                   sizeof(struct m2s_data),
                   sizeof(struct s2m_data),
                   (uint8_t *) &g_master_buf,
                   master_query_complete);
}

static void init_clk (void)
{
  MASTER_CLK_OUTPUT_DDR |= MASTER_CLK_OUTPUT_MASK;
  MASTER_CLK_OUTPUT_PORT &= ~MASTER_CLK_OUTPUT_MASK;

  // CTC, clk/8 (= 1 MHz)
  TCCR0 |= _BV(WGM01) | _BV(CS01);

#define TIMER_PRESCALER 8
#define TIMER_FREQ (F_CPU / TIMER_PRESCALER)
#define TIMER_TOP (TIMER_FREQ / (2 * MASTER_CLK_FREQ))

  OCR0 = TIMER_TOP;

  // Reset the timer, just for fun.
  TCNT0 = 0;

  // Enable timer interrupt
  TIMSK |= _BV(OCIE0);
}

static void init_state_led (void)
{
  STATUS_LED_RED_DDR |= STATUS_LED_RED_MASK;
  STATUS_LED_GREEN_DDR |= STATUS_LED_GREEN_MASK;
  STATUS_LED_BLUE_DDR |= STATUS_LED_BLUE_MASK;
  STATUS_LED_ANODE_DDR |= STATUS_LED_ANODE_MASK;

  STATUS_LED_RED_PORT |= STATUS_LED_RED_MASK;
  STATUS_LED_GREEN_PORT |= STATUS_LED_GREEN_MASK;
  STATUS_LED_BLUE_PORT |= STATUS_LED_BLUE_MASK;
  STATUS_LED_ANODE_PORT |= STATUS_LED_ANODE_MASK;
}

static void init_inputs (void)
{
  BTN0_DDR &= ~BTN0_MASK;
  BTN1_DDR &= ~BTN1_MASK;
}

static void loop ()
{
  for (;;);
}

int main (void)
{
  init_state ();
  init_comm ();
  init_clk ();
  init_state_led ();
  init_inputs ();

  status_led_red();
  _delay_ms(200);
  status_led_blue();
  _delay_ms(200);
  status_led_green();
  _delay_ms(200);

  sei();

  loop();
}
