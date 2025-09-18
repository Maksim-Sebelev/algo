#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <memory.h>
#include <assert.h>
// #include "stack/stack.hpp"

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct stack
{
    size_t elem_size;
    size_t pointer;
    size_t capacity;
    void* data;
};

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

stack* stack_new        (size_t elem_size);
stack* stack_delete     (stack* st);
int    stack_push       (stack* st, void const* elem);
int    stack_pop        (stack* st, void* elem);
int    stack_top        (stack* st, void* elem);
int    stack_empty      (stack const* st);
void   stack_print      (stack const* st, void (*pf) (void const* st));

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static const size_t MinCapacity = 1<<10;
static const size_t MaxCapacity = ~((size_t) 0);

static const unsigned int CapPushReallocCoef = 2;
static const unsigned int CapPopReallocCoef  = 4;

static size_t GetNewPushCapacity  (const stack* st);
static size_t GetNewPopCapacity   (const stack* st);

static int CtorCalloc   (stack* st);
static int PushRealloc  (stack* st);
static int PopRealloc   (stack* st);


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define GOOD_RETURN 0
#define BAD_RETURN  1

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

stack* stack_new(size_t elem_size)
{
    stack* st = (stack*) calloc(1, sizeof(stack));
    if (!st)
        return nullptr;

    st->pointer = 0;
    st->elem_size = elem_size;
    st->capacity = MinCapacity;

    int data_calloc_result = CtorCalloc(st);
    if (data_calloc_result == BAD_RETURN)
        return nullptr;

    return st;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

stack* stack_delete(stack* st)
{
    assert(st);
    free(st->data);
    st->data     = nullptr;
    st->capacity = 0;
    st->pointer  = 0;
    free(st);
    return nullptr;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int stack_push(stack* st, void const* elem)
{
    assert(st);
    assert(elem);

    size_t newSize = st->pointer + st->elem_size;

    if (newSize > MaxCapacity)
        return BAD_RETURN;
    
    if (newSize <= st->capacity)
    {
        void*  data = (char*) st->data + st->pointer;
        memcpy(data, elem, st->elem_size);

        st->pointer = newSize;

        return GOOD_RETURN;
    }

    while (st->capacity < newSize)
        st->capacity = GetNewPushCapacity(st);

    if (PushRealloc(st) == BAD_RETURN)
        return BAD_RETURN;

    void*  data = (char*) st->data + st->pointer;
    memcpy(data, elem, st->elem_size);
    
    st->pointer = newSize;

    if (st->capacity == MaxCapacity)
        return BAD_RETURN;

    return GOOD_RETURN;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int stack_pop(stack* st, void* elem)
{
    assert(st);
    assert(elem);


    if (st->pointer < st->elem_size)
        return BAD_RETURN;

    void* data = (char*) st->data + st->pointer - st->elem_size;
    memcpy(elem, data, st->elem_size);

    size_t newSize = st->pointer - st->elem_size;
    st->pointer = newSize;

    if (st->pointer * CapPopReallocCoef > st->capacity)
        return GOOD_RETURN;

    st->capacity = GetNewPopCapacity(st);
    PopRealloc(st);

    return GOOD_RETURN;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int stack_top(stack* st, void* elem)
{
    assert(st);
    assert(elem);

    if (st->pointer < st->elem_size)
        return BAD_RETURN;

    void* data = (char*) st->data + st->pointer - st->elem_size;
    memcpy(elem, data, st->elem_size);

    return GOOD_RETURN;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void stack_print(stack const* st, void (*pf) (void const * st))
{
    assert(st);
    assert(pf);

    printf("[");
    if (stack_empty(st))
        goto label; // я люблю свою маму

    pf(st->data);
    for (char* i = (char*) st->data + st->elem_size; i < (char*) st->data + st->pointer; i += st->elem_size)
    {
        printf(", ");
        pf(i);
    }

    label:
    printf("]\n");
    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int stack_empty(stack const* st)
{
    assert(st);

    return (st->pointer == 0);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static size_t GetNewPushCapacity(const stack* st)
{
    assert(st);

    size_t NewCapacity = (st->capacity * CapPushReallocCoef);
    size_t temp = NewCapacity < MaxCapacity ? NewCapacity : MaxCapacity;
    return temp;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static size_t GetNewPopCapacity(const stack* st)
{
    assert(st);

    size_t NewCapacity = (st->capacity / CapPopReallocCoef);
    size_t temp = (NewCapacity > MinCapacity) ? NewCapacity : MinCapacity;
    return temp;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static int PushRealloc(stack* st)
{
    assert(st);

    st->data = realloc(st->data, st->capacity * sizeof(char));
    if (st->data == nullptr)
        return BAD_RETURN;
    return GOOD_RETURN; 
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static int PopRealloc(stack* st)
{
    assert(st);

    st->data = realloc(st->data, st->capacity * sizeof(char));

    if (!st->data)
        return BAD_RETURN;

    return GOOD_RETURN;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static int CtorCalloc(stack* st)
{
    assert(st);

    st->data = calloc(st->capacity, st->elem_size);

    if (!st->data)
        return BAD_RETURN;

    return GOOD_RETURN;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#undef GOOD_RETURN
#undef  BAD_RETURN
