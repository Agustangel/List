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
typedef uint_fast16_t listStatus_t;

static int condition__;


typedef struct list_t
{  
    elem_t* data;
    listIndex_t* next;
    listIndex_t* prev;

    listIndex_t free;
    listIndex_t head;
    listIndex_t tail;
    
    size_t size;
    size_t capacity;

    listStatus_t status;
    
}list_t;

enum list_codes
{
    LIST_SUCCESS              = 0,
    ERR_LIST_NULL_PTR         = 1,
    ERR_LIST_BAD_PTR          = 2,
    ERR_LIST_OUT_MEMORY       = 3,
    ERR_LIST_BAD_SIZE         = 4,
    ERR_LIST_BAD_POSITION     = 5,
    ERR_LIST_UNDERFLOW        = 6,
    ERR_LIST_OVERFLOW         = 7,
    ERR_LIST_INC_LIST         = 8
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
int list_init_data(list_t* list);
int list_dtor(list_t* list);
int list_dump(list_t* list);
int push_back(list_t* list, elem_t value);
int pop_back(list_t* list);
int list_resize(list_t* list);
int insert_after(list_t* list, listIndex_t lognum, elem_t value);
int insert_before(list_t* list, listIndex_t lognum, elem_t value);
int get_physical_number(list_t* list, listIndex_t lognum);
int delete_elem(list_t* list, listIndex_t lognum);
int list_veryfi(list_t* list);
