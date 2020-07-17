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
            if (vm->cells[index] != 0) {
                index = bf_routine(vm, &instr[1], index);
            }

            instr = instr->cicle_end_index;
            goto LCONTINUE;

        LBF_INSTR_LOOP_E:
            if (vm->cells[index] != 0) {
                instr = instr->cicle_start_index;
            } else {
                return index;
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

dynarray_t* bf_decode_src(char *src)
{
    static const bf_symbol_t bf_symbols[] = {
        { "[+]", false, BF_OPT_LOOP_TO_ZERO },
        { "[-]", false, BF_OPT_LOOP_TO_ZERO },

        { ">", true,  BF_INSTR_RIGHT  },
        { "<", true,  BF_INSTR_LEFT   },
        { "+", true,  BF_INSTR_ADD    },
        { "-", true,  BF_INSTR_SUB    },
        { ".", false, BF_INSTR_PUTC   },
        { ",", false, BF_INSTR_GETC   },
        { "[", false, BF_INSTR_LOOP_S },
        { "]", false, BF_INSTR_LOOP_E },
    };

    dynarray_t* instructions = dynarray_new(sizeof(bf_instruction_t));

    // Decode pass.
    
    char temp;
    while (temp = *(src++)) 
    for (size_t i = 0; i < sizeof(bf_symbols) / sizeof(struct bf_symbol); i++)
    {
        const char* string = bf_symbols[i].string; 
        int length = strlen(string);

        if (strncmp(string, src-1, length) == 0) {
            struct bf_symbol symbol = bf_symbols[i];
            bf_instruction_t *instr = calloc(1, sizeof(bf_instruction_t));

            u_int64_t repeat_for = symbol.optimized ? 
                    get_symbol_repetition(symbol.string[0], src) : 0;

            src += (length - 1) + (instr->repeat_for = repeat_for);
            instr->type = symbol.type;

            dynarray_append_array(instructions, instr, 1); break;
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