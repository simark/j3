#ifndef J3P_COMMON_H
#define J3P_COMMON_H

#define J3P_BREAK_NUM_BITS 12
#define J3P_MARK_AFTER_BREAK_NUM_BITS 4

typedef void (*j3p_set_line_op) (void);
typedef uint8_t (*j3p_read_line_op) (void);

#endif /* J3P_COMMON_H */
