#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dynarray.h"
#include "utils.h"
#include "runtime.h"

int main(int argc, char* argv[]) {
    bf_vm vm; memset(&vm, 0x0, sizeof(bf_vm));

    if (argc < 2) {
        fprintf(stderr, "Missing arguments. Usage: %s <src_file.bf>\n", argv[0]);

        return 1;
    }

    FILE* file;
    if (!(file = fopen(argv[1], "r"))) {
        fputs("There was an error while opening the file.", stderr);

        return 2;
    }

    int size = get_file_size(file);
    
    char* src = malloc(size);
    fread(src, 1, size - 1, file);
    src[size - 1] = '\0';

    dynarray_t* instructions = bf_decode_src(src);
    if (!instructions) {
        switch (bf_get_error())
        {
        case BF_ERR_BRACKET_NOT_CLOSED:
            fputs("ERROR: Bracket not closed.", stderr);
            break;
        
        case BF_ERR_UNEXPECTED_CLOSED_BRACKET:
            fputs("ERROR: Unexpected closed bracket.", stderr);
            break;
        
        default:
            break;
        }
        return 3;
    }

    bf_exec(&vm, (bf_instruction_t*)instructions->data);

    dynarray_free(instructions);
    return 0;
}