#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/cdefs.h>

graph_node_t *graph_create(void *data)
{
    graph_node_t *g = malloc(sizeof(graph_node_t));
    g->visited = 0;
    g->data = data;
    g->nodes = malloc(sizeof(Llist_t));
    g->nodes->prev = NULL;
    g->nodes->next = NULL;
    g->nodes->data = NULL;
    
    return g;
}

graph_node_t *graph_find(graph_node_t *base, void *data, int (*cmp)(void*, void*))
{
    graph_node_t *found_g = NULL;
    STACK nodes_stack = stack_init(NULL, NULL);
    stack_push(nodes_stack, (void*) base);
    
    while (stack_empty(nodes_stack) == STACK_NOT_EMPTY) {
        graph_node_t *node = (graph_node_t*) stack_head(nodes_stack);
        stack_pop(nodes_stack);
        
        if (!node->visited) {
            node->visited = 1;
            
            if (cmp(node->data, data)) {
                found_g = node;
                break;
            }
            
            Llist_t *temp_l = node->nodes;
            while (temp_l != NULL) {
                graph_node_t *temp_n = (graph_node_t*) temp_l->data;
                stack_push(nodes_stack, (void*) temp_n);
                temp_l = temp_l->next;
            }
        }
    }
    
    stack_delete(&nodes_stack);
    return found_g;
}

int graph_add_node(graph_node_t *parent, graph_node_t *child)
{
    if (parent->nodes->data == NULL) {
        parent->nodes->data = (void*) child;
        return 0;
    } else {
        Llist_t *node_list = parent->nodes;
        while (node_list->next != NULL) {
            node_list = node_list->next;
        }
        node_list->next = malloc(sizeof(Llist_t));
        node_list->next->next = NULL;
        node_list->next->prev = node_list;
        node_list->next->data = (void*) child;
        return 0;
    }
    
    return 1;
}

int graph_is_reachable(graph_node_t *a, graph_node_t *b)
{
    Llist_t *node_list = a->nodes;
    while (node_list != NULL) {
        graph_node_t *cmp_data = (graph_node_t*) node_list->data;
        if (cmp_data == b) {
            return 1;
        }
        node_list = node_list->next;
    }
    
    return 0;
}

void graph_destroy(__attribute__((unused)) graph_node_t *graph)
{
    // TODO: implement destroy
}
