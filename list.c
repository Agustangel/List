#include <stdio.h>
#include <logger.h>
#include "list.h"


//=========================================================================
int list_ctor(list_t* list, size_t capacity)
{
    if(list == NULL)
    {
        fprintf(stderr, RED "ERROR: " RESET "INVALID POINTER TO LIST.\n");
        exit(ERR_LIST_NULL_POINTER);
    }

    if(capacity < 0)
    {
        fprintf(stderr, RED "ERROR: " RESET "INCORRECT SIZE OF LIST.\n");
        exit(ERR_LIST_INC_INPUT);
    }
    if(capacity == 0)
    {
        list->nodes = NULL;
        list->capacity = 0;
        list->free = NULL_INDEX;
    }

    list = (list_t*) calloc(capacity, sizeof(list_t));
    if(list == NULL)
    {
        fprintf(stderr, RED "ERROR: " RESET "OUT OF MEMORY.\n");
        return ERR_LIST_OUT_MEMORY;
    }

    list->nodes[NULL_INDEX].data = DATA_POISON;
    list->capacity = capacity;
    list->free = NULL_INDEX;
    list_init_nodes(list, 1);
}

//=========================================================================

int list_init_nodes(list_t* list, listIndex_t start)
{
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
    free(list->nodes);
    list->nodes = NULL;
    list->capacity = SIZE_MAX;
    list->free = NULL_INDEX;
    
    return 0;
}

//=========================================================================
