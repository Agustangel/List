#pragma once

#include <math.h>
#include <stddef.h>
#include <stdint.h>


/*! enumeration colors and their corresponding ANSI values. */
#define BLACK "\033[0;30m"
#define BLUE "\033[0;34m"
#define GREEN "\033[0;32m"
#define CYAN "\033[0;36m"
#define RED "\033[0;31m"
#define PURPLE "\033[0;35m"
#define BROWN "\033[0;33m"
#define YELLOW "\e[0;33m"
#define RESET "\033[0m"


#define FREE_INDEX -1
#define INDEX_POISON -666
#define START_INDEX 1
#define DATA_POISON NAN
#define NULL_INDEX 0
#define ERROR_NUMBER 8

typedef double elem_t;
typedef int listIndex_t;
typedef uint_fast16_t listStatus_t;

static int condition__;
static int ret_;


typedef struct list_t
{  
    elem_t* data;
    listIndex_t* next;
    listIndex_t* prev;

    listIndex_t free;
    listIndex_t end;
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


//! Macros LIST_ERROR
/*! macros to print the error. */
#define LIST_ERROR(ERROR_NAME)                                                      \
{                                                                                   \
    fprintf(stderr, RED "ERROR:" RESET "line %d. " #ERROR_NAME "\n", __LINE__);     \
};

//! Macros LIST_OK
/*! macros to describe the error. */
#define LIST_OK(list)                                                               \
                                                                                    \
    ret_ = list_veryfi(list);                                                       \
                                                                                    \
    switch (ret_)                                                                   \
    {                                                                               \
    case (1 << ERR_LIST_NULL_PTR):                                                  \
        LIST_ERROR(ERR_LIST_NULL_PTR);                                              \
        break;                                                                      \
                                                                                    \
    case (1 << ERR_LIST_OUT_MEMORY):                                                \
        LIST_ERROR(ERR_LIST_OUT_MEMORY);                                            \
        break;                                                                      \
                                                                                    \
    case (1 << ERR_LIST_UNDERFLOW):                                                 \
        LIST_ERROR(ERR_LIST_UNDERFLOW);                                             \
        break;                                                                      \
                                                                                    \
    case (1 << ERR_LIST_OVERFLOW):                                                  \
        LIST_ERROR(ERR_LIST_OVERFLOW);                                              \
        break;                                                                      \
                                                                                    \
    case (1 << ERR_LIST_BAD_POSITION):                                              \
        LIST_ERROR(ERR_LIST_BAD_POSITION);                                          \
        break;                                                                      \
                                                                                    \
    case (1 << ERR_LIST_BAD_PTR):                                                   \
        LIST_ERROR(ERR_LIST_BAD_PTR);                                               \
        break;                                                                      \
                                                                                    \
    case (1 << ERR_LIST_BAD_PTR):                                                   \
        LIST_ERROR(ERR_LIST_BAD_PTR);                                               \
        break;                                                                      \
                                                                                    \
    case (1 << ERR_LIST_INC_LIST):                                                  \
        LIST_ERROR(ERR_LIST_INC_LIST);                                              \
        break;                                                                      \
    };                                                          


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
