#include <lib/types.h>

typedef struct list_node {
    struct list_node *next;
    struct list_node *prev;
} list_node_t;

typedef struct list{
    list_node_t *head;
    list_node_t *tail;
} list;

void *pop_tail(list *list);
void *pop_front(list *list);
void initialize_list(list *list);
void push_back(list *list, list_node_t *new_node);
void push_front(list *list, list_node_t *new_node);