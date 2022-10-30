#include <stdio.h>
#include <stdlib.h>

#include "list.h"


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
    list_init_data(list, START_INDEX);

    list->size = 0;
    list->capacity = capacity;

    list->head = START_INDEX;
    list->tail = START_INDEX;
    list->free = START_INDEX;

    return 0;
}

//=========================================================================

int list_init_data(list_t* list, listIndex_t start)
{
    CHECK(list !=  NULL, ERR_LIST_NULL_PTR);

    for(int idx = start; idx < list->capacity; ++idx)
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
    CHECK(list !=  NULL, ERR_LIST_NULL_PTR);

    if (list->size == list->capacity)
    {
        list_resize(list);
    }
    ++list->size;
    ++list->tail;
    
    list->data[list->tail] = value;
    list->next[list->tail] = FREE_INDEX;

    for(listIndex_t idx = list->head; idx < list->tail; ++idx)
    {
        if((list->next[idx] == FREE_INDEX) && (list->prev[idx] != FREE_INDEX))
        {
            list->next[idx] = list->tail; // physical number of tail
            list->prev[list->tail] = idx; 
        }
    }

    return 0;
}

//=========================================================================

int insert_after(list_t* list, listIndex_t position, elem_t value)
{
    CHECK(list !=  NULL, ERR_LIST_NULL_PTR);
    CHECK((position >= list->head) && (position <= list->tail), ERR_LIST_BAD_POSITION);
}

//=========================================================================

int pop_back(list_t* list)
{
    CHECK(list !=  NULL, ERR_LIST_NULL_PTR);

    list->data[list->tail] = DATA_POISON;
    list->next[list->tail] = FREE_INDEX;
    list->prev[list->tail] = FREE_INDEX;

    --list->tail;
    --list->size;
    CHECK(list->size > 0, ERR_LIST_UNDERFLOW);

    return 0;
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
