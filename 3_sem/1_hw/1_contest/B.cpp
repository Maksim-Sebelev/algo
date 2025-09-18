#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <memory.h>
#include <assert.h>
// #include "vector/vector.hpp"

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct vector
{
    size_t elem_size;
    size_t pointer;
    size_t capacity;
    void* data;
};

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

vector* vector_new        (size_t elem_size);
vector* vector_delete     (vector* v);
int     vector_push       (vector* v, void const* elem);
int     vector_pop        (vector* v, void* elem);
int     vector_top        (vector* v, void* elem);
int     vector_empty      (vector const* v);
void    vector_print      (vector const* v, void (*pf) (void const* v));

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static const size_t MinCapacity = 1<<10;
static const size_t MaxCapacity = ~((size_t) 0);

static const unsigned int CapPushReallocCoef = 2;
static const unsigned int CapPopReallocCoef  = 4;

static size_t GetNewPushCapacity  (const vector* v);
static size_t GetNewPopCapacity   (const vector* v);

static int CtorCalloc   (vector* v);
static int PushRealloc  (vector* v);
static int PopRealloc   (vector* v);


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define GOOD_RETURN 0
#define BAD_RETURN  1

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

vector* vector_new(size_t elems, size_t elem_size)
{
    assert(elem_size);

    vector* v = (vector*) calloc(1, sizeof(vector));
    if (!v)
        return nullptr;

    v->pointer = 0;
    v->elem_size = elem_size;
    v->capacity = elems;

    int data_calloc_result = CtorCalloc(v);
    if (data_calloc_result == BAD_RETURN)
        return nullptr;

    return v;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

vector* vector_delete(vector* v)
{
    assert(v);
    free(v->data);
    v->data     = nullptr;
    v->capacity = 0;
    v->pointer  = 0;
    free(v);
    return nullptr;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int vector_push(vector* v, void const* elem)
{
    assert(v);
    assert(elem);

    size_t newSize = v->pointer + v->elem_size;

    if (newSize > MaxCapacity)
        return BAD_RETURN;
    
    if (newSize <= v->capacity)
    {
        void*  data = (char*) v->data + v->pointer;
        memcpy(data, elem, v->elem_size);

        v->pointer = newSize;

        return GOOD_RETURN;
    }

    while (v->capacity < newSize)
        v->capacity = GetNewPushCapacity(v);

    if (PushRealloc(v) == BAD_RETURN)
        return BAD_RETURN;

    void*  data = (char*) v->data + v->pointer;
    memcpy(data, elem, v->elem_size);
    
    v->pointer = newSize;

    if (v->capacity == MaxCapacity)
        return BAD_RETURN;

    return GOOD_RETURN;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int vector_pop(vector* v, void* elem)
{
    assert(v);
    assert(elem);


    if (v->pointer < v->elem_size)
        return BAD_RETURN;

    void* data = (char*) v->data + v->pointer - v->elem_size;
    memcpy(elem, data, v->elem_size);

    size_t newSize = v->pointer - v->elem_size;
    v->pointer = newSize;

    if (v->pointer * CapPopReallocCoef > v->capacity)
        return GOOD_RETURN;

    v->capacity = GetNewPopCapacity(v);

    if (!PopRealloc(v))
        return BAD_RETURN;

    return GOOD_RETURN;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int vector_get(vector const* v, size_t index, void* elem)
{
    assert(v);
    assert(elem);

    size_t pointer = index * v->elem_size;

    if (pointer > v->pointer)
        return BAD_RETURN;
    
    char* data = (char*)v->data + pointer;

    if (!memcpy(elem, data, v->elem_size))
        return BAD_RETURN;

    return GOOD_RETURN;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int vector_set(vector* v, size_t index, void const* elem)
{
    assert(v);
    assert(elem);
    
    size_t pointer = index * v->elem_size; 
    
    if (pointer >= v->pointer)
    {
        if (pointer > v->pointer + v->elem_size)
            return BAD_RETURN;

        if (vector_push(v, elem) == BAD_RETURN)
            return BAD_RETURN;

        return GOOD_RETURN;
    }

    char* data = (char*)v->data + pointer;
    memcpy(data, elem, v->elem_size); 

    return GOOD_RETURN;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int vector_resize(vector* v, size_t new_size)
{
    assert(v);

    if (new_size < v->pointer / v->elem_size)
        return BAD_RETURN;
    
    v->capacity = new_size;

    size_t real_size = (new_size > 0) ? new_size : 1;
    v->data = realloc(v->data, real_size * v->elem_size);

    if (!v->data)
        return BAD_RETURN;
    
    return GOOD_RETURN;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void vector_print(vector const* v, void (*pf) (void const * v))
{
    assert(v);

    printf("[");
    if (vector_empty(v))
        goto label; // я люблю свою маму

    pf(v->data);
    for (char* i = (char*) v->data + v->elem_size; i < (char*) v->data + v->pointer; i += v->elem_size)
    {
        printf(", ");
        pf(i);
    }

    label:
    printf("]\n");
    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int vector_empty(vector const* v)
{
    assert(v);

    return (v->pointer == 0);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

size_t vector_size(vector const* v)
{
    assert(v);
    return v->pointer / v->elem_size;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static size_t GetNewPushCapacity(const vector* v)
{
    assert(v);

    size_t NewCapacity = (v->capacity * CapPushReallocCoef);
    size_t temp = NewCapacity < MaxCapacity ? NewCapacity : MaxCapacity;
    return temp;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static size_t GetNewPopCapacity(const vector* v)
{
    assert(v);

    size_t NewCapacity = (v->capacity / CapPopReallocCoef);
    size_t temp = (NewCapacity > MinCapacity) ? NewCapacity : MinCapacity;
    return temp;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static int PushRealloc(vector* v)
{
    assert(v);

    v->data = realloc(v->data, v->capacity * sizeof(char));
    if (v->data == nullptr)
        return BAD_RETURN;
    return GOOD_RETURN; 
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static int PopRealloc(vector* v)
{
    assert(v);

    v->data = realloc(v->data, v->capacity * sizeof(char));

    if (!v->data)
        return BAD_RETURN;

    return GOOD_RETURN;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static int CtorCalloc(vector* v)
{
    assert(v);

    size_t capacity = (v->capacity > 0) ? v->capacity : 1;

    v->data = calloc(capacity, v->elem_size);

    if (!v->data)
        return BAD_RETURN;

    return GOOD_RETURN;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#undef GOOD_RETURN
#undef  BAD_RETURN
