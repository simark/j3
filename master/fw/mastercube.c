#include <j3p.h>
#include <common-config.h>
#include <string.h>

static struct j3p_master_ctx j3p_master_ctx_instance;

static comm_buf g_master_buf;

static struct {
  // The word we currently display
  uint8_t word[MAX_CUBES];
  enum anim_pattern patterns[MAX_CUBES];

  // Our idea of the present slaves.  The slave ids start at 1, so 0 means
  // "no slave present".
  uint8_t slave_sequence[MAX_CUBES];
} g_state;

/*
 * Get the current sequence of slaves present.
 *
 * @param slave_sequence Pointer to an array of MAX_CUBES uint8_t's.
 */
void get_slave_sequence (uint8_t *slave_sequence)
{
  memcpy (slave_sequence, g_state.slave_sequence, MAX_CUBES);
}

/*
 * Get the number of slaves present.
 */
uint8_t get_slave_count (void)
{
  uint8_t cnt = 0;

  for (cnt = 0;
       cnt < MAX_CUBES && g_state.slave_sequence[cnt] != 0;
       cnt++);

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
  memcpy (g_state.word, word, sizeof (*word) * MAX_CUBES);
  memcpy (g_state.patterns, patterns, sizeof (*patterns) * MAX_CUBES);
}

static void master_line_up(void)
{
  // TODO
}

static void master_line_down(void)
{
  // TODO
}

static uint8_t master_read_line(void)
{
  //TODO
  return 0;
}

static void master_query_complete(uint8_t *buf)
{
  buf++;
}

static void init_state (void)
{
  memset (&g_state, 0, sizeof (g_state));
}

static void init_comm (void)
{
  // TODO: init comm pins
  j3p_master_init (&j3p_master_ctx_instance,
                   master_line_up, master_line_down,
                   master_read_line,
                   sizeof(struct master_to_slave_data),
                   sizeof(struct slave_to_master_data),
                   (uint8_t *) &g_master_buf,
                   master_query_complete);
}

int
main (void)
{
  init_state ();
  init_comm ();
}
