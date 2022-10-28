#include <stdio.h>
#include <stdlib.h>

#include "list.h"


//=========================================================================
int list_ctor(list_t* list, size_t capacity)
{
    CHECK(list !=  NULL, ERR_LIST_BAD_PTR);
    CHECK(capacity >= 0, ERR_LIST_BAD_SIZE);

    if(capacity == 0)
    {
        list->nodes = NULL;
        list->capacity = 0;
        list->free = NULL_INDEX;
    }

    list->nodes = (node_t*) calloc(capacity + 1, sizeof(node_t));
    CHECK(list !=  NULL, ERR_LIST_BAD_PTR);

    list->nodes[NULL_INDEX].data = DATA_POISON;
    list->capacity = capacity;
    list->free = NULL_INDEX;
    list_init_nodes(list, 1);

    return 0;
}

//=========================================================================

int list_init_nodes(list_t* list, listIndex_t start)
{
    CHECK(list !=  NULL, ERR_LIST_BAD_PTR);

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
    CHECK(list !=  NULL, ERR_LIST_BAD_PTR);

    free(list->nodes);
    list->nodes = NULL;
    list->capacity = SIZE_MAX;
    list->free = NULL_INDEX;
    
    return 0;
}

//=========================================================================

int push_front(list_t* list, size_t value)
{
    CHECK(list !=  NULL, ERR_LIST_BAD_PTR);

    list->nodes[list->capacity].next = list->capacity + 1;
    ++list->capacity;

    list->nodes[list->capacity].data = value;
    list->nodes[list->capacity].next = list->free;
    list->nodes[list->capacity].prev = list->capacity - 1;

    return 0;
}

//=========================================================================

int list_dump(list_t* list)
{
    CHECK(list !=  NULL, ERR_LIST_BAD_PTR);

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
