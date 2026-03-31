#include "types.h"
#include "stat.h"
#include "user.h"
#include "uthread.h"


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

void thread_create(void (*fn)(void*), void *arg){ (void)fn; (void)arg; return -1; }
void thread_yield(void){}
int thread_join(tid_t tid){ (void)tid; return -1; }
