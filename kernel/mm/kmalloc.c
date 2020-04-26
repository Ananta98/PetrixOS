#include <mm/kmalloc.h>

#define CMAGIC 0xBEDABEDA

typedef struct chunk {
    int magic;
    size_t size;
    bool allocated;
    struct chunk *prev, *next;
} allocator_chunk;

allocator_chunk *chunk_head;

void kmalloc_init(size_t start, size_t size) {
    if (size < sizeof(allocator_chunk)) {
        chunk_head = NULL;
    } else {
        chunk_head = (allocator_chunk*)start;
        chunk_head->allocated = false;
        chunk_head->prev = NULL;
        chunk_head->next = NULL;
        chunk_head->size = size - sizeof(allocator_chunk);
        chunk_head->magic = CMAGIC;
    }
}

void *malloc(size_t size) {
    allocator_chunk *result = NULL;
    for (allocator_chunk *current = chunk_head; current != NULL; current->next) {
        if (current->magic != CMAGIC)
            return;
        if (!current->allocated && current->size > size)
            result = current;
    }

    if (result->size >= size + sizeof(allocator_chunk) + 1) {
        allocator_chunk *new_chunk = (allocator_chunk*)((size_t)result + sizeof(allocator_chunk) + size);
        new_chunk->allocated = false;
        new_chunk->size = result->size - size - sizeof(allocator_chunk);
        new_chunk->prev = result;
        new_chunk->next = result->next;
        if (new_chunk->next != NULL) {
            new_chunk->next->prev = new_chunk;
        }
        result->size = size;
        result->next = new_chunk;
    }

    result->allocated = true;
    return (void*)(((size_t)result) + sizeof(allocator_chunk));
}

void free(void *ptr) {
    allocator_chunk *chunk = (allocator_chunk*)((size_t)ptr - sizeof(allocator_chunk));
    chunk->allocated = false;
    if (chunk->prev != NULL && !chunk->prev->allocated) {
        chunk->prev->next = chunk->next;
        chunk->prev->size += chunk->size + sizeof(allocator_chunk);
        if (chunk->next != NULL)
            chunk->next->prev = chunk->prev;
        chunk = chunk->prev;
    }
    if (chunk->next != NULL && !chunk->next->allocated) {
        chunk->size += chunk->next->size + sizeof(allocator_chunk);
        chunk->next = chunk->next->next;
        if (chunk->next != NULL)
            chunk->next->prev = chunk;
    }

}