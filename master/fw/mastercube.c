#include <j3p.h>
#include <common-config.h>

static struct j3p_master_ctx j3p_master_ctx_instance;

static comm_buf g_master_buf;

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

int
main (void)
{
  j3p_master_init (&j3p_master_ctx_instance,
                   master_line_up, master_line_down,
                   master_read_line,
                   sizeof(struct master_to_slave_data),
                   sizeof(struct slave_to_master_data),
                   (uint8_t *) &g_master_buf,
                   master_query_complete);
}
