# List on arrays
It is project for Ilab (Intel course for DREC MIPT). Doubly linked list realized on arrays. Instead of Nodes with data, pointer on next and previous Nodes we have three arrays: array of data, array of next Nodes, array of previous Nodes. Node is index in array, on this index elment of data equal data of this Node, element of next equal index of next Node, element of previous equal index of previous Node.

## Index
1. [Doxygen documentation.](#1-doxygen-documentation)

2. [Features of work with list on massives.](#2-features-of-work-with-list-on-massives)
3. [Graphviz dump.](#3-graphviz-dump)
4. [Example of main for debugging.](#4-example-of-main-for-debugging)

## 1. Doxygen documentation.
Documentation is there - https://github.com/LegendaryHog/List/tree/main/Documentation.

## 2. Features of work with list on arrays.

### arrays.
The biggest trouble in work with this realization of list is arrays. I show you examples.

In ordinary list on pointer:

```c    
node = node->next; //go forward on next link

int num = node->next->data; //num is equal to data of next Node of node 
```

In list on arrays:
```c
index = list->next[index]; //go forward on next link

int num = list->data[list->next[index]]; // num is equal to data of next Node of index
```
Cycles in list on pointer:
```c
for (Node* node = firstnode; node != NULL; node = node->next)
{
    ; //cycle body
}
```

This cycle in list on arrays:
```c
for (long long i = list->next[START_END]; i != START_END; i = list->next[i])
{
    ; //cycle body
}
```

Anological for cycles on previous link.

### Push

To push in list, use `int List_Ins_Aft (List* list, long long last, data_t push)`.

*Warning!!!*: last is **physical** number in list. To take physical number by logical number use fucntion `long long Logic_To_Phys (List* list, long long lognum)` taht return physical number.

If you call `int Linear (List* list)` list becomre linerized: logical numbers equal to physical.

## 3. Graphviz dump.

Example of Graphviz Dump (from [main](#4-example-of-main-for-debugging))

![Example of Graphviz Dump (from [main](#4-example-of-main-for-debugging))](https://github.com/Agustangel/List/blob/master/graph_log.jpeg "a title")

## 4. Example of main for debugging.

```c
#include "List.h"

int main (void)
{
    list_t list;
    size_t capacity = 0;
    scanf("%zu", &capacity);

    list_ctor(&list, capacity);

    push_back(&list, 5);
    push_back(&list, 1);
    insert_after(&list, 1, 7);
    delete_elem(&list, 2);
    pop_back(&list);
    
    list_linearize(&list);
    graph_dump(&list);
    list_dump(&list);
    
    list_dtor(&list);

    return 0;
}
```
