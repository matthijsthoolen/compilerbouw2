#ifndef _LIST_H
#define _LIST_H

typedef struct list list;

struct list {
    void *value;
    void *next;
};

bool list_free(list *head);
bool list_reset(list *head);
list* list_new();

list* list_push(list *head, void *value);
int list_length(list *head);

#endif
