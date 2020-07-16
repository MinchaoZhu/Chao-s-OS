#ifndef KLIB_QUEUE_H_
#define KLIB_QUEUE_H_

#include <stdint.h>
#include "kernel/sync.h"
#include "kernel/task.h"
#include "kernel/schedule.h"
#include "klib/list.h"

#pragma GCC push_options
#pragma GCC optimize ("O0")
typedef struct circular_queue {
    uint32_t begin;
    uint32_t count;
    uint32_t size;
    uint32_t* arr;
    mutex_t lock;
    struct list_head producer;
    struct list_head consumer;
} circular_queue;

#define CIRCULAR_QUEUE_INIT(name, size, container_ptr) { \ 
    0, 0, size, container_ptr, MUTEX_INIT(name.lock), LIST_HEAD_INIT(name.producer), LIST_HEAD_INIT(name.consumer) \
}

// define a circular queue
// @param size: size of queue
// @param container_ptr: uint32_t array with size no less than SIZE 
#define CIRCULAR_QUEUE(name, size, container_ptr)  \
    struct circular_queue name = CIRCULAR_QUEUE_INIT(name, size, container_ptr)

#define INIT_CIRCULAR_QUEUE(ptr, size, container_ptr) do { \
    (ptr)->begin=0;(ptr)->end=0;(ptr)->size=size;(ptr)->arr=arr; \ 
    INIT_MUTEX(&(ptr)->lock);INIT_LIST_HEAD(&(ptr)->producer);INIT_LIST_HEAD(&(ptr)->consumer); \
}


static inline uint32_t circular_count(int add, struct circular_queue* queue) {
    mutex_acquire(&queue->lock);
    uint32_t ret = (queue->count += add);
    mutex_release(&queue->lock);
    return ret;
}

static inline void circular_enqueue(uint32_t ele, struct circular_queue* queue) {
    while(circular_count(0, queue) >= queue->size) {
        list_move_to_tail(&current_task->list_head, &queue->producer);
        current_task->state = TASK_SLEEPING;
        task_schedule();
    }
    uint32_t index = (queue->begin + circular_count(1, queue) - 1) % queue->size;
    queue->arr[index] = ele;
    if(queue->consumer.next != &(queue->consumer)){
        list_head_t* tmp = queue->consumer.next;
        // insert first waiting task ahead _next_task 
        list_move_to_tail(queue->consumer.next, &_next_task->list_head);
        // set _next task is this waiting one
        _next_task = container_of(tmp, task_struct_t,list_head);
        _next_task->state = TASK_RUNNABLE;
    }
}

static inline uint32_t circular_dequeue(struct circular_queue* queue) {
    while(circular_count(0, queue) == 0) {
        list_move_to_tail(&current_task->list_head, &queue->consumer);
        current_task->state = TASK_SLEEPING;
        task_schedule();
    }
    circular_count(-1, queue);
    uint32_t ret = queue->arr[queue->begin ++];
    queue->begin %= queue->size;
    if(queue->producer.next != &(queue->producer)){
        list_head_t* tmp = queue->producer.next;
        // insert first waiting task ahead _next_task 
        list_move_to_tail(queue->producer.next, &_next_task->list_head);
        // set _next task is this waiting one
        _next_task = container_of(tmp, task_struct_t,list_head);
        _next_task->state = TASK_RUNNABLE;
    }
    return ret;
}



#pragma GCC pop_options

#endif // KLIB_QUEUE_H_