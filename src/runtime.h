#ifndef BF_RUNTIME_H
#define BF_RUNTIME_H

#include <sys/types.h>
#include <stdbool.h>

#include "dynarray.h"

typedef struct {
    u_int8_t    cells[65535];
} bf_vm;

#define BF_INSTR_EMPTY     0x0
#define BF_INSTR_RIGHT     0x1
#define BF_INSTR_LEFT      0x2
#define BF_INSTR_ADD       0x3
#define BF_INSTR_SUB       0x4
#define BF_INSTR_PUTC      0x5
#define BF_INSTR_GETC      0x6
#define BF_INSTR_LOOP_S    0x7
#define BF_INSTR_LOOP_E    0x8

#define BF_OPT_LOOP_TO_ZERO 0x9
//#define BF_OPT_MOVE_CELL    0xA

// Gotta move all the declaration somewhere else.

typedef struct bf_instruction
{
    u_int8_t type;
    u_int64_t repeat_for;

    union
    {
        struct bf_instruction* cicle_start_index;
        struct bf_instruction* cicle_end_index;
    };
    
} bf_instruction_t;

typedef struct bf_pattern {
    const char* string;
    bool special;
    bool optimized;
    u_int8_t type;
} bf_pattern_t;

typedef struct routine_result {
    int index;
    int current_cell;
} routine_result_t;

dynarray_t* bf_decode_src(char *src);
u_int16_t bf_routine(bf_vm* vm, bf_instruction_t* instr, u_int16_t offset);
u_int64_t bf_get_error();

#define BF_ERR_UNEXPECTED_CLOSED_BRACKET    10
#define BF_ERR_BRACKET_NOT_CLOSED           11

#endif