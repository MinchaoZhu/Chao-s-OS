#include "kernel/task.h"
#include "kernel/kmm.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "klib/list.h"
#include "klib/map.h"

LIST_HEAD(tasks);
map_t* tasks_map_ptr;
task_struct_t* current_task;
extern char kernel_stack[STACK_SIZE];
extern page_directory_t pgd_k[1024];

pid_t current_pid = 0;


void init_task() {
    tasks_map_ptr = map_init();
    current_pid = 0;
    task_struct_t* new_task = (task_struct_t*) kmalloc(sizeof(task_struct_t));

    new_task->flags = 0;
    new_task->priority = 0;
    new_task->pid = current_pid++;
    new_task->ppid = UINT32_MAX;
    new_task->stack = (uint32_t) &(kernel_stack);
    new_task->page_dir = ((uint32_t)&pgd_k) - PAGE_OFFSET;
    new_task->state = TASK_RUNNABLE;
    current_task = new_task;

    map_insert(new_task->pid, (uint32_t)new_task, tasks_map_ptr);

    list_add_tail(&new_task->list_head, &tasks);
}


int32_t thread_create(int (*fn)(void *), void *arg) {
    
    task_struct_t* new_task = (task_struct_t*) kmalloc(sizeof(task_struct_t));

    uint32_t* new_stack = (uint32_t*) kmalloc(STACK_SIZE_KERNEL);

    new_task->flags = 0;
    new_task->priority = 0;
    new_task->pid = current_pid++;
    new_task->ppid = current_task->pid;
    new_task->stack = (uint32_t) new_stack;
    new_task->page_dir = ((uint32_t)&pgd_k) - PAGE_OFFSET;
    new_task->state = TASK_RUNNABLE;

    new_stack = (uint32_t*) ((uint32_t)new_stack +  STACK_SIZE_KERNEL);
    *(--new_stack) = (uint32_t) thread_exit;
    *(--new_stack) = (uint32_t) arg;
    *(--new_stack) = (uint32_t) fn;

    new_task->context.esp = (uint32_t) new_stack;
    new_task->context.eflags = 0x200;


    map_insert(new_task->pid, (uint32_t)new_task, tasks_map_ptr);
    list_add_tail(&new_task->list_head, &tasks);
    return new_task->pid;
}




void thread_exit(pid_t pid) {
    if(pid == 0)
        return;
    uint32_t value; 
    if(map_find(pid, &value, tasks_map_ptr)) {
        task_struct_t* task = (task_struct_t*) value;
        map_delete(pid, tasks_map_ptr);
        list_del(&task->list_head);
        kfree(task->stack);
        kfree(task->page_dir);
        kfree(task);
    }
}