#include "utils.h"

u_int64_t get_symbol_repetition(char c, char *s) {
    u_int64_t count = 0;
    while (*s && *(s++) == c) 
        count++;

    return count;
}

int get_file_size(FILE* file)
{
    int current_pos = ftell(file);
    
    fseek(file, 0, SEEK_END);
    int size = ftell(file) + 1;
    fseek(file, current_pos, SEEK_SET);

    return size; 
}