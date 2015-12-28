#ifndef COMMON_CONFIG_H
#define COMMON_CONFIG_H

#define MAX_CUBES 6
#define MASTER_CLK_FREQ 10000 // hz

/* Frequency at which we want to query our neighbour */
#define SLAVE_POLL_FREQ 20 // hz
#define SLAVE_QUERY_TIMER_MAX (MASTER_CLK_FREQ / SLAVE_POLL_FREQ)

enum anim_pattern {
  ANIM_PATTERN_TODO,
};

struct m2s_data {
  uint8_t rank;
  uint8_t word[MAX_CUBES];
  enum anim_pattern patterns[MAX_CUBES];
};

struct s2m_data {
  uint8_t cubes[MAX_CUBES];
};

typedef union {
  struct m2s_data m2s;
  struct s2m_data s2m;
} comm_buf;

#endif /* COMMON_CONFIG_H */

