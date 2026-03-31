#include "types.h"
#include "stat.h"
#include "user.h"
#include "uthread.h"
#include "umutex.h"

void mutex_init(umutex_t *m) {
    m->locked = 0; 
}
void mutex_lock(umutex_t *m) {
    while (__sync_lock_test_and_set(&m->locked, 1)) {
        
        thread_yield();
    }
}
void mutex_unlock(umutex_t *m){ m->locked = 0; }
