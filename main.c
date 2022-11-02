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
    //push_back(&list, 3);
    insert_before(&list, 2, 7);
    push_back(&list, 6);
    //delete_elem(&list, 2);
    //pop_back(&list);

    list_dump(&list);
    //list_dtor(&list);

    return 0;
}
