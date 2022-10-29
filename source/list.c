#include <stdio.h>
#include <stdlib.h>

#include "list.h"


//=========================================================================
int list_ctor(list_t* list, size_t capacity)
{
    CHECK(list !=  NULL, ERR_LIST_NULL_PTR);
    CHECK(capacity >= 0, ERR_LIST_BAD_SIZE);

    if(capacity == 0)
    {
        list->nodes = NULL;
        list->free = NULL_INDEX;
        list->capacity = 0;
        list->size = 0;
    }

    list->nodes = (node_t*) calloc(capacity + 1, sizeof(node_t));
    CHECK(list !=  NULL, ERR_LIST_NULL_PTR);

    list->nodes[NULL_INDEX].data = DATA_POISON;
    list->capacity = capacity;
    list->size = 0;
    list->free = NULL_INDEX;
    list_init_nodes(list, 1);

    return 0;
}

//=========================================================================

int list_init_nodes(list_t* list, listIndex_t start)
{
    CHECK(list !=  NULL, ERR_LIST_NULL_PTR);

    for(int idx = start; idx < list->capacity; ++idx)
    {
        list->nodes[idx].data = DATA_POISON;
        list->nodes[idx].next = idx + 1;
        list->nodes[idx].prev = INDEX_POISON;
    }
    list->nodes[list->capacity].next = list->free;

    return 0;
}

//=========================================================================

int list_dtor(list_t* list)
{
    CHECK(list !=  NULL, ERR_LIST_NULL_PTR);

    free(list->nodes);
    list->nodes = NULL;
    list->capacity = SIZE_MAX;
    list->free = NULL_INDEX;
    
    return 0;
}

//=========================================================================

int push_back(list_t* list, size_t value)
{
    CHECK(list !=  NULL, ERR_LIST_NULL_PTR);

    if (list->size + 1 == list->capacity)
    {
        list_resize(list);
    }
    ++list->size;

    list->nodes[list->size].data = value;
    list->nodes[list->size].next = list->free;
    list->nodes[list->capacity].prev = list->size - 1;

    return 0;
}

//=========================================================================

int pop_back(list_t* list)
{
    CHECK(list !=  NULL, ERR_LIST_NULL_PTR);

    list->nodes[list->size].data = FREE_POISON;
    list->nodes[list->size].next = FREE_POISON;
    list->nodes[list->size].prev = FREE_POISON;

    --list->size;
    CHECK(list->size > 0, ERR_LIST_UNDERFLOW);

    list->nodes[list->size].next = list->free;
    list->nodes[list->size].prev = list->capacity - 1;

    return 0;
}

//=========================================================================

int list_resize(list_t* list)
{
    CHECK(list !=  NULL, ERR_LIST_NULL_PTR);

    list->capacity *= 2;
    CHECK(list->capacity != SIZE_MAX, ERR_LIST_OVERFLOW);

    //realloc prev/next/data
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
        printf("%5.2lf ", list->nodes[idx].data);
    }
    printf("\n");

    printf("next: ");
    for(int idx = 0; idx < list->capacity + 1; ++idx)
    {
        printf("%5zu ", list->nodes[idx].next);
    }
    printf("\n");

    printf("prev: ");
    for(int idx = 0; idx < list->capacity + 1; ++idx)
    {
        printf("%5zu ", list->nodes[idx].prev);
    }
    printf("\n");

    return 0;
}

//=========================================================================
