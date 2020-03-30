typedef struct  {
    struct list_node *next;
    struct list_node *prev;
} list_node;

typedef struct {
    struct list_node *head;
    struct list_node *tail;
} list;

void initialize_list(list *list) {
    list->head = 0;
    list->tail = 0;
}