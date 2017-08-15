#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"
#include "dbug.h"
#include "str.h"
#include "list.h"

/**
 * Destory and empty the list
 */
bool list_free(list *head) {
    DBUG_ENTER("list_free");

    list *tmp;
    list *tmp2;

    if (head == NULL) {
        DBUG_RETURN(FALSE);
    }

    tmp = head;

    while (tmp2) {
        tmp2 = tmp;
        tmp = tmp->next;
        tmp->next = NULL;
        MEMfree(tmp);
    }

    DBUG_RETURN(TRUE);
}

/**
 * Refresh the list
 */
bool list_reset(list *head) {
    DBUG_ENTER("list_reset");
    DBUG_RETURN(TRUE);
}

/**
 * Create a new list
 */
list* list_new() {
    list *new;
    
    DBUG_ENTER("list_new");

    new = (list *) MEMmalloc(sizeof(list));

    new->value = NULL;
    new->next = NULL;

    DBUG_RETURN(new);
}

/**
 * Push value to the end of the list
 */
list* list_push(list *head, void *value) {
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

    DBUG_RETURN(current);
}

void list_print_str(list *head){
    printf("[ ");
    while((head = head->next))
        printf("%s, ", (char*) head->value);
    printf("]\n");
}

int list_length(list *head) {
    int length;
    DBUG_ENTER("list_length");

    if (head == NULL) {
        return 0;
    }

    length = 0;

    while ((head = head->next) && ++length);

    DBUG_RETURN(length);
}
