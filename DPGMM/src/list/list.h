#ifndef _LIST_H
#define _LIST_H

#include <stdlib.h>

/* Node in doubly linked list. */
struct ListNode;

typedef struct ListNode {
    struct ListNode *next; /* Pointer to next node in the list. */
    struct ListNode *prev; /* Pointer to previous node in the list. */
    void *value; /* Pointer to this node's value. */
} ListNode;

/* Doubly Linked list */
typedef struct List {
    int count; /* Number of elements in the list. */
    ListNode *first; /* Pointer to first element in the list. */
    ListNode *last; /* Pointer to last element in the list. */
} List;

/* 
 * Returns the number of elements in the list, which is maintained 
 * as elements are added and removed. 
 */
#define List_count(A) ((A)->count)

/* Returns the first element of the list, but does not remove it. */
#define List_first(A) ((A)->first != NULL ? (A)->first->value : NULL)

/* Returns the last element of the list, but does not remove it. */
#define List_last(A) ((A)->last != NULL ? (A)->last->value : NULL)

/* Returns the value of a node  */
#define Node_value(A) ((A) != NULL ? (A)->value : NULL)

/* Iterates over the elements in the list. */
#define LIST_FOREACH(LIST, NODE)\
    ListNode *NODE = NULL;\
    for(NODE = LIST->first; NODE != NULL; NODE = NODE->next)

/* Creates a List struct. */
List *List_create();

/* Destroys the list and any elements it might have. */
void List_destroy(List *list);

/* Frees the values of each node in the list. */
void List_clear(List *list);

/* Clears and destroys the list. */
void List_clear_destroy(List *list);

/* Creates a new node at the end of the list with the specified value. */
void List_push(List *list, void *value);

/* Removes the last element of the list and returns its value. */
void *List_pop(List *list);

/* Removes the node from the list and returns its value. */
void *List_remove(List *list, ListNode *node);

void *List_unshift(List *list);
void List_shift(List *list, void *value);

#endif