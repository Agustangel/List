#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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

    list->size = 0;
    list->capacity = capacity;

    list->data[NULL_INDEX] = DATA_POISON;
    list->next[NULL_INDEX] = INDEX_POISON;
    list->prev[NULL_INDEX] = INDEX_POISON;
    list_init_data(list);

    list->head = START_INDEX;
    list->tail = NULL_INDEX;
    list->free = START_INDEX;
    list->end  = NULL_INDEX;

    list->status = LIST_SUCCESS;

    printf("next: ");
    for(int idx = 0; idx < list->capacity + 1; ++idx)
    {
        printf("%5d ", list->next[idx]);
    }
    printf("\n");

    return LIST_SUCCESS;
}

//=========================================================================

int list_init_data(list_t* list)
{
    CHECK(list !=  NULL, ERR_LIST_NULL_PTR);

    for(int idx = START_INDEX; idx < list->capacity + 2; ++idx)
    {
        list->data[idx] = DATA_POISON;
        list->next[idx] = START_INDEX + idx;
        list->prev[idx] = FREE_INDEX;
    }

    return LIST_SUCCESS;
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
    list->size = SIZE_MAX;

    list->free = NULL_INDEX;
    list->end  = INDEX_POISON;
    list->head = INDEX_POISON;
    list->tail = INDEX_POISON;
    
    return LIST_SUCCESS;
}

//=========================================================================

int push_back(list_t* list, elem_t value)
{
    insert_after(list, list->tail, value);

    return LIST_SUCCESS;
}

//=========================================================================

int pop_back(list_t* list)
{
    delete_elem(list, list->tail);

    return LIST_SUCCESS;
}

//=========================================================================

int insert_after(list_t* list, listIndex_t lognum, elem_t value)
{
    CHECK(list !=  NULL, ERR_LIST_NULL_PTR);
    CHECK(lognum >= 0, ERR_LIST_BAD_POSITION);

    if (list->size + 1 == list->capacity)
    {
        list_resize(list);
    }
    ++list->size;

    listIndex_t position = get_physical_number(list, lognum);
    if(position != NULL_INDEX)
    {
        CHECK((position >= list->head) && (position <= list->tail), ERR_LIST_BAD_POSITION);
    }

    listIndex_t free = list->free;                     // free save
    list->free = list->next[list->free];               // free up
    
    list->data[free] = value;
    list->prev[free] = position;                       // position -> free

    if(position == list->tail)
    {
        if(list->end != NULL_INDEX)
        {
            list->next[list->end] = free;
        }
        list->end = free;
        list->next[free] = -1;
        ++list->tail;

        return LIST_SUCCESS;
    }
    list->next[free] = list->next[position];            // free -> next[position]

    list->next[position] = free;
    list->prev[list->next[free]] = free;
    ++list->tail;

    return LIST_SUCCESS;
}

//=========================================================================

int insert_before(list_t* list, listIndex_t lognum, elem_t value)
{
    CHECK(list !=  NULL, ERR_LIST_NULL_PTR);
    CHECK(lognum > 0, ERR_LIST_BAD_POSITION);

    if (list->size + 1 == list->capacity)
    {
        list_resize(list);
    }
    ++list->size;

    listIndex_t position = get_physical_number(list, lognum);
    CHECK((position >= list->head) && (position <= list->tail), ERR_LIST_BAD_POSITION);

    listIndex_t free = list->free;
    list->free = list->next[list->free];

    list->data[free] = value;
    list->next[free] = position;
    list->prev[free] = list->prev[position];

    list->prev[position] = free;
    list->next[list->prev[free]] = free;
    ++list->tail;

    return LIST_SUCCESS;   
}

//=========================================================================

int delete_elem(list_t* list, listIndex_t lognum)
{
    CHECK(list !=  NULL, ERR_LIST_NULL_PTR);
    CHECK(lognum > 0, ERR_LIST_BAD_POSITION);

    CHECK(list->size > 0, ERR_LIST_UNDERFLOW);
    --list->size;

    listIndex_t position = get_physical_number(list, lognum);
    CHECK((position >= list->head) && (position <= list->tail), ERR_LIST_BAD_POSITION);

    if(position == list->tail)
    {
        list->next[position] = list->free;
        list->free = position;

        list->next[list->prev[position]] = FREE_INDEX;

        list->data[position] = DATA_POISON;
        list->prev[position] = FREE_INDEX;
        --list->tail;

        return LIST_SUCCESS;
    }

    list->prev[list->next[position]] = list->prev[position];
    list->next[list->prev[position]] = list->next[position];

    list->next[position] = list->free;
    list->free = position;

    list->data[position] = DATA_POISON;
    list->prev[position] = FREE_INDEX;
    --list->tail;

    return LIST_SUCCESS; 
}

