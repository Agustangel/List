#include <stdio.h>
#include <logger.h>
#include "list.h"


int list_ctor(list_t* list, int capacity)
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
    }

    list = (list_t*) calloc(capacity, sizeof(list_t));
    if(list == NULL)
    {
        fprintf(stderr, RED "ERROR: " RESET "OUT OF MEMORY.\n");
        return ERR_LIST_OUT_MEMORY;
    }

    list->nodes[NULL_INDEX] = DATA_POISON;
    list->nodes = capacity;
    list_init_nodes();
}

int list_init_nodes()
{

}
int list_dtor()
{

}
