#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "list.h"
#include "debug.h"


//=========================================================================
int list_ctor(list_t* list, size_t capacity)
{
    CHECK(list !=  NULL, ERR_LIST_NULL_PTR);
    CHECK(capacity > 0, ERR_LIST_BAD_SIZE);

    list->data = (elem_t*) calloc(capacity + 1, sizeof(elem_t));
    list->next = (listIndex_t*) calloc(capacity + 1, sizeof(listIndex_t));
    list->prev = (listIndex_t*) calloc(capacity + 1, sizeof(listIndex_t));

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

    list->status  = LIST_SUCCESS;
    list->linflag = 1;

    return LIST_SUCCESS;
}

//=========================================================================

int list_init_data(list_t* list)
{
    CHECK(list !=  NULL, ERR_LIST_NULL_PTR);

    for(int idx = START_INDEX; idx < list->capacity + 1; ++idx)
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

    if ((list->linflag == 1) && (lognum != list->size))
    {
        list->linflag = 0;
    }

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

    if(lognum == list->tail)
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

    if ((list->linflag == 1) && (lognum != list->free))
    {
        list->linflag = 0;
    }

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

    if ((list->linflag == 1) && (lognum != list->size))
    {
        list->linflag = 0;
    }
    --list->size;

    listIndex_t position = get_physical_number(list, lognum);
    CHECK((position >= list->head) && (position <= list->tail), ERR_LIST_BAD_POSITION);

    if(lognum == list->tail)
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

int list_linearize(list_t* list)
{
    CHECK(list !=  NULL, ERR_LIST_NULL_PTR);

    elem_t* new_data = (elem_t*) calloc(list->capacity + 1, sizeof(elem_t));
    listIndex_t* new_next = (listIndex_t*) calloc(list->capacity + 1, sizeof(listIndex_t));
    listIndex_t* new_prev = (listIndex_t*) calloc(list->capacity + 1, sizeof(listIndex_t));

    int elem_num = START_INDEX;

    new_data[NULL_INDEX] = DATA_POISON;
    new_data[START_INDEX] = list->data[START_INDEX];
    for (int idx = START_INDEX + 1; idx < list->size + 1; ++idx)
    {
        new_data[idx] = list->data[list->next[elem_num]];
        elem_num = list->next[elem_num];
    }
    list->free = list->size + 1;

    new_next[NULL_INDEX] = INDEX_POISON;
    for (int idx = START_INDEX; idx < list->free; ++idx)
    {
        new_next[idx] = (idx + 1 % list->free) ? idx + 1 : FREE_INDEX;
    }
    for (int idx = list->free; idx < list->capacity; ++idx)
    {
        new_next[idx] = idx + 1;
    }

    new_prev[NULL_INDEX] = INDEX_POISON;
    for (int idx = 1; idx < list->free; ++idx)
    {
        new_prev[idx] = idx - 1;
    }
    for (int idx = list->free; idx < list->capacity; ++idx)
    {
        new_prev[idx] = FREE_INDEX;
    }

    free (list->data);
    free (list->next);
    free (list->prev);
    
    list->data = new_data;
    list->next = new_next;
    list->prev = new_prev;

    list->linflag = 1;

    return LIST_SUCCESS;
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
    printf("\n");

    printf("list->data = %p\n", list->data);
    printf("list->next = %p\n", list->next);
    printf("list->prev = %p\n", list->prev);

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

int graph_dump(list_t* list)
{
    CHECK(list !=  NULL, ERR_LIST_NULL_PTR);

    FILE* text = fopen("graph_log.dot", "w");

    fprintf(text, "digraph G {\n"            
                  "rankdir = RL;\n");

    fprintf(text, "\t%d [shape = plaintext, label =<\n"
                          "\t<table>\n"
                          "\t<tr>\n" 
                          "\t\t<td colspan=\"3\"> %lf </td>\n"
                          "\t </tr>\n"
                          "\t <tr>\n"
                          "\t\t <td> %d </td>\n"
                          "\t\t <td> %d </td>\n"
                          "\t\t <td> %d </td>\n"
                          "\t </tr>\n"
                          "\t</table>>, style = filled, color = dodgerblue4, fillcolor = mediumaquamarine]\n", 
                    0, list->data[0], list->prev[0], 0, list->next[0]);

   
    for (size_t index = 1; index <= list->capacity; ++index)
    {   
        if(!isnan(list->data[index]) && list->next[index] != -1 && list->prev[index] != -1)
        {
           fprintf(text, "\t%zu [shape = plaintext, label =<\n"
                          "\t<table>\n"
                          "\t<tr>\n" 
                          "\t\t<td colspan=\"3\"> %lf </td>\n"
                          "\t </tr>\n"
                          "\t <tr>\n"
                          "\t\t <td> %d </td>\n"
                          "\t\t <td> %zu </td>\n"
                          "\t\t <td> %d </td>\n"
                          "\t </tr>\n"
                          "\t</table>>, style = filled, color = deepskyblue2, fillcolor = lightskyblue]\n", 
                    index, list->data[index], list->prev[index], index, list->next[index]);
        }

        else if(isnan(list->data[index]) && list->next[index] != -1 && list->prev[index] == -1)
        {
             fprintf(text, "\t%zu [shape = plaintext, label =<\n"
                          "\t<table>\n"
                          "\t<tr>\n" 
                          "\t\t<td colspan=\"3\"> %lf </td>\n"
                          "\t </tr>\n"
                          "\t <tr>\n"
                          "\t\t <td> %d </td>\n"
                          "\t\t <td> %zu </td>\n"
                          "\t\t <td> %d </td>\n"
                          "\t </tr>\n"
                          "\t</table>>, style = filled, color = darkgreen, fillcolor = lightgreen]\n", 
                    index, list->data[index], list->prev[index], index, list->next[index]);
        }

        else
        {
            fprintf(text, "\t%zu [shape = plaintext, label =<\n"
                          "\t<table>\n"
                          "\t<tr>\n" 
                          "\t\t<td colspan=\"3\"> %lf </td>\n"
                          "\t </tr>\n"
                          "\t <tr>\n"
                          "\t\t <td> %d </td>\n"
                          "\t\t <td> %zu </td>\n"
                          "\t\t <td> %d </td>\n"
                          "\t </tr>\n"
                          "\t</table>>, style = filled, color = red,  fillcolor = lightcoral]\n", 
                    index, list->data[index], list->prev[index], index, list->next[index]);
        }
    }
    
    fprintf(text, "\t%d -> %d [color = dodgerblue4]; \n", 0, list->next[0]);
    fprintf(text, "\t%d -> %d [color = dodgerblue4]; \n", 0, list->prev[0]);

    for(size_t i = 1; i <= list->capacity; ++i)
    {   
        if(isnan(list->data[i]) && isnan(list->data[list->next[i]]) && list->prev[i] == -1 &&
           (list->prev[list->next[i]] == -1) || (list->prev[list->next[i]] == 0))
        {
            fprintf(text, "\t%zu -> %d [color = darkgreen]; \n", i, list->next[i]);
        }
        
        else if(!isnan(list->data[i]))
        {
            if(!isnan(list->data[list->next[i]]) && list->prev[list->next[i]] == i)
            {
                fprintf(text, "\t%zu -> %d [color = deepskyblue2]; \n", i, list->next[i]);
                fprintf(text, "\t%d -> %zu [color = deepskyblue2]; \n", list->next[i], i);
            }

            else if(list->next[i] == 0 && i == list->tail)
            {
                fprintf(text, "\t%zu -> %d [color = deepskyblue2]; \n", i, list->next[i]);
                fprintf(text, "\t%d -> %zu [color = deepskyblue2]; \n", list->next[i], i);
            }

            else 
            {
                fprintf(text, "\t%zu -> %d [color = darkred]; \n", i, list->next[i]);
                fprintf(text, "\t%d -> %zu [color = darkred]; \n", list->next[i], i);
            }
        }

        else 
        {
            fprintf(text, "\t%zu -> %d [color = darkred]; \n", i, list->next[i]);
            fprintf(text, "\t%d -> %zu [color = darkred]; \n", list->next[i], i);
        }
    }

    fprintf(text, "\tHEAD [style = filled, color = black, fillcolor = salmon1]; \n");
    fprintf(text, "\tTAIL [style = filled, color = black, fillcolor = salmon1]; \n");
    fprintf(text, "\tFREE [style = filled, color = black, fillcolor = salmon1]; \n");

    fprintf(text, "\tHEAD -> %d [color = deeppink]; \n", list->head);
    fprintf(text, "\tTAIL -> %d [color = deeppink]; \n", list->tail);
    fprintf(text, "\tFREE -> %d [color = deeppink]; \n", list->free);

    fprintf(text, "}\n");

    fclose(text);

    system("dot -Tjpeg -ograph_log.jpeg graph_log.dot");

    text = fopen("graph_phys.dot", "w");

    fprintf(text, "digraph G {\n");

    fprintf(text, "\t%d [shape = plaintext, label =<\n"
                          "\t<table>\n"
                          "\t<tr>\n" 
                          "\t\t<td colspan=\"3\"> %lf </td>\n"
                          "\t </tr>\n"
                          "\t <tr>\n"
                          "\t\t <td> %d </td>\n"
                          "\t\t <td> %d </td>\n"
                          "\t\t <td> %d </td>\n"
                          "\t </tr>\n"
                          "\t</table>>, style = filled, color = dodgerblue4, fillcolor = mediumaquamarine]\n", 
                    0, list->data[0], list->prev[0], 0, list->next[0]);

   
    for (size_t index = 1; index <= list->capacity; ++index)
    {   
        if(!isnan(list->data[index]) && list->next[index] != -1 && list->prev[index] != -1)
        {
           fprintf(text, "\t%zu [shape = plaintext, label =<\n"
                          "\t<table>\n"
                          "\t<tr>\n" 
                          "\t\t<td colspan=\"3\"> %lf </td>\n"
                          "\t </tr>\n"
                          "\t <tr>\n"
                          "\t\t <td> %d </td>\n"
                          "\t\t <td> %zu </td>\n"
                          "\t\t <td> %d </td>\n"
                          "\t </tr>\n"
                          "\t</table>>, style = filled, color = deepskyblue2, fillcolor = lightskyblue]\n", 
                    index, list->data[index], list->prev[index], index, list->next[index]);
        }

        else if(isnan(list->data[index]) && list->next[index] != -1 && list->prev[index] == -1)
        {
             fprintf(text, "\t%zu [shape = plaintext, label =<\n"
                          "\t<table>\n"
                          "\t<tr>\n" 
                          "\t\t<td colspan=\"3\"> %lf </td>\n"
                          "\t </tr>\n"
                          "\t <tr>\n"
                          "\t\t <td> %d </td>\n"
                          "\t\t <td> %zu </td>\n"
                          "\t\t <td> %d </td>\n"
                          "\t </tr>\n"
                          "\t</table>>, style = filled, color = darkgreen, fillcolor = lightgreen]\n", 
                    index, list->data[index], list->prev[index], index, list->next[index]);
        }

        else
        {
            fprintf(text, "\t%zu [shape = plaintext, label =<\n"
                          "\t<table>\n"
                          "\t<tr>\n" 
                          "\t\t<td colspan=\"3\"> %lf </td>\n"
                          "\t </tr>\n"
                          "\t <tr>\n"
                          "\t\t <td> %d </td>\n"
                          "\t\t <td> %zu </td>\n"
                          "\t\t <td> %d </td>\n"
                          "\t </tr>\n"
                          "\t</table>>, style = filled, color = red,  fillcolor = lightcoral]\n", 
                    index, list->data[index], list->prev[index], index, list->next[index]);
        }
    }
    
    fprintf(text, "\t{ rank = same; ");

    for (size_t index = 0; index <= list->capacity; ++index)
    {
        fprintf(text, " %zu", index);
    }

    fprintf(text, " }\n");

    for (size_t index = 0; index < list->capacity; ++index)
    {
        fprintf(text, " %zu ->", index);
    }

    fprintf(text, " [style = invis]; \n");

    fprintf(text, "\t%d -> %d [color = dodgerblue4]; \n", 0, list->next[0]);
    fprintf(text, "\t%d -> %d [color = dodgerblue4]; \n", 0, list->prev[0]);

    for (size_t i = 1; i <= list->capacity; ++i)
    {   
        if (isnan(list->data[i]) && isnan(list->data[list->next[i]]) && \
            list->prev[i] == -1  && (list->prev[list->next[i]] == -1 || list->prev[list->next[i]] == 0))
        {
            fprintf(text, "\t%zu -> %d [color = darkgreen]; \n", i, list->next[i]);
        }
        
        else if(!isnan(list->data[i]))
        {
            if(!isnan(list->data[list->next[i]]) && list->prev[list->next[i]] == i)
            {
                fprintf(text, "\t%zu -> %d [color = deepskyblue2, dir = both]; \n", i, list->next[i]);
            }

            else if(list->next[i] == 0 && i == list->tail)
            {
                fprintf(text, "\t%zu -> %d [color = deepskyblue2, dir = both]; \n", i, list->next[i]);
            }

            else 
            {
                fprintf(text, "\t%zu -> %d [color = darkred]; \n", i, list->next[i]);
                fprintf(text, "\t%d -> %zu [color = darkred]; \n", list->next[i], i);
            }
        }

        else 
        {
            fprintf(text, "\t%zu -> %d [color = darkred]; \n", i, list->next[i]);
            fprintf(text, "\t%d -> %zu [color = darkred]; \n", list->next[i], i);
        }
    }

    fprintf(text, "\tHEAD [style = filled, color = black, fillcolor = salmon1]; \n");
    fprintf(text, "\tTAIL [style = filled, color = black, fillcolor = salmon1]; \n");
    fprintf(text, "\tFREE [style = filled, color = black, fillcolor = salmon1]; \n");

    fprintf(text, "\tHEAD -> %d [color = deeppink]; \n", list->head);
    fprintf(text, "\tTAIL -> %d [color = deeppink]; \n", list->tail);
    fprintf(text, "\tFREE -> %d [color = deeppink]; \n", list->free);

    fprintf(text, "}\n");

    fclose(text);

    system("dot -Tjpeg -ograph_phys.jpeg graph_phys.dot");

    system("convert graph_log.jpeg graph_phys.jpeg -append graph_array.jpeg");
    system("gwenview graph_array.jpeg");
}
