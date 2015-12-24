#ifndef COMMON_CONFIG_H
#define COMMON_CONFIG_H

#define MAX_CUBES 6

struct master_to_slave_data {
  uint8_t rank;
  uint8_t word[MAX_CUBES];
};

struct slave_to_master_data {
  uint8_t cubes[MAX_CUBES];
};

typedef union {
  struct master_to_slave_data m2s;
  struct slave_to_master_data s2m;
} comm_buf;


enum anim_pattern {
  ANIM_PATTERN_TODO,
};

#endif /* COMMON_CONFIG_H */

