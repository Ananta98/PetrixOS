#include <lib/types.h>

void kfree(void *ptr);
void *kmalloc(size_t size);
void initialize_kmalloc(size_t start, size_t size);