//=========================================================================

int get_physical_number(list_t* list, listIndex_t lognum)
{
    CHECK(list !=  NULL, ERR_LIST_NULL_PTR);
    CHECK(lognum >= 0, ERR_LIST_BAD_POSITION);

    int physnum = 0;
    if(lognum != 0)
    {
        physnum = START_INDEX;
    }

    for(int idx = START_INDEX; idx < lognum; ++idx)
    {
        physnum = list->next[physnum];
    }

    return physnum;
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
        list->next[i] = i + 1;
        list->prev[i] = FREE_INDEX;
    }

    return LIST_SUCCESS;
}

//=========================================================================

int list_veryfi(list_t* list)
{
    CHECK(list !=  NULL, ERR_LIST_NULL_PTR);
    bool value = 0;

    value = (list->data == NULL) || (list->next == NULL) || (list->prev == NULL);
    list->status |= value << ERR_LIST_NULL_PTR;

    value = (list->size > list->capacity);
    list->status |= value << ERR_LIST_OVERFLOW;

    value = (list->size < 0) || (list->capacity < 0);
    list->status |= value << ERR_LIST_BAD_SIZE;

    value = (list == (list_t*) list->data) || (list == (list_t*) list->next) || (list == (list_t*) list->prev);
    list->status |= value << ERR_LIST_BAD_PTR;

    value = (list->head > list->tail);
    list->status |= value << ERR_LIST_INC_LIST;

    value = (list->head < 0) || (list->tail < 0);
    list->status |= value << ERR_LIST_BAD_POSITION;

    return LIST_SUCCESS;
}

//=========================================================================

int list_dump(list_t* list)
{
    CHECK(list !=  NULL, ERR_LIST_NULL_PTR);

    if(list->status != 0)
    {
        int error_tmp = 0;
        for(int error_idx = 1; error_idx < ERROR_NUMBER + 1; ++error_idx)
        {
            error_tmp = list->status;
            error_tmp &= 1 << error_idx;
    
            switch (error_tmp)
            {
            case (1 << ERR_LIST_NULL_PTR):
                LIST_ERROR(ERR_LIST_NULL_PTR);
                break;

            case (1 << ERR_LIST_BAD_PTR):
                LIST_ERROR(ERR_LIST_BAD_PTR);
                break;

            case (1 << ERR_LIST_OUT_MEMORY):
                LIST_ERROR(ERR_LIST_OUT_MEMORY);
                break;

            case (1 << ERR_LIST_BAD_SIZE):
                LIST_ERROR(ERR_LIST_BAD_SIZE);
                break;

            case (1 << ERR_LIST_BAD_POSITION):
                LIST_ERROR(ERR_LIST_BAD_POSITION);
                break;

            case (1 << ERR_LIST_UNDERFLOW):
                LIST_ERROR(ERR_LIST_UNDERFLOW);
                break;

            case (1 << ERR_LIST_OVERFLOW):
                LIST_ERROR(ERR_LIST_OVERFLOW);
                break;

            case (1 << ERR_LIST_INC_LIST):
                LIST_ERROR(ERR_LIST_INC_LIST);
                break;
            
            default:
                break;
            };
        }
    }
    printf("\n");
    printf("-----------------------------------------------------------\n");
    printf("\n");

    printf("capacity = %zu\n", list->capacity);
    printf("size = %zu\n", list->size);
    printf("\n");

    printf("head = %d\n", list->head);
    printf("tail = %d\n", list->tail);

    printf("-----------------------------------------------------------\n");
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
        printf("%5d ", list->next[idx]);
    }
    printf("\n");

    printf("prev: ");
    for(int idx = 0; idx < list->capacity + 1; ++idx)
    {
        printf("%5d ", list->prev[idx]);
    }
    printf("\n");

    return LIST_SUCCESS;
}

