#ifndef COMMON_CONFIG_H
#define COMMON_CONFIG_H

#define MAX_CUBES 6
#define MASTER_CLK_FREQ 10000UL // hz

/* Frequency at which we want to query our neighbour */
#define SLAVE_POLL_MS 50

enum anim_pattern {
  ANIM_PATTERN_TODO,
};

struct slave_seq {
  uint8_t ids[MAX_CUBES];
};

struct m2s_data {
  uint8_t rank;

  struct anim_word {
    uint8_t text[MAX_CUBES];
    enum anim_pattern patterns[MAX_CUBES];
  } anim_word;
};

struct s2m_data {
  struct slave_seq slave_seq;
};

union comm_buf {
  struct m2s_data m2s;
  struct s2m_data s2m;
};

#endif /* COMMON_CONFIG_H */

