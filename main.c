#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include "list.h"



int main()
{
    list_t list;
    size_t capacity = 0;
    scanf("%zu", &capacity);

    list_ctor(&list, capacity);

    push_back(&list, 5);
    push_back(&list, 1);
    push_back(&list, 3);
    insert_after(&list, 1, 7);
    //push_back(&list, 8);

    list_dump(&list);
    //list_dtor(&list);

    return 0;
}
