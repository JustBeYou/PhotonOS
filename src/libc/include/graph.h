#ifndef _tree_h
#define _tree_h

#include <system.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/cdefs.h>

typedef struct graph_node {
    union {
        int id;
        char *name;
    };
    int visited;
    
    void *data;
    Llist_t *nodes;
} graph_node_t;

graph_node_t *graph_create(int id, char *name, void *data);
graph_node_t *graph_find(graph_node_t *base, int id, char *name);
int graph_add_node(graph_node_t *parent, graph_node_t *child);
int graph_is_reachable(graph_node_t *a, graph_node_t *b);
void graph_destroy(graph_node_t *graph);

#endif
