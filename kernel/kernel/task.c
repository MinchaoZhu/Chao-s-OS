#include "kernel/task.h"
#include "kernel/kmm.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>

task_list_node_t* tasks;
task_list_node_t* tail;
task_list_node_t* current_task;
extern char kernel_stack[STACK_SIZE];
extern page_directory_t pgd_k[1024];

pid_t current_pid = 0;


void init_task() {
    current_pid = 0;
    tasks = (task_list_node_t*) kmalloc(sizeof(task_list_node_t));
    task_struct_t* root = (task_struct_t*) kmalloc(sizeof(task_struct_t)); 
    tasks->next = tasks;
    tasks->prev = tasks;
    tasks->task_ptr = root;
    tail = tasks;

    root->flags = 0;
    root->priority = 0;
    root->pid = current_pid++;
    root->ppid = UINT32_MAX;
    root->stack = (uint32_t) &(kernel_stack);
    root->page_dir = ((uint32_t)&pgd_k) - PAGE_OFFSET;
    root->state = TASK_RUNNABLE;
    current_task = tasks;
}


int32_t thread_create(int (*fn)(void *), void *arg) {
    task_list_node_t* new_task_node = (task_list_node_t*) kmalloc(sizeof(task_list_node_t));
    task_struct_t* new_task = (task_struct_t*) kmalloc(sizeof(task_struct_t)); 

    uint32_t* new_stack = (uint32_t*) kmalloc(STACK_SIZE_KERNEL);

    new_task->flags = 0;
    new_task->priority = 0;
    new_task->pid = current_pid++;
    new_task->ppid = current_task->task_ptr->pid;
    new_task->stack = (uint32_t) new_stack;
    new_task->page_dir = ((uint32_t)&pgd_k) - PAGE_OFFSET;
    new_task->state = TASK_RUNNABLE;

    new_stack = (uint32_t*) ((uint32_t)new_stack +  STACK_SIZE_KERNEL);
    *(--new_stack) = (uint32_t) thread_exit;
    *(--new_stack) = (uint32_t) arg;
    *(--new_stack) = (uint32_t) fn;

    new_task->context.esp = (uint32_t) new_stack;
    new_task->context.eflags = 0x200;

    new_task_node->task_ptr = new_task;
    new_task_node -> next = tail->next;
    new_task_node -> prev = tail;
    tail -> next = new_task_node;
    tasks -> prev = new_task_node;
    tail = new_task_node;

    return new_task->pid;
}




void thread_exit(pid_t pid) {
    if(pid == 0)
        return;
    task_list_node_t* node = tasks; 
    if(tasks->task_ptr->pid != pid){
        node = node -> next;
        while(node != tasks) {
            if(node -> task_ptr ->pid == pid)
                break;
            node = node->next;
        }
        if(node == tasks)
            return;
    }
    task_struct_t* task_info = node->task_ptr;
    kfree(task_info->stack);
    kfree(task_info->page_dir);
    kfree(task_info);
    
    node->prev->next = node->next;
    node->next->prev = node->prev;
    kfree(node);
}