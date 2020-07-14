#include "kernel/schedule.h"
#include "interrupt/timer.h"
#include "kernel/task.h"
#include <stdio.h>


extern list_head_t tasks;
extern task_struct_t* current_task;


static void switch_context(context_t* cur, context_t* next);

static inline task_struct_t* next_task() {
    list_head_t* next_list_head = current_task->list_head.next;
    if(next_list_head == &tasks) {
        next_list_head = next_list_head->next;
    }
    return (task_struct_t*) container_of(next_list_head, task_struct_t, list_head);
}


void task_schedule() {
    if(current_task) {
        task_struct_t* next = next_task();
        if(next != current_task) {
            task_struct_t* current_tmp = current_task;
            current_task = next;
            switch_context(&(current_tmp->context), &(next->context));
        }
    }
}

void init_schedule() {
    init_task();
    register_timer(task_schedule, TASK_SWITCH_FREQUENCY);
}