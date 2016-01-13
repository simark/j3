#include <j3p.h>
#include <common-config.h>
#include <tick.h>
#include <string.h>
#include <util/atomic.h>
#include <util/delay.h>

#include "beep.h"
#include "settings.h"
#include "config.h"
#include "btn.h"
#include "input.h"
#include "menu.h"


static volatile struct {
  // The word we currently display
  struct anim_word anim_word;

  // Our idea of the present slaves.  The slave ids start at 1, so 0 means
  // "no slave present".
  struct slave_seq slave_seq;

  // Incremented once per clock cycle.  Once this reaches SLAVE_QUERY_TIMER_MAX,
  // we initiate a slave query.
  uint8_t slave_has_answered;
  tick_t slave_query_start, slave_query_end;

  // Communication with the slaves
  struct j3p_master_ctx j3p_master_ctx;
  union comm_buf master_buf;
} g_volatile_state;

static struct {
  struct btn_state btn0;
  struct btn_state btn1;
  struct input_state input;
  struct beep_state beep;
} g_state;

static const uint8_t beep_freq_table[] = {
  25,
  50,
  100,
  125,
  150,
  175,
  200,
  225,
  250,
};

/*
static void beep_melody (struct beep_note *song)
{
  if (settings_get_sound_melody ()) {
    beep (song);
  }
}*/

/*
 * Get the current sequence of slaves present.
 *
 * @param slave_sequence Pointer to an array of MAX_CUBES uint8_t's.
 */
