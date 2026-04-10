#include "types.h"
#include "stat.h"
#include "user.h"
#include "uthread.h"

#define STACK_SIZE 4096

static tid_t next_tid = 0;

// User-mode context layout (matches xv6 kernel struct context)
struct context {
  uint edi;
  uint esi;
  uint ebx;
  uint ebp;
  uint eip;
};

extern void switch_context(struct context **old, struct context *new);

/* minimal NULL definition for no-host-include builds */
#ifndef NULL
#define NULL ((void*)0)
#endif

/* forward declaration of thread entry stub */
static void thread_stub(void);

typedef struct thread {
    tid_t tid;
    void *stack_raw;           // pointer returned by malloc (for free)
    struct context *context;   // pointer to context on thread stack
    void (*fn)(void*);
    void *arg;
    int finished;
    struct thread *next;       // ready-queue link
    struct thread *all_next;   // all-threads list link
} thread_t;

typedef struct thread_queue {
    thread_t *head;
    thread_t *tail;
} thread_queue_t;


static thread_queue_t ready_queue;
static thread_t *current_thread;
static thread_t *all_threads_head;
static struct context *main_ctx; // saved main context pointer

void thread_init(void) {
    ready_queue.head = NULL;
    ready_queue.tail = NULL;
    current_thread = NULL;
    all_threads_head = NULL;
    main_ctx = NULL;
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


//  function to find a thread by its tid
static thread_t *find_thread(tid_t tid) {
    thread_t *t = all_threads_head;
    while (t) {
        if (t->tid == tid) return t;
        t = t->all_next;
    }
    return NULL;
}

tid_t thread_create(void (*fn)(void*), void *arg) {
    thread_t *new_thread = malloc(sizeof(thread_t));
    if (!new_thread) {
        return -1; 
    }

    new_thread->tid = next_tid++;
    new_thread->fn = fn;
    new_thread->arg = arg;
    new_thread->finished = 0;
    new_thread->stack_raw = malloc(STACK_SIZE);
    if (!new_thread->stack_raw) {
        free(new_thread);
        return -1;
    }

    // prepare context at top of stack
    memset(new_thread->stack_raw, 0, STACK_SIZE);
    void *stack_top = (char*)new_thread->stack_raw + STACK_SIZE;
    struct context *ctx = (struct context *)((char*)stack_top - sizeof(struct context));
    memset(ctx, 0, sizeof(*ctx));
    ctx->eip = (uint)thread_stub; // entry point for thread
    new_thread->context = ctx;

    // add to global list of threads
    new_thread->all_next = all_threads_head;
    all_threads_head = new_thread;

    enqueue_thread(new_thread);

    return new_thread->tid;
}

void thread_yield(void) {
    if (!current_thread) {
        // running in main
        thread_t *next = dequeue_thread();
        if (!next) return;
        current_thread = next;
        switch_context(&main_ctx, current_thread->context);
        return;
    }

    // running in a thread
    thread_t *next = dequeue_thread();
    if (!next) return;
    thread_t *prev = current_thread;
    enqueue_thread(prev);
    current_thread = next;
    switch_context(&prev->context, current_thread->context);
}

int thread_join(tid_t tid) {
    thread_t *target = find_thread(tid);
    if (!target) return -1;

    while (!target->finished) {
        thread_yield();
    }
    return 0;
}

// Thread entry wrapper
static void thread_stub(void) {
    if (!current_thread) {
        // shouldn't happen
        return;
    }
    current_thread->fn(current_thread->arg);
    current_thread->finished = 1;
    // switch back to main (scheduler)
    thread_t *t = current_thread;
    current_thread = NULL;
    switch_context(&t->context, main_ctx);
    // never returns
}
