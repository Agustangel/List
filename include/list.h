#pragma once

#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>


//=========================================================================
//! enumeration of constants
enum
{
    NULL_INDEX   = 0,
    START_INDEX  = 1,
    FREE_INDEX   = -1,
    INDEX_POISON = -666,
    ERROR_NUMBER = 8
};

//! constant for indicates poison data in list
#define DATA_POISON NAN

//=========================================================================

typedef double elem_t;
typedef int listIndex_t;
typedef uint_fast16_t listStatus_t;

//!  A structure of list
typedef struct list_t
{  
    elem_t* data;           /**< array of the data */
    listIndex_t* next;      /**< array of the next indices */
    listIndex_t* prev;      /**< array of the prev indices */

    listIndex_t free;       /**< the first free  index */
    listIndex_t end;        /**< the end logical index */
    listIndex_t head;       /**< the first physical index */
    listIndex_t tail;       /**< the end   physical index */
    
    size_t size;            /**< size of list */
    size_t capacity;        /**< capacity of list */

    listStatus_t status;    /**< contains information about errors */ 
    bool linflag;           /**< linearization flag */
    
}list_t;

//=========================================================================
/*! enumeration of list errors and list success with values. */
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

//=========================================================================

//! Сonstructor function
/*! \param list - pointer to list on stack.
    \param capacity - initial capacity of list.
 *  \returns result - list_codes. */
int list_ctor(list_t* list, size_t capacity);

//! Function that initializes the list data
/*! \param list - pointer to list on stack.
 *  \returns result - list_codes. */
int list_init_data(list_t* list);

//! Destructor function
/*! \param list - pointer to list on stack.
 *  \returns result - list_codes. */
int list_dtor(list_t* list);

//! Function that prints all information about list
/*! \param list - pointer to list on stack.
 *  \returns result - list_codes. */
int list_dump(list_t* list);

//! Function that creates new node at the end of list
/*! \param list - pointer to list on stack.
    \param value - value of element at the end of list.
 *  \returns result - list_codes. */
int push_back(list_t* list, elem_t value);

//! Function that deletes node at the end of list
/*! \param list - pointer to list on stack.
 *  \returns result - list_codes. */
int pop_back(list_t* list);

//! Function that changes the size of list
/*! \param list - pointer to list on stack.
 *  \returns result - list_codes. */
int list_resize(list_t* list);

//! Function that inserts element after specified position
/*! \param list - pointer to list on stack.
    \param lognum - logical number of element.
    \param value - value of the inserted element.
 *  \returns result - list_codes. */
int insert_after(list_t* list, listIndex_t lognum, elem_t value);

//! Function that inserts element before specified position
/*! \param list - pointer to list on stack.
    \param lognum - logical number of element.
    \param value - value of the inserted element.
 *  \returns result - list_codes. */
int insert_before(list_t* list, listIndex_t lognum, elem_t value);

//! Function that gets physical number from logical
/*! \param list - pointer to list on stack.
    \param lognum - logical number of element.
 *  \returns result - list_codes. */
int get_physical_number(list_t* list, listIndex_t lognum);

//! Function that deletes element specified position
/*! \param list - pointer to list on stack.
    \param lognum - logical number of element.
 *  \returns result - list_codes. */
int delete_elem(list_t* list, listIndex_t lognum);

//! Function that looks for all errors of list
/*! \param list - pointer to list on stack.
 *  \returns result - list_codes. */
int list_veryfi(list_t* list);

//! Function that shows all information about list graphically
/*! \param list - pointer to list on stack.
 *  \returns result - list_codes. */
int graph_dump(list_t* list);

//! Function that linearizes a list
/*! \param list - pointer to list on stack.
 *  \returns result - list_codes. */
int list_linearize(list_t* list);