void get_slave_sequence (struct slave_seq *slave_seq)
{
  ATOMIC_BLOCK (ATOMIC_FORCEON) {
    memcpy (slave_seq, (void *) &g_volatile_state.slave_seq, sizeof (*slave_seq));
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
         cnt < MAX_CUBES && g_volatile_state.slave_seq.ids[cnt] != 0;
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

void set_current_anim_word (struct anim_word *anim_word)
{
  ATOMIC_BLOCK (ATOMIC_FORCEON) {
    memcpy ((void *) &g_volatile_state.anim_word, anim_word, sizeof (struct anim_word));
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

static void input_event (enum input_event ev)
{
  menu_input_event (&menu_instance, ev);
}

/* Called when a slave query times out. */
static void isr_slave_timeout (void)
{
  memset ((void *) &g_volatile_state.slave_seq, 0,
          sizeof (g_volatile_state.slave_seq));;
}

static void isr_rising (void)
{
  tick ();

  j3p_master_on_rising (&g_volatile_state.j3p_master_ctx);

  if (tick_expired (g_volatile_state.slave_query_start,
                    g_volatile_state.slave_query_end)) {
    /* If we are about to send another query and the slave hasn't
     * answered the previous one, assume he is not there. */
    if (!g_volatile_state.slave_has_answered) {
      isr_slave_timeout ();
    }

    // Fill message to slave.
    g_volatile_state.master_buf.m2s.rank = 0;
    memcpy ((void *) &g_volatile_state.master_buf.m2s.anim_word,
            (void *) &g_volatile_state.anim_word,
            sizeof (g_volatile_state.master_buf.m2s.anim_word));

    // Send it!
    j3p_master_query (&g_volatile_state.j3p_master_ctx);
    g_volatile_state.slave_has_answered = 0;

    g_volatile_state.slave_query_start = get_tick ();
    g_volatile_state.slave_query_end =
      g_volatile_state.slave_query_start + MS_TO_TICKS(SLAVE_POLL_MS);
  }

}

static void isr_falling (void)
{
  j3p_master_on_falling (&g_volatile_state.j3p_master_ctx);
}

ISR(TIMER0_COMP_vect)
{
  // FIXME: I think we could read back from PORT, but I am not sure.  I put this
  // just to be sure.
  static uint8_t clock_value = 0;

  if (clock_value == 0) {
    clock_value = 1;
    MASTER_CLK_OUTPUT_PORT |= MASTER_CLK_OUTPUT_MASK;
    isr_rising ();
  } else {
    clock_value = 0;
    MASTER_CLK_OUTPUT_PORT &= ~MASTER_CLK_OUTPUT_MASK;
    isr_falling ();
  }
}

static void isr_master_line_up (void)
{
  COMM_MASTER_DDR &= ~COMM_MASTER_MASK;
}

static void isr_master_line_down (void)
{
  COMM_MASTER_DDR |= COMM_MASTER_MASK;
}

static uint8_t isr_master_read_line (void)
{
  return (COMM_MASTER_PIN & COMM_MASTER_MASK) != 0;
}

static void isr_master_query_complete (void)
{
  g_volatile_state.slave_has_answered = 1;

  memcpy ((void *) &g_volatile_state.slave_seq,
          (void *) &g_volatile_state.master_buf.s2m.slave_seq,
          sizeof (g_volatile_state.slave_seq));
}

static void init_state (void)
{
  memset ((void *) &g_state, 0, sizeof (g_state));
}

static void init_comm (void)
{
  j3p_master_init (&g_volatile_state.j3p_master_ctx,
                   isr_master_line_up, isr_master_line_down,
                   isr_master_read_line,
                   sizeof(struct m2s_data),
                   sizeof(struct s2m_data),
                   (volatile uint8_t *) &g_volatile_state.master_buf,
                   isr_master_query_complete);

  g_volatile_state.slave_query_start = get_tick ();
  g_volatile_state.slave_query_end =
    g_volatile_state.slave_query_start + MS_TO_TICKS(SLAVE_POLL_MS);
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

static void init_buttons (void)
{
  BTN0_DDR &= ~BTN0_MASK;
  BTN1_DDR &= ~BTN1_MASK;

  uint8_t btn0_val (void)
  {
    return BTN0_PIN & BTN0_MASK;
  }

  uint8_t btn1_val (void)
  {
    return BTN1_PIN & BTN1_MASK;
  }

  init_btn (&g_state.btn0, btn0_val);
  init_btn (&g_state.btn1, btn1_val);
}

static void init_inputs (void)
{
  enum btn_state_value btn0_state (void)
  {
    return btn_get_state (&g_state.btn0);
  }

  enum btn_state_value btn1_state (void)
  {
    return btn_get_state (&g_state.btn1);
  }

  init_input (&g_state.input, btn0_state, btn1_state, input_event);
}

static void init_alsa (void)
{
  // CTC mode, toggle OC2 on compare, prescaler = 64
  TCCR2 |= _BV(WGM21) | _BV(CS22);
  TCNT2 = 0x00;
  OCR2 = 0xff;

  PIEZO_PASSIVE_DDR |= PIEZO_PASSIVE_MASK;
  PIEZO_PASSIVE_PORT &= ~PIEZO_PASSIVE_MASK;

  PIEZO_ACTIVE_DDR |= PIEZO_ACTIVE_MASK;


  void beep_off (void)
  {
    TCCR2 &= ~_BV(COM20);
  }

  void beep_on (uint8_t freq_idx)
  {
    TCCR2 |= _BV(COM20);
    OCR2 = beep_freq_table[freq_idx];
  }

  init_beep (&g_state.beep, beep_on, beep_off);
}

static void render_loop (void)
{

}

static void status_led_loop (void)
{
  if (menu_active (&menu_instance)) {
    status_led_green ();
  } else if (get_slave_count() > 0) {
    status_led_blue ();
  } else {
    status_led_red ();
  }
}

tick_t s = 0, e = 1;
uint8_t cur_char = 0;
static void loop ()
{
  for (;;) {
    if (tick_expired(s, e)) {
      cur_char = (cur_char + 2) % 150;

      g_volatile_state.anim_word.text[0] = cur_char;

      s = get_tick(); e = s + MS_TO_TICKS(500);
    }
    btn_loop (&g_state.btn0);
    btn_loop (&g_state.btn1);
    input_loop (&g_state.input);
    beep_loop (&g_state.beep);
    render_loop ();
    status_led_loop ();
  }
}

static void play_intro (void)
{
  // TODO
}

int main (void)
{
  init_state ();
  init_comm ();
  init_clk ();
  init_state_led ();
  init_buttons ();
  init_inputs ();
  init_alsa ();
  init_settings ();
  init_menu (&menu_instance, &g_state.beep);

  DDRB &= ~(_BV(PB5) |_BV(PB6) |_BV(PB7));

  play_intro ();

  sei();

  loop();
}
