#include "compiler.h"

#include <stdio.h>
#include <stdlib.h>

void bf_jit(bf_vm* vm, bf_instruction_t* c_instruction) {
    printf("JIT compiler is not currently supported for this platform.");
    exit(105);
}