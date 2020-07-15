#include "dynarray.h"

#include <stdlib.h>

void dynarray_free(dynarray_t* array) {
    free(array->data);
    free(array);
}

dynarray_t* dynarray_new(int element_size)
{
    dynarray_t* array = (dynarray_t*)calloc(sizeof(dynarray_t), 1);
    array->element_size = element_size;
    array->data = malloc(DYNARRAY_INITIAL_CAP);

    return array;
}

#define dynarray_cap_index(array, size) \
    (size / DYNARRAY_INITIAL_CAP + 1) 
    
void dynarray_realloc(dynarray_t* array)
{
    int n_size = DYNARRAY_INITIAL_CAP * dynarray_cap_index(array, array->size);
    array->data = realloc(array->data, n_size);
}

int dynarray_append_array(dynarray_t* array, void* data, int count)
{
    if (data) {
        for (int i = 0; i < count; i++)
        for (int j = 0; j < array->element_size; j++) {
            if (array->size != 0 && array->size % DYNARRAY_INITIAL_CAP == 0)
                dynarray_realloc(array);
                
            char value = *(char*)(data + i * array->element_size + j);
            *(char*)(array->data + array->element_size * (array->count + i) + j) = value;

            array->size++;
        }

        array->count += count;

        return 1;
    }

    return 0;
}