//=========================================================================

// void graph_dump(list_t* list)
// {
//     Null_check(list);

//     FILE* text = fopen("graph_log.dot", "w");

//     fprintf(text, "digraph G {\n"            
//                   "rankdir = RL;\n");

//     fprintf(text, "\t%lu [shape = plaintext, label =<\n"
//                           "\t<table>\n"
//                           "\t<tr>\n" 
//                           "\t\t<td colspan=\"3\"> %lg </td>\n"
//                           "\t </tr>\n"
//                           "\t <tr>\n"
//                           "\t\t <td> %ld </td>\n"
//                           "\t\t <td> %ld </td>\n"
//                           "\t\t <td> %ld </td>\n"
//                           "\t </tr>\n"
//                           "\t</table>>, style = filled, color = dodgerblue4, fillcolor = mediumaquamarine]\n", 
//                     0, (list->data)[0].value, (list->data)[0].prev, 0, (list->data)[0].next);

   
//     for (size_t index = 1; index <= list->capacity; ++index)
//     {   
//         if ((!isnan((list->data)[index].value)) && ((list->data)[index].next != -1) && ((list->data)[index].prev != -1))
//         {
//            fprintf(text, "\t%lu [shape = plaintext, label =<\n"
//                           "\t<table>\n"
//                           "\t<tr>\n" 
//                           "\t\t<td colspan=\"3\"> %lg </td>\n"
//                           "\t </tr>\n"
//                           "\t <tr>\n"
//                           "\t\t <td> %ld </td>\n"
//                           "\t\t <td> %ld </td>\n"
//                           "\t\t <td> %ld </td>\n"
//                           "\t </tr>\n"
//                           "\t</table>>, style = filled, color = deepskyblue2, fillcolor = lightskyblue]\n", 
//                     index, (list->data)[index].value, (list->data)[index].prev, index, (list->data)[index].next);
//         }

//         else if (((isnan((list->data)[index].value)) && ((list->data)[index].next != -1) && ((list->data)[index].prev == -1)))
//         {
//              fprintf(text, "\t%lu [shape = plaintext, label =<\n"
//                           "\t<table>\n"
//                           "\t<tr>\n" 
//                           "\t\t<td colspan=\"3\"> %lg </td>\n"
//                           "\t </tr>\n"
//                           "\t <tr>\n"
//                           "\t\t <td> %ld </td>\n"
//                           "\t\t <td> %ld </td>\n"
//                           "\t\t <td> %ld </td>\n"
//                           "\t </tr>\n"
//                           "\t</table>>, style = filled, color = darkgreen, fillcolor = lightgreen]\n", 
//                     index, (list->data)[index].value, (list->data)[index].prev, index, (list->data)[index].next);
//         }

//         else
//         {
//             fprintf(text, "\t%lu [shape = plaintext, label =<\n"
//                           "\t<table>\n"
//                           "\t<tr>\n" 
//                           "\t\t<td colspan=\"3\"> %lg </td>\n"
//                           "\t </tr>\n"
//                           "\t <tr>\n"
//                           "\t\t <td> %ld </td>\n"
//                           "\t\t <td> %ld </td>\n"
//                           "\t\t <td> %ld </td>\n"
//                           "\t </tr>\n"
//                           "\t</table>>, style = filled, color = red,  fillcolor = lightcoral]\n", 
//                     index, (list->data)[index].value, (list->data)[index].prev, index, (list->data)[index].next);
//         }
//     }
    
//     fprintf(text, "\t%ld -> %ld [color = dodgerblue4]; \n", 0, (list->data)[0].next);
//     fprintf(text, "\t%ld -> %ld [color = dodgerblue4]; \n", 0, (list->data)[0].prev);

//     for (size_t i = 1; i <= list->capacity; ++i)
//     {   
//         if ((isnan((list->data)[i].value)) && (isnan((list->data)[(list->data)[i].next].value)) && \
//             ((list->data)[i].prev == -1)   && (((list->data)[(list->data)[i].next].prev == -1) || ((list->data)[(list->data)[i].next].prev == 0)))
//         {
//             fprintf(text, "\t%ld -> %ld [color = darkgreen]; \n", i, (list->data)[i].next);
//         }
        
