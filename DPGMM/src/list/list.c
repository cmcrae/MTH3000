#include "list.h"
#include "debug.h"

/*
 * Creates a List struct.
 */
List *List_create()
{
	return calloc(1, sizeof(List));
}

/*
 * Destroys a list and any elements it might have.
 */
void List_destroy(List *list)
{
	LIST_FOREACH(list, node) {
		if(node->prev) {
			free(node->prev);
		}
	}
	free(list->last);
	free(list);
}

/*
 * Frees the values in each node. 
 * Does not destroy the list or its elements.
 */
void List_clear(List *list)
{
	LIST_FOREACH(list, node) {
		free(node->value);
	}
}

/*
 * Clears and destroys a list.
 */
void List_clear_destroy(List *list)
{
	LIST_FOREACH(list, node) {
		if(node->prev) {
			free(node->prev->value);
			free(node->prev);
		}
	}
	free(list->last->value);
	free(list->last);
	free(list);
}

/*
 * Adds a new node to the end of the list
 * and assigns it the specified value.
 */
void List_push(List *list, void *value)
{
	ListNode *node = calloc(1, sizeof(ListNode));
	check_mem(node);

	node->value = value;

	if(list->last == NULL) {
		list->first = node;
		list->last = node;
	} else {
		list->last->next = node;
		node->prev = list->last;
		list->last = node;
	}

	list->count++;

error:
	return;
}

/*
 * Removes the last element of a list and returns its value.
 */
 void *List_pop(List *list)
 {
 	ListNode *node = list->last;
 	return node != NULL ? List_remove(list, node) : NULL;
 }

/*
 * Removes specified node from the list and returns its value.
 */
 void *List_remove(List *list, ListNode *node)
 {
 	void *result = NULL;

 	check(list->first && list->last, "List is empty.");
 	check(node, "node can't be NULL");

 	if(node == list->first && node == list->last) {
 		list->first = NULL;
 		list->last = NULL;
 	} else if(node == list->first) {
 		list->first = node->next;
 		check(list->first != NULL, "Invalid list, somehow got a first that is NULL.");
 		list->first->prev = NULL;
 	} else if(node == list->last) {
 		list->last = node->prev;
 		check(list->last != NULL, "Invalid list, somehow got a next that is NULL.");
 		list->last->next = NULL;
 	} else {
 		ListNode *after = node->next;
 		ListNode *before = node->prev;
 		after->prev = before;
 		before->next = after;
 	}

 	list->count--;
 	result = node->value;
 	free(node);

error:
	return result;
 }

void List_shift(List *list, void *value)
{
    ListNode *node = calloc(1, sizeof(ListNode));
    check_mem(node);

    node->value = value;

    if(list->first == NULL) {
        list->first = node;
        list->last = node;
    } else {
        node->next = list->first;
        list->first->prev = node;
        list->first = node;
    }

    list->count++;

error:
    return;
}

void *List_unshift(List *list)
{
    ListNode *node = list->first;
    return node != NULL ? List_remove(list, node) : NULL;
}