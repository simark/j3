#include <j3p.h>
#include <common-config.h>
#include <string.h>
#include <util/atomic.h>

#include "config.h"

static struct j3p_master_ctx g_j3p_master_ctx;

static comm_buf g_master_buf;

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
} g_state;

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

/* Called when a slave query times out. */
static void slave_timeout (void)
{

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

  memcpy (g_state.slave_sequence, buf->s2m.cubes, MAX_CUBES);
}

static void init_state (void)
{
  memset (&g_state, 0, sizeof (g_state));
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
#define TIMER_TOP (TIMER_FREQ / MASTER_CLK_FREQ)

  OCR0 = TIMER_TOP;

  // Reset the timer, just for fun.
  TCNT0 = 0;

  // Enable timer interrupt
  TIMSK |= _BV(OCIE0);
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

  sei();

  loop();
}
