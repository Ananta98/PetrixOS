#include <lib/types.h>

#define CMAGIC 0xBEDABEDA

typedef struct chunk {
    int magic;
    size_t size;
    struct chunk *prev, *next;
} allocator_chunk;

allocator_chunk *chunk_head;

void kmalloc_init(size_t start, size_t size) {
    if (size < sizeof(allocator_chunk)) {
        chunk_head = NULL;
    } else {
        chunk_head = (allocator_chunk*)start;
        chunk_head->prev = NULL;
        chunk_head->next = NULL;
        chunk_head->size = size - sizeof(allocator_chunk);
    }
}

void *malloc(size_t size) {
    for (allocator_chunk *current = chunk_head; current != NULL; current->next) {
        
    }
}