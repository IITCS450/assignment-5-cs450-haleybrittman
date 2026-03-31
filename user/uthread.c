#include "types.h"
#include "stat.h"
#include "user.h"
#include "uthread.h"
#include <stdlib.h>
#include <string.h>

#define STACK_SIZE 4096

static tid_t next_tid = 0;

typedef struct thread {
    tid_t tid;
    void *stack;
    void (*fn)(void*);
    void *arg;
    struct thread *next;
} thread_t;

typedef struct thread_queue {
    thread_t *head;
    thread_t *tail;
} thread_queue_t;


static thread_queue_t ready_queue;
static thread_t *current_thread;

void thread_init(void) {
    ready_queue.head = NULL;
    ready_queue.tail = NULL;
    current_thread = NULL;
}

static void enqueue_thread(thread_t *thread) {
    if (ready_queue.tail) {
        ready_queue.tail->next = thread;
    } else {
        ready_queue.head = thread;
    }
    ready_queue.tail = thread;
    thread->next = NULL;
}

// Helper function to dequeue a thread
static thread_t *dequeue_thread() {
    if (!ready_queue.head) {
        return NULL;
    }

    thread_t *thread = ready_queue.head;
    ready_queue.head = thread->next;
    if (!ready_queue.head) {
        ready_queue.tail = NULL;
    }

    return thread;
}

tid_t thread_create(void (*fn)(void*), void *arg) {
    thread_t *new_thread = malloc(sizeof(thread_t));
    if (!new_thread) {
        return -1; 
    }

    new_thread->tid = next_tid++;
    new_thread->stack = malloc(STACK_SIZE);
    if (!new_thread->stack) {
        free(new_thread);
        return -1; 
    }

    new_thread->fn = fn;
    new_thread->arg = arg;

    memset(new_thread->stack, 0, STACK_SIZE);
    void **stack_top = (void **)((char *)new_thread->stack + STACK_SIZE);
    *(--stack_top) = arg; 
    *(--stack_top) = NULL; 

    new_thread->stack = stack_top;

  enqueue_thread(new_thread);

    return new_thread->tid;
}

void thread_yield(void) {
    if (current_thread) {
        enqueue_thread(current_thread);
    }

    thread_t *next_thread = dequeue_thread();
    if (next_thread) {
        thread_t *prev_thread = current_thread;
        current_thread = next_thread;

        
        switch_context(&prev_thread->stack, &current_thread->stack);
    }
}

int thread_join(tid_t tid){ (void)tid; return -1; }
