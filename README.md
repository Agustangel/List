# List on arrays
It is project for Ilab (Intel course for DREC MIPT). Doubly linked list realized on arrays. Instead of Nodes with data, pointer on next and previous Nodes we have three arrays: array of data, array of next Nodes, array of previous Nodes. Node is index in array, on this index elment of data equal data of this Node, element of next equal index of next Node, element of previous equal index of previous Node.

## Index
1. [Doxygen documentation.](#1-doxygen-documentation)

2. [Features of work with list on massives.](#2-features-of-work-with-list-on-massives)
3. [Graphviz dump.](#3-graphviz-dump)
4. [Example of main for debugging.](#4-example-of-main-for-debugging)

## 1. Doxygen documentation.
Documentation is there - https://github.com/LegendaryHog/List/tree/main/Documentation.

## 2. Features of work with list on massives.

### Massives.
The biggest trouble in work with this realization of list is massives. I show you examples.

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

All dumps saves in directory "logs" with numbering.

## 4. Example of main for debugging.

```c
#include "List.h"

int main (void)
{
    List* list = (List*) calloc (1, sizeof (List));

    List_Ctor (list);
    Dbg_Dump (list);

    List_Ins_Aft (list, 0, 20);
    List_Ins_Aft (list, 0, 10);
    List_Ins_Aft (list, 1, 30);
    Dbg_Dump (list);
    Graph_Dump (list);
    Linear (list);
    Dbg_Dump (list);
    List_Dtor (list);
    free (list);
    return 0;
}
```

Makefile for debug with this main.
```Makefile
all: comp run

comp:
	gcc List.c main.c -Wall -Wextra -o list.out

run:
	./list.out

val_run:
	valgrind --leak-check=full -s ./list.out
```



