#pragma once

#include <math.h>
#include <stddef.h>
#include <stdint.h>


#define POISON 666
#define DATA_POISON NAN
#define INDEX_POISON SIZE_MAX
#define NULL_INDEX 0

typedef int elem_t;
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
    size_t capacity;

}list_t;

enum list_error_names
{
    ERR_LIST_BAD_POINTER      = -9,
    ERR_LIST_INC_INPUT        = -8,
    ERR_LIST_OUT_MEMORY       = -7,
    ERR_LIST_NULL_POINTER     = -2,
    ERR_LIST_NEGATIVE_COUNT   = -1,
};
