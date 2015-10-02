#include <system.h>
#include <stdio.h>
#include <string.h>
#include <phapi.h>
#include <sys/cdefs.h>

Llist_t *Llist_create()
{
    Llist_t *head = malloc(sizeof(Llist_t));
    head->prev = NULL;
    head->next = NULL;
    
    return head;
}

void Llist_push(Llist_t *head, Llist_t *last)
{
    Llist_t *current = head;
    while (current->next != NULL) {
        current = current->next;
    }
    
    current->next = last;
    current->next->next = NULL;
    current->next->prev = current;
}

void List_pop(Llist_t *head)
{
    Llist_t *current = head;
    while (current->next != NULL) {
        current = current->next;
    }
    
    current->prev->next = NULL;
    free(current);
}

void Llist_insert_after(Llist_t *location, Llist_t *element)
{
    Llist_t *next_location = location->next;
    
    location->next = element;
    element->next = next_location;
    
    next_location->prev = element;
    element->prev = location;
}

void Llist_insert_before(Llist_t *location, Llist_t *element)
{
    Llist_t *prev_location = location->prev;
    
    location->prev = element;
    element->prev = prev_location;
    
    prev_location->next = element;
    element->next = location;
}

void Llist_remove(Llist_t *location)
{
    Llist_t *next = location->next;
    Llist_t *prev = location->prev;
    
    location->prev->next = next;
    location->next->prev = prev;
    
    free(location);
}

void Llist_destroy(Llist_t *head)
{
    Llist_t *current = head;

    while (current->next != NULL) {
        current->prev = NULL;
        current = current->next;
        free(current->prev);
    }
    current->prev = NULL;
    current->next = NULL;
    free(current);
}

