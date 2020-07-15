#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stdbool.h>

#include "dynarray.h"

typedef struct {
    u_int8_t    cells[65535];
    u_int16_t   cell_index;
    u_int64_t   program_counter;
} bf_vm;

#define INSTR_EMPTY     0x0
#define INSTR_RIGHT     0x1
#define INSTR_LEFT      0x2
#define INSTR_ADD       0x3
#define INSTR_SUB       0x4
#define INSTR_PUTC      0x5
#define INSTR_GETC      0x6
#define INSTR_LOOP_S    0x7
#define INSTR_LOOP_E    0x8

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

struct bf_symbol {
    char character;
    bool optimized;
    u_int8_t type;
} bf_symbols[] = {
    { '>', true,  INSTR_RIGHT  },
    { '<', true,  INSTR_LEFT   },
    { '+', true,  INSTR_ADD    },
    { '-', true,  INSTR_SUB    },
    { '.', false, INSTR_PUTC   },
    { ',', false, INSTR_GETC   },
    { '[', false, INSTR_LOOP_S },
    { ']', false, INSTR_LOOP_E }, 
};

u_int64_t get_symbol_repetition(char c, char *s) {
    u_int64_t count = 0;
    while (*s && *(s++) == c) 
        count++;

    return count;
}

int find_cycle_end(bf_instruction_t* instructions, int count) {
    int depth = 0;
    for (size_t i = 0; i < count; i++)
    {
        if (instructions[i].type == INSTR_LOOP_S) 
            depth++;

        if (instructions[i].type == INSTR_LOOP_E) {
            if (depth != 0) {
                depth--; continue;
            }

            return i;
        }
    }

    return -1;
}

void exec(bf_vm* vm, bf_instruction_t* instr) {
    static void* jump_table[] = { 
        &&LCONTINUE, &&LINSTR_RIGHT, &&LINSTR_LEFT,
        &&LINSTR_ADD, &&LINSTR_SUB,
        &&LINSTR_PUTC, &&LINSTR_GETC,
        &&LINSTR_LOOP_S, &&LINSTR_LOOP_E
    };

    while (instr->type != INSTR_EMPTY) {
        goto *jump_table[instr->type];

        LINSTR_RIGHT:
            vm->cell_index += 1 + instr->repeat_for;
            goto LCONTINUE;
        
        LINSTR_LEFT:
            vm->cell_index -= 1 + instr->repeat_for;
            goto LCONTINUE;
        
        LINSTR_ADD:
            vm->cells[vm->cell_index] += 1 + instr->repeat_for;
            goto LCONTINUE;

        LINSTR_SUB:
            vm->cells[vm->cell_index] -= 1 + instr->repeat_for;
            goto LCONTINUE;

        LINSTR_PUTC:
            putc(vm->cells[vm->cell_index], stdout);
            goto LCONTINUE;

        LINSTR_GETC:
            vm->cells[vm->cell_index] = getc(stdin);
            goto LCONTINUE;

        LINSTR_LOOP_S:
            if (vm->cells[vm->cell_index] != 0) {
                exec(vm, &instr[1]);
            }

            instr = instr->cicle_end_index;
            goto LCONTINUE;

        LINSTR_LOOP_E:
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

dynarray_t* decode_src(char *src) {
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

        if (instr->type == INSTR_LOOP_S) {
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

int get_file_size(FILE* file) {
    int current_pos = ftell(file);
    
    fseek(file, 0, SEEK_END);
    int size = ftell(file) + 1;
    fseek(file, current_pos, SEEK_SET);

    return size; 
}

int main(int argc, char* argv[]) {
    bf_vm vm; memset(&vm, 0x0, sizeof(bf_vm));

    if (argc < 2) {
        printf("Usage: %s <src_file.bf>\n", argv[0]);

        return 1;
    }

    FILE* file;
    if (!(file = fopen(argv[1], "r"))) {
        puts("There was an error while opening the file.");

        return 2;
    }

    int size = get_file_size(file);
    
    char* src = malloc(size);
    fread(src, 1, size - 1, file);
    src[size - 1] = '\0';

    dynarray_t* instructions = decode_src(src);
    exec(&vm, (bf_instruction_t*)instructions->data);

    dynarray_free(instructions);
    return 0;
}