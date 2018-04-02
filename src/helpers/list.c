#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"
#include "dbug.h"
#include "str.h"

#include "list.h"

/**
 * Destroy and empty the list
 */
bool list_free(list *head)
{
    list *tmp;
    list *curr;

    DBUG_ENTER("list_free");

    if (head == NULL) {
        DBUG_RETURN(FALSE);
    }

    curr = head;

    while (curr) {
        tmp = curr;
        curr = curr->next;
        tmp->next = NULL;
        MEMfree(tmp);
    }

    DBUG_RETURN(TRUE);
}

/**
 * Refresh the list
 */
bool list_reset(list *head)
{
    DBUG_ENTER("list_reset");

    if (head == NULL) {
        DBUG_RETURN(TRUE);
    }

    head->next = NULL;

    DBUG_RETURN(list_free(head->next));
}

/**
 * Create a new list
 */
list* list_new()
{
    list *new;

    DBUG_ENTER("list_new");

    new = (list *) MEMmalloc(sizeof(list));

    new->value = NULL;
    new->next  = NULL;

    DBUG_RETURN(new);
}

/**
 * Push value to the end of the list
 */
bool list_reversepush(list *head, void *value)
{
    list *add;
    list *current;

    DBUG_ENTER("list_push");

    if (head == NULL) {
        DBUG_RETURN(FALSE);
    }

    current = head;

    // Go to the last list item
    while (current->next && (current = current->next));

    add = list_new();
    add->value = value;
    add->next = NULL;

    // Add the new item to the (previous) tail
    current->next = add;

    DBUG_RETURN(TRUE);
}

/**
 * Push the value to the front of the list
 */
bool list_push(list *head, void *value)
{
    list *new;

    DBUG_ENTER("list_push");

    if (head == NULL) {
        DBUG_PRINT("LIST", ("HEAD CANT BE NULL"));
        DBUG_RETURN(FALSE);
    }

    new = list_new();
    new->value = value;
    new->next  = head->next;

    head->next = new;

    DBUG_RETURN(TRUE);
}

void list_print_str(list *head)
{
    printf("[ ");
    while((head = head->next))
        printf("%s, ", (char*) head->value);
    printf("]\n");
}

int list_length(list *head)
{
    int length;
    DBUG_ENTER("list_length");

    if (head == NULL) {
        DBUG_RETURN(0);
    }

    length = 0;

    while ((head = head->next) && ++length);

    DBUG_RETURN(length);
}
