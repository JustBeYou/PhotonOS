#ifndef _linked_list_h
#define _linked_list_h

#include <system.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>
#include <sys/cdefs.h>

typedef struct Llist {
    struct Llist *prev;
    struct Llist *next;
    void *data;
} Llist_t;

Llist_t *Llist_create();
void Llist_push(Llist_t *head, Llist_t *last);
void List_pop(Llist_t *head);
void Llist_insert_after(Llist_t *location, Llist_t *element);
void Llist_insert_before(Llist_t *location, Llist_t *element);
void Llist_remove(Llist_t *location);
void Llist_destroy(Llist_t *head);

#endif
