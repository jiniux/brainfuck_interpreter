#ifndef DYNARRAY_H
#define DYNARRAY_H

#define DYNARRAY_INITIAL_CAP 4096

typedef struct dynarray {
    unsigned int count;
    unsigned int size;
    unsigned int element_size;

    void* data;
} dynarray_t;

dynarray_t* dynarray_new(int element_size);

#define dynarray_at(array, type, i) \
    ((type*)array->data)[i]

void dynarray_free(dynarray_t*);
void dynarray_realloc(dynarray_t*);

int dynarray_append_array(dynarray_t*, void*, int);

#endif