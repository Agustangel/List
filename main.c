#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include "list.h"



int main()
{
    list_t* list;
    size_t capacity = 0;
    scanf("%d", &capacity);

    list_ctor(list, capacity);
    list_dtor(list);

    return 0;
}