//         else if (!isnan((list->data)[i].value))
//         {
//             if ((!isnan((list->data)[(list->data)[i].next].value)) && ((list->data)[(list->data)[i].next].prev == i))
//             {
//                 fprintf(text, "\t%ld -> %ld [color = deepskyblue2]; \n", i, (list->data)[i].next);
//                 fprintf(text, "\t%ld -> %ld [color = deepskyblue2]; \n", (list->data)[i].next, i);
//             }

//             else if  (((list->data)[i].next == 0) && (i == list->tail))
//             {
//                 fprintf(text, "\t%ld -> %ld [color = deepskyblue2]; \n", i, (list->data)[i].next);
//                 fprintf(text, "\t%ld -> %ld [color = deepskyblue2]; \n", (list->data)[i].next, i);
//             }

//             else 
//             {
//                 fprintf(text, "\t%ld -> %ld [color = darkred]; \n", i, (list->data)[i].next);
//                 fprintf(text, "\t%ld -> %ld [color = darkred]; \n", (list->data)[i].next, i);
//             }
//         }

//         else 
//         {
//             fprintf(text, "\t%ld -> %ld [color = darkred]; \n", i, (list->data)[i].next);
//             fprintf(text, "\t%ld -> %ld [color = darkred]; \n", (list->data)[i].next, i);
//         }
//     }

//     fprintf(text, "\tHEAD [style = filled, color = black, fillcolor = salmon1]; \n");
//     fprintf(text, "\tTAIL [style = filled, color = black, fillcolor = salmon1]; \n");
//     fprintf(text, "\tFREE [style = filled, color = black, fillcolor = salmon1]; \n");

//     fprintf(text, "\tHEAD -> %ld [color = deeppink]; \n", list->head);
//     fprintf(text, "\tTAIL -> %ld [color = deeppink]; \n", list->tail);
//     fprintf(text, "\tFREE -> %ld [color = deeppink]; \n", list->first_free);

//     fprintf(text, "}\n");

//     fclose(text);

//     system("dot -Tjpeg -ograph_log.jpeg graph_log.dot");

//     text = fopen("graph_phys.dot", "w");

//     fprintf(text, "digraph G {\n");

//     fprintf(text, "\t%lu [shape = plaintext, label =<\n"
//                           "\t<table>\n"
//                           "\t<tr>\n" 
//                           "\t\t<td colspan=\"3\"> %lg </td>\n"
//                           "\t </tr>\n"
//                           "\t <tr>\n"
//                           "\t\t <td> %ld </td>\n"
//                           "\t\t <td> %ld </td>\n"
//                           "\t\t <td> %ld </td>\n"
//                           "\t </tr>\n"
//                           "\t</table>>, style = filled, color = dodgerblue4, fillcolor = mediumaquamarine]\n", 
//                     0, (list->data)[0].value, (list->data)[0].prev, 0, (list->data)[0].next);

   
//     for (size_t index = 1; index <= list->capacity; ++index)
//     {   
//         if ((!isnan((list->data)[index].value)) && ((list->data)[index].next != -1) && ((list->data)[index].prev != -1))
//         {
//            fprintf(text, "\t%lu [shape = plaintext, label =<\n"
//                           "\t<table>\n"
//                           "\t<tr>\n" 
//                           "\t\t<td colspan=\"3\"> %lg </td>\n"
//                           "\t </tr>\n"
//                           "\t <tr>\n"
//                           "\t\t <td> %ld </td>\n"
//                           "\t\t <td> %ld </td>\n"
//                           "\t\t <td> %ld </td>\n"
//                           "\t </tr>\n"
//                           "\t</table>>, style = filled, color = deepskyblue2, fillcolor = lightskyblue]\n", 
//                     index, (list->data)[index].value, (list->data)[index].prev, index, (list->data)[index].next);
//         }

//         else if (((isnan((list->data)[index].value)) && ((list->data)[index].next != -1) && ((list->data)[index].prev == -1)))
//         {
//              fprintf(text, "\t%lu [shape = plaintext, label =<\n"
//                           "\t<table>\n"
//                           "\t<tr>\n" 
//                           "\t\t<td colspan=\"3\"> %lg </td>\n"
//                           "\t </tr>\n"
//                           "\t <tr>\n"
//                           "\t\t <td> %ld </td>\n"
//                           "\t\t <td> %ld </td>\n"
//                           "\t\t <td> %ld </td>\n"
//                           "\t </tr>\n"
//                           "\t</table>>, style = filled, color = darkgreen, fillcolor = lightgreen]\n", 
//                     index, (list->data)[index].value, (list->data)[index].prev, index, (list->data)[index].next);
//         }

