#include <lib/types.h>

typedef struct list_node {
    struct list_node *next;
    struct list_node *prev;
} list_node_t;

typedef struct {
    list_node_t *head;
    list_node_t *tail;
} list;

void initialize_list(list *list) {
    list->head = 0;
    list->tail = 0;
}

void push_back(list *list, list_node_t *new_node) {
    new_node->prev = list->tail;
    new_node->next = 0;
    if (list->tail)
        list->tail->next = new_node;
    list->tail = new_node;
    if (!list->head)
        list->head = new_node;
}

void push_front(list *list, list_node_t *new_node)  {
    new_node->next = list->head;
    new_node->prev = 0;
    if (list->head)
        list->head->prev = new_node;
    list->head = new_node;
    if (!list->tail)
        list->tail = new_node;
}

list_node_t *pop_front(list *list) {
    if (!list->head)
        return NULL; 
    list_node_t *head = list->head;
    list->head = list->head->next;
    if (list->head)
        list->head->prev = 0;
    if (!list->head)
        list->tail = 0;
    head->next = head->prev = 0;
    return head;
}

list_node_t *pop_tail(list *list) {
    if (!list->head)
        return NULL;
    list_node_t *tail = list->tail;
    list->tail = list->tail->prev;
    if (list->tail)
        list->tail->prev = 0;
    if (!list->tail)
        list->head = 0;
    tail->next = tail->prev = 0;
    return tail;
}