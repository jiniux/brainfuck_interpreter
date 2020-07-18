#include "runtime.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

static int find_cycle_end(bf_instruction_t* instructions, int count) {
    int depth = 0;
    for (size_t i = 0; i < count; i++)
    {
        if (instructions[i].type == BF_INSTR_LOOP_S) 
            depth++;

        if (instructions[i].type == BF_INSTR_LOOP_E) {
            if (depth != 0) {
                depth--; continue;
            }

            return i;
        }
    }

    return -1;
}

u_int16_t bf_routine(bf_vm *vm, bf_instruction_t *instr, u_int16_t index) {
    static void* jump_table[] = { 
        &&LCONTINUE, &&LBF_INSTR_RIGHT, &&LBF_INSTR_LEFT,
        &&LBF_INSTR_ADD, &&LBF_INSTR_SUB,
        &&LBF_INSTR_PUTC, &&LBF_INSTR_GETC,
        &&LBF_INSTR_LOOP_S, &&LBF_INSTR_LOOP_E,
        &&LBF_OPT_LOOP_TO_ZERO
    };

    while (instr->type != BF_INSTR_EMPTY) {
        goto *jump_table[instr->type];

        LBF_INSTR_RIGHT:
            index += 1 + instr->repeat_for;
            goto LCONTINUE;
        
        LBF_INSTR_LEFT:
            index -= 1 + instr->repeat_for;
            goto LCONTINUE;
        
        LBF_INSTR_ADD:
            vm->cells[index] += 1 + instr->repeat_for;
            goto LCONTINUE;

        LBF_INSTR_SUB:
            vm->cells[index] -= 1 + instr->repeat_for;
            goto LCONTINUE;

        LBF_INSTR_PUTC:
            putc(vm->cells[index], stdout);
            goto LCONTINUE;

        LBF_INSTR_GETC:
            vm->cells[index] = getc(stdin);
            goto LCONTINUE;

        LBF_INSTR_LOOP_S:
            if (vm->cells[index] == 0) {
                instr = instr->cicle_end_index;
            }
            
            goto LCONTINUE;

        LBF_INSTR_LOOP_E:
            if (vm->cells[index] != 0) {
                instr = instr->cicle_start_index;
            }
            
            goto LCONTINUE;

        LBF_OPT_LOOP_TO_ZERO:
            vm->cells[index] = 0;
            goto LCONTINUE;

        LCONTINUE:
            instr = &instr[1];
    }

    return 0;
}

u_int64_t current_error_code = 0;

u_int64_t bf_get_error() {
    return current_error_code;
}

/*typedef struct bf_pattern_cell {
    char character;
    u_int16_t pos;
} bf_pattern_cell_t; */

int starts_with(const char *restrict string, const char *restrict prefix)
{
    int len = 0;

    while(*prefix)
    {
        if(*prefix++ != *string++)
            return 0;

        ++len;
    }

    return len;
}

dynarray_t* bf_decode_src(char *src) {
    static const bf_pattern_t bf_patterns[] = {
        { "[+]", false, false, BF_OPT_LOOP_TO_ZERO },
        { "[-]", false, false, BF_OPT_LOOP_TO_ZERO },

        { "i[-]x[-]y[x+i+y-]i[y+i-]", true, false, BF_OPT_LOOP_TO_ZERO },

        { ">", false, true,  BF_INSTR_RIGHT  },
        { "<", false, true,  BF_INSTR_LEFT   },
        { "+", false, true,  BF_INSTR_ADD    },
        { "-", false, true,  BF_INSTR_SUB    },
        { ".", false, false, BF_INSTR_PUTC   },
        { ",", false, false, BF_INSTR_GETC   },
        { "[", false, false, BF_INSTR_LOOP_S },
        { "]", false, false, BF_INSTR_LOOP_E },
    };

    dynarray_t* instructions = dynarray_new(sizeof(bf_instruction_t));

    // Decode pass.
    
    char temp;
    while (temp = *(src++)) 
    for (size_t i = 0; i < sizeof(bf_patterns) / sizeof(bf_pattern_t); i++)
    {
        bf_pattern_t* pattern = &bf_patterns[i];

        int len;
        if (len = starts_with(src - 1, pattern->string)) {
            bf_instruction_t instr = { BF_INSTR_EMPTY, 0, { NULL } };

            u_int64_t repeat_for = pattern->optimized ? 
                    get_symbol_repetition(pattern->string[0], src) : 0;

            src += (len - 1) + (instr.repeat_for = repeat_for);
            instr.type = pattern->type;

            dynarray_append_array(instructions, &instr, 1); break;
        }
    }

    // Optimize cycles pass.
    for (size_t i = 0; i < instructions->count; i++) {
        bf_instruction_t* instr = &dynarray_at(instructions, bf_instruction_t, i);

        if (instr->type == BF_INSTR_LOOP_E && instr->cicle_end_index == NULL) {
            current_error_code = BF_ERR_UNEXPECTED_CLOSED_BRACKET;
            return NULL;
        }

        if (instr->type == BF_INSTR_LOOP_S) {
            int result = find_cycle_end(&instr[1], instructions->count - (i + 1));

            if (result == -1) {
                current_error_code = BF_ERR_BRACKET_NOT_CLOSED;
                return NULL;
            }

            instr[result+1].cicle_start_index = &((bf_instruction_t*)instructions->data)[i];
            instr->cicle_end_index = &((bf_instruction_t*)instructions->data)[i + (result + 1)];
        }
    }
    

    bf_instruction_t empty_instr = { 0 };
    dynarray_append_array(instructions, &empty_instr, 1);

    return instructions;
}