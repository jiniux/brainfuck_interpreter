#include "runtime.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

extern const struct bf_symbol bf_symbols[] = {
    { '>', true,  BF_INSTR_RIGHT  },
    { '<', true,  BF_INSTR_LEFT   },
    { '+', true,  BF_INSTR_ADD    },
    { '-', true,  BF_INSTR_SUB    },
    { '.', false, BF_INSTR_PUTC   },
    { ',', false, BF_INSTR_GETC   },
    { '[', false, BF_INSTR_LOOP_S },
    { ']', false, BF_INSTR_LOOP_E }, 
};

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

void bf_exec(bf_vm *vm, bf_instruction_t *instr) {
    static void* jump_table[] = { 
        &&LCONTINUE, &&LBF_INSTR_RIGHT, &&LBF_INSTR_LEFT,
        &&LBF_INSTR_ADD, &&LBF_INSTR_SUB,
        &&LBF_INSTR_PUTC, &&LBF_INSTR_GETC,
        &&LBF_INSTR_LOOP_S, &&LBF_INSTR_LOOP_E
    };

    while (instr->type != BF_INSTR_EMPTY) {
        goto *jump_table[instr->type];

        LBF_INSTR_RIGHT:
            vm->cell_index += 1 + instr->repeat_for;
            goto LCONTINUE;
        
        LBF_INSTR_LEFT:
            vm->cell_index -= 1 + instr->repeat_for;
            goto LCONTINUE;
        
        LBF_INSTR_ADD:
            vm->cells[vm->cell_index] += 1 + instr->repeat_for;
            goto LCONTINUE;

        LBF_INSTR_SUB:
            vm->cells[vm->cell_index] -= 1 + instr->repeat_for;
            goto LCONTINUE;

        LBF_INSTR_PUTC:
            putc(vm->cells[vm->cell_index], stdout);
            goto LCONTINUE;

        LBF_INSTR_GETC:
            vm->cells[vm->cell_index] = getc(stdin);
            goto LCONTINUE;

        LBF_INSTR_LOOP_S:
            if (vm->cells[vm->cell_index] != 0) {
                bf_exec(vm, &instr[1]);
            }

            instr = instr->cicle_end_index;
            goto LCONTINUE;

        LBF_INSTR_LOOP_E:
            if (vm->cells[vm->cell_index] != 0) {
                instr = instr->cicle_start_index;
            } else {
                return;
            }
            goto LCONTINUE;

        LCONTINUE:
            instr = &instr[1];
    }
}

u_int64_t current_error_code = 0;

u_int64_t bf_get_error() {
    return current_error_code;
}

dynarray_t* bf_decode_src(char *src)
{
    dynarray_t* instructions = dynarray_new(sizeof(bf_instruction_t));

    // Decode pass.
    
    char temp;
    while (temp = *(src++)) 
    for (size_t i = 0; i < sizeof(bf_symbols) / sizeof(struct bf_symbol); i++)
    {
        if (temp == bf_symbols[i].character) {
            struct bf_symbol symbol = bf_symbols[i];
            bf_instruction_t *instr = calloc(1, sizeof(bf_instruction_t));

            u_int64_t repeat_for = symbol.optimized ? 
                    get_symbol_repetition(symbol.character, src) : 0;      
                    
            src += instr->repeat_for = repeat_for;
            instr->type = symbol.type;

            dynarray_append_array(instructions, instr, 1); break;
        }
    }


    // Optimize cycles pass.
    
    for (size_t i = 0; i < instructions->count; i++) {
        bf_instruction_t* instr = &dynarray_at(instructions, bf_instruction_t, i);

        if (instr->type == BF_INSTR_LOOP_E && instr->cicle_end_index == NULL) {
            return NULL;
        }

        if (instr->type == BF_INSTR_LOOP_S) {
            int result = find_cycle_end(&instr[1], instructions->count - (i + 1));

            if (result == -1) {
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