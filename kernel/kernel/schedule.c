#include "kernel/schedule.h"
#include "interrupt/timer.h"
#include "kernel/task.h"
#include <stdio.h>


extern list_head_t tasks;
extern task_struct_t* current_task;
extern task_struct_t* _next_task;

static void switch_context(context_t* cur, context_t* next);

static inline task_struct_t* next_task(task_struct_t* __next_task) {
    list_head_t* next_list_head = __next_task->list_head.next;
    if(next_list_head == &tasks) {
        next_list_head = next_list_head->next;
    }
    return (task_struct_t*) container_of(next_list_head, task_struct_t, list_head);
}


void task_schedule() {
    asm volatile ("cli");
    if(current_task) {
        task_struct_t* next_tmp = _next_task;
        _next_task = next_task(_next_task);
        if(next_tmp != current_task) {
            task_struct_t* current_tmp = current_task;
            current_task = next_tmp;
            switch_context(&(current_tmp->context), &(next_tmp->context));
        }
    }
    asm volatile ("sti");
}

void init_schedule() {
    init_task();
    register_timer(task_schedule, TASK_SWITCH_FREQUENCY);
}