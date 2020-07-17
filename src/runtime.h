#ifndef BF_RUNTIME_H
#define BF_RUNTIME_H

#include <sys/types.h>
#include <stdbool.h>

#include "dynarray.h"

typedef struct {
    u_int8_t    cells[65535];
    u_int16_t   cell_index;
    u_int64_t   program_counter;
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

const struct bf_symbol {
    char character;
    bool optimized;
    u_int8_t type;
} bf_symbols[];

dynarray_t* bf_decode_src(char *src);
void bf_exec(bf_vm* vm, bf_instruction_t* instr);
u_int64_t bf_get_error();

#define BF_ERR_UNEXPECTED_CLOSED_BRACKET    10
#define BF_ERR_BRACKET_NOT_CLOSED           11

#endif