//         else
//         {
//             fprintf(text, "\t%lu [shape = plaintext, label =<\n"
//                           "\t<table>\n"
//                           "\t<tr>\n" 
//                           "\t\t<td colspan=\"3\"> %lg </td>\n"
//                           "\t </tr>\n"
//                           "\t <tr>\n"
//                           "\t\t <td> %ld </td>\n"
//                           "\t\t <td> %ld </td>\n"
//                           "\t\t <td> %ld </td>\n"
//                           "\t </tr>\n"
//                           "\t</table>>, style = filled, color = red,  fillcolor = lightcoral]\n", 
//                     index, (list->data)[index].value, (list->data)[index].prev, index, (list->data)[index].next);
//         }
//     }
    
//     fprintf(text, "\t{ rank = same; ");

//     for (size_t index = 0; index <= list->capacity; ++index)
//     {
//         fprintf(text, " %ld", index);
//     }

//     fprintf(text, " }\n");

//     for (size_t index = 0; index < list->capacity; ++index)
//     {
//         fprintf(text, " %ld ->", index);
//     }

//     fprintf(text, "%ld [style = invis]; \n");

//     fprintf(text, "\t%ld -> %ld [color = dodgerblue4]; \n", 0, (list->data)[0].next);
//     fprintf(text, "\t%ld -> %ld [color = dodgerblue4]; \n", 0, (list->data)[0].prev);

//     for (size_t i = 1; i <= list->capacity; ++i)
//     {   
//         if ((isnan((list->data)[i].value)) && (isnan((list->data)[(list->data)[i].next].value)) && \
//             ((list->data)[i].prev == -1)   && (((list->data)[(list->data)[i].next].prev == -1) || ((list->data)[(list->data)[i].next].prev == 0)))
//         {
//             fprintf(text, "\t%ld -> %ld [color = darkgreen]; \n", i, (list->data)[i].next);
//         }
        
//         else if (!isnan((list->data)[i].value))
//         {
//             if ((!isnan((list->data)[(list->data)[i].next].value)) && ((list->data)[(list->data)[i].next].prev == i))
//             {
//                 fprintf(text, "\t%ld -> %ld [color = deepskyblue2, dir = both]; \n", i, (list->data)[i].next);
//             }

//             else if  (((list->data)[i].next == 0) && (i == list->tail))
//             {
//                 fprintf(text, "\t%ld -> %ld [color = deepskyblue2, dir = both]; \n", i, (list->data)[i].next);
//             }

//             else 
//             {
//                 fprintf(text, "\t%ld -> %ld [color = darkred]; \n", i, (list->data)[i].next);
//                 fprintf(text, "\t%ld -> %ld [color = darkred]; \n", (list->data)[i].next, i);
//             }
//         }

//         else 
//         {
//             fprintf(text, "\t%ld -> %ld [color = darkred]; \n", i, (list->data)[i].next);
//             fprintf(text, "\t%ld -> %ld [color = darkred]; \n", (list->data)[i].next, i);
//         }
//     }

//     fprintf(text, "\tHEAD [style = filled, color = black, fillcolor = salmon1]; \n");
//     fprintf(text, "\tTAIL [style = filled, color = black, fillcolor = salmon1]; \n");
//     fprintf(text, "\tFREE [style = filled, color = black, fillcolor = salmon1]; \n");

//     fprintf(text, "\tHEAD -> %ld [color = deeppink]; \n", list->head);
//     fprintf(text, "\tTAIL -> %ld [color = deeppink]; \n", list->tail);
//     fprintf(text, "\tFREE -> %ld [color = deeppink]; \n", list->first_free);

//     fprintf(text, "}\n");

//     fclose(text);

//     system("dot -Tjpeg -ograph_phys.jpeg graph_phys.dot");

//     system("convert graph_log.jpeg graph_phys.jpeg -append graph_array.jpeg");
//     system("gwenview graph_array.jpeg");
// }
