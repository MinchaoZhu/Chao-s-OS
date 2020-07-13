#include "kernel/schedule.h"
#include "interrupt/timer.h"

extern task_list_node_t* tasks;
extern task_list_node_t* tail;
extern task_list_node_t* current_task;


static void switch_context(context_t* cur, context_t* next);

timer_callback task_schedule() {
    if(current_task) {
        task_list_node_t* next = current_task -> next;
        if(next != current_task) {
            task_list_node_t* current_tmp = current_task;
            current_task = next;
            switch_context(&(current_tmp->task_ptr->context), &(next->task_ptr->context));
        }
    }
}

void init_schedule() {
    init_task();
    register_timer(TASK_SWITCH_FREQUENCY, &task_schedule);
}