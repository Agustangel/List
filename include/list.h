#pragma once

#include <math.h>
#include <stddef.h>
#include <stdint.h>


#define FREE_INDEX -1
#define INDEX_POISON -3
#define START_INDEX 1
#define DATA_POISON NAN
#define NULL_INDEX 0

typedef double elem_t;
typedef size_t listIndex_t;

static int condition__;


typedef struct list_t
{  
    elem_t* data;
    listIndex_t* next;
    listIndex_t* prev;

    listIndex_t free;
    listIndex_t head;
    listIndex_t tail;
    
    size_t capacity;

}list_t;

enum list_error_names
{
    ERR_LIST_NULL_PTR         = -9,
    ERR_LIST_INC_INPUT        = -8,
    ERR_LIST_OUT_MEMORY       = -7,
    ERR_LIST_BAD_SIZE         = -6,
    ERR_LIST_UNDERFLOW        = -5,
    ERR_LIST_OVERFLOW         = -4,
    ERR_LIST_NEGATIVE_COUNT   = -1,
};

#define CHECK(condition, retval)                          \
                                                          \
    condition__ = condition;                              \
    if (!condition__)                                     \
    {                                                     \
        ASSERT(condition__, #condition, retval);          \
        return retval;                                    \
    };                                                    \

#ifdef DEBUG
    #define ASSERT(condition, message, retval)            \
                                                          \
    if (!condition)                                       \
    {                                                     \
        fprintf(stderr, "*** ASSERTION ERROR ***\n");     \
        fprintf(stderr, "    Failed %s\n", message);      \
        fprintf(stderr, "    Location:\n");               \
        fprintf(stderr, "    File ----- %s\n", __FILE__); \
        fprintf(stderr, "    Function - %s\n", __func__); \
        fprintf(stderr, "    Line ----- %d\n", __LINE__); \
        exit(retval);                                     \
    }
#else
    #define ASSERT(condition, message, retval){}
#endif

int list_ctor(list_t* list, size_t capacity);
int list_init_data(list_t* list, listIndex_t start);
int list_dtor(list_t* list);
int list_dump(list_t* list);
int push_back(list_t* list, elem_t value);
int pop_back(list_t* list);
int list_resize(list_t* list);
