#pragma once

#include <math.h>
#include <stddef.h>
#include <stdint.h>


#define POISON 666
#define DATA_POISON NAN
#define INDEX_POISON SIZE_MAX
#define NULL_INDEX 0

typedef double elem_t;
typedef size_t listIndex_t;


typedef struct node_t
{
    elem_t data;

    listIndex_t next;
    listIndex_t prev;

} node_t;

typedef struct list_t
{  
    node_t* nodes;
    listIndex_t free;
    size_t capacity;

}list_t;

enum list_error_names
{
    ERR_LIST_BAD_PTR          = -9,
    ERR_LIST_INC_INPUT        = -8,
    ERR_LIST_OUT_MEMORY       = -7,
    ERR_LIST_BAD_SIZE         = -6,
    ERR_LIST_NULL_POINTER     = -2,
    ERR_LIST_NEGATIVE_COUNT   = -1,
};

#define CHECK(condition, retval)                          \
                                                          \
    int condition__ = condition;                          \
    if (!condition__)                                     \
    {                                                     \
        ASSERT(condition__, #condition, retval);          \
        return retval;                                    \
    };                                                    \

#ifdef DEBUG
    #define ASSERT(condition, message)                    \
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
    #define ASSERT(condition, message){}
#endif

int list_ctor(list_t* list, size_t capacity);
int list_init_nodes(list_t* list, listIndex_t start);
int list_dtor(list_t* list);
