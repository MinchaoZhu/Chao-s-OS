#ifndef KERNEL_SYNC_H_
#define KERNEL_SYNC_H_

#include <stdint.h>
#include <stdatomic.h>
#include "kernel/task.h"
#include "klib/list.h"
#include "kernel/schedule.h"

extern task_struct_t* current_task;
extern task_struct_t* _next_task;

/*** Define spin lock, only used in multi-processor architechture
 * */

typedef struct spin_lock {
    uint32_t value; // lock value
    int holder; // holder pid
} spin_lock;

#define SPIN_LOCK_INIT(name) {__ATOMIC_RELAXED, -1}

// declare&define a spin_lock
#define SPIN_LOCK(name) \
    struct spin_lock name = SPIN_LOCK_INIT(name)

// init a dynamically allocated spin_lock
#define INIT_SPIN_LOCK(ptr) do { \ 
    (ptr) -> value = __ATOMIC_RELAXED;(ptr) -> holder = -1; \
} while(0)


static inline void spin_lock_acquire(struct spin_lock *lock) {
    while(__atomic_test_and_set(&lock->value, __ATOMIC_ACQUIRE) != __ATOMIC_RELAXED) {}
    lock->holder = current_task->pid; 
}

static inline void spin_lock_release(struct spin_lock *lock) {
    INIT_SPIN_LOCK(lock);
}

/** define mutex
 * */
typedef struct mutex {
    uint32_t value; // mutex status
    int holder; // holder thread pid
    struct list_head waiting_list; // thread waiting list
} mutex_t;


#define MUTEX_INIT(name) {__ATOMIC_RELAXED, -1, LIST_HEAD_INIT(name.waiting_list)}
#define MUTEX(name) \
    struct mutex name = MUTEX_INIT(name)

#define INIT_MUTEX(ptr) do { \
    (ptr)->value = __ATOMIC_RELAXED;(ptr)->holder = -1; INIT_LIST_HEAD(&(ptr)->waiting_list); \
} while(0)


static inline void mutex_acquire(struct mutex *lock) {
    asm volatile ("cli");
    while(__atomic_test_and_set(&lock->value, __ATOMIC_ACQUIRE) != __ATOMIC_RELAXED) {
        // if lock is in __ATOMIC_ACQUIRE, move thread to waiting list
        list_move_to_tail(&current_task->list_head, &lock->waiting_list);
        current_task->state = TASK_SLEEPING;
        task_schedule();
    }
    // mutex is free __ATOMIC_RELAXED
    lock->holder = current_task->pid;
    asm volatile ("sti");
}

static inline void mutex_release(struct mutex *lock) {
    asm volatile ("cli");
    if(lock->waiting_list.next != &(lock->waiting_list)){
        list_head_t* tmp = lock->waiting_list.next;
        // insert first waiting task ahead _next_task 
        list_move_to_tail(lock->waiting_list.next, &_next_task->list_head);
        // set _next task is this waiting one
        _next_task = container_of(tmp, task_struct_t,list_head);
        _next_task->state = TASK_SLEEPING;
    }
    lock->holder = -1;
    lock->value = __ATOMIC_RELAXED;
    asm volatile ("sti");
}

// /** Define semephore
//  * */

// typedef struct sema_lock {
//     uint32_t value;
//     struct spin_lock s_lock;
//     struct list_head waiting_list;
// } sema_lock;

// #define SEMA_LOCK_INIT(name, value) {value, SPIN_LOCK_INIT(name), LIST_HEAD_INIT(name.waiting_list)}

// // declare&define a sema_lock
// #define SEMA_LOCK(name, value) \
//     struct sema_lock name = SEMA_LOCK_INIT(name, value)

// // init a dynamically allocated sema_lock
// #define INIT_SEMA_LOCK(ptr, value) do { \
//     (ptr) -> value = value;INIT_SPIN_LOCK(&(ptr)->s_lock);INIT_LIST_HEAD(&(ptr)->waiting_list); \
// }




#endif // KERNEL_SYNC_H_