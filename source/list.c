#include <stdio.h>
#include <stdlib.h>

#include "list.h"


// #TODO change array free; add negative number in array next for them
//=========================================================================
int list_ctor(list_t* list, size_t capacity)
{
    CHECK(list !=  NULL, ERR_LIST_NULL_PTR);
    CHECK(capacity > 0, ERR_LIST_BAD_SIZE);

    list->data = (elem_t*) calloc(capacity + 2, sizeof(elem_t));
    list->next = (listIndex_t*) calloc(capacity + 2, sizeof(listIndex_t));
    list->prev = (listIndex_t*) calloc(capacity + 2, sizeof(listIndex_t));

    CHECK(list->data !=  NULL, ERR_LIST_NULL_PTR);
    CHECK(list->next !=  NULL, ERR_LIST_NULL_PTR);
    CHECK(list->prev !=  NULL, ERR_LIST_NULL_PTR);

    list->data[NULL_INDEX] = DATA_POISON;
    list->next[NULL_INDEX] = INDEX_POISON;
    list->prev[NULL_INDEX] = INDEX_POISON;
    list_init_data(list);

    list->size = 0;
    list->capacity = capacity;

    list->head = START_INDEX;
    list->tail = START_INDEX;
    list->free = START_INDEX;

    return 0;
}

//=========================================================================

int list_init_data(list_t* list)
{
    CHECK(list !=  NULL, ERR_LIST_NULL_PTR);

    for(int idx = START_INDEX; idx < list->capacity; ++idx)
    {
        list->data[idx] = DATA_POISON;
        list->next[idx] = FREE_INDEX;
        list->prev[idx] = FREE_INDEX;
    }

    return 0;
}

//=========================================================================

int list_dtor(list_t* list)
{
    CHECK(list !=  NULL, ERR_LIST_NULL_PTR);

    free(list->data);
    free(list->next);
    free(list->prev);

    list->data = NULL;
    list->next = NULL;
    list->prev = NULL;

    list->capacity = SIZE_MAX;
    list->free = NULL_INDEX;
    
    return 0;
}

//=========================================================================

int push_back(list_t* list, elem_t value)
{
    insert_after(list, list->tail, value);
    ++list->tail;

    return 0;
}

//=========================================================================

int pop_back(list_t* list)
{
    delete_elem(list, list->tail);
    --list->tail;

    return 0;
}

//=========================================================================

int insert_after(list_t* list, listIndex_t lognum, elem_t value)
{
    CHECK(list !=  NULL, ERR_LIST_NULL_PTR);
    CHECK(lognum > 0, ERR_LIST_BAD_POSITION);

    if (list->size + 1 == list->capacity)
    {
        list_resize(list);
    }
    ++list->size;

    listIndex_t position = get_physical_number(list, lognum);
    CHECK((position >= list->head) && (position <= list->tail), ERR_LIST_BAD_POSITION);

    list->data[list->free] = value;
    list->prev[list->free] = position;              // position -> free
    list->next[list->free] = list->next[position];  // free -> next[position]

    list->next[position] = list->free;
    list->prev[list->next[list->free]] = list->free;

    list->free = -list->next[list->free];          // next[free]; free elems in array next are negative

    return 0;
}

//=========================================================================

int insert_before(list_t* list, listIndex_t lognum, elem_t value)
{
    CHECK(list !=  NULL, ERR_LIST_NULL_PTR);
    CHECK(lognum > 0, ERR_LIST_BAD_POSITION);

    if (list->size + 1 == list->capacity)
    {
        list_resize(list);
    }
    ++list->size;

    listIndex_t position = get_physical_number(list, lognum);
    CHECK((position >= list->head) && (position <= list->tail), ERR_LIST_BAD_POSITION);

    list->data[list->free] = value;
    list->next[list->free] = position;
    list->prev[list->free] = list->prev[position];

    list->prev[position] = list->free;
    list->next[list->prev[list->free]] = list->free;

    list->free = -list->next[list->free];

    return 0;   
}

//=========================================================================

int delete_elem(list_t* list, listIndex_t lognum)
{
    CHECK(list !=  NULL, ERR_LIST_NULL_PTR);
    CHECK(lognum > 0, ERR_LIST_BAD_POSITION);

    CHECK(list->size > 0, ERR_LIST_UNDERFLOW);
    --list->size;

    listIndex_t position = get_physical_number(list, lognum);
    CHECK((position >= list->head) && (position <= list->tail), ERR_LIST_BAD_POSITION);

    list->prev[list->next[position]] = list->prev[position];
    list->next[list->prev[position]] = list->next[position];

    list->next[position] = FREE_INDEX;
    list->prev[position] = FREE_INDEX;
    
    return 0; 
}

//=========================================================================

int get_physical_number(list_t* list, listIndex_t lognum)
{
    CHECK(list !=  NULL, ERR_LIST_NULL_PTR);
    CHECK(lognum > 0, ERR_LIST_BAD_POSITION);

    int physnum = START_INDEX;
    for(int idx = START_INDEX; idx < lognum; ++idx)
    {
        physnum = list->next[physnum];
    }

    return physnum;
}

//=========================================================================

int list_resize(list_t* list)
{
    CHECK(list !=  NULL, ERR_LIST_NULL_PTR);

    list->capacity *= 2;
    CHECK(list->capacity != SIZE_MAX, ERR_LIST_OVERFLOW);

    elem_t* check_data_ptr = (elem_t*) realloc (list->data, list->capacity * sizeof(elem_t));
    listIndex_t* check_next_ptr = (listIndex_t*) realloc (list->next, list->capacity * sizeof(listIndex_t));
    listIndex_t* check_prev_ptr = (listIndex_t*) realloc (list->prev, list->capacity * sizeof(listIndex_t));

    CHECK(check_data_ptr !=  NULL, ERR_LIST_OUT_MEMORY);
    CHECK(check_next_ptr !=  NULL, ERR_LIST_OUT_MEMORY);
    CHECK(check_prev_ptr !=  NULL, ERR_LIST_OUT_MEMORY);

    list->data = check_data_ptr;
    list->next = check_next_ptr;
    list->prev = check_prev_ptr;

    for(long long i = list->capacity / 2; i < list->capacity; i++)
    {
        list->data[i] = DATA_POISON;
        list->next[i] = FREE_INDEX;
        list->prev[i] = FREE_INDEX;
    }

    return 0;
}

//=========================================================================

int list_dump(list_t* list)
{
    CHECK(list !=  NULL, ERR_LIST_NULL_PTR);

    printf("capacity = %zu\n", list->capacity);

    printf("indx: ");
    for(int idx = 0; idx < list->capacity + 1; ++idx)
    {
        printf("%5d ", idx);
    }
    printf("\n");

    printf("data: ");
    for(int idx = 0; idx < list->capacity + 1; ++idx)
    {
        printf("%5.2lf ", list->data[idx]);
    }
    printf("\n");

    printf("next: ");
    for(int idx = 0; idx < list->capacity + 1; ++idx)
    {
        printf("%5zu ", list->next[idx]);
    }
    printf("\n");

    printf("prev: ");
    for(int idx = 0; idx < list->capacity + 1; ++idx)
    {
        printf("%5zu ", list->prev[idx]);
    }
    printf("\n");

    return 0;
}

//=========================================================================
