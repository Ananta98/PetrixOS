#include <lib/types.h>

typedef struct frame {
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t ecx;
    
} frame_t;

typedef struct thread {
    int thread_id;
    char name[16];
    int priority;
    int thread_state;
    struct frame stack_frame;
    struct thread *parent;
    struct thread *next;
} thread_t;