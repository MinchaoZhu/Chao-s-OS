#ifndef KERNEL_TASK_H_
#define KERNEL_TASK_H_
#include <stdint.h>
#include "kernel/vmm.h"
#include "klib/list.h"

typedef uint32_t pid_t;

#define USER_THREAD 1

#define STACK_SIZE_KERNEL STACK_SIZE

typedef enum task_state {
    TASK_UNINIT = 0,    
    TASK_SLEEPING = 1,  // waitinf for signal to be waken   
    TASK_RUNNABLE = 2,  //under running or ready to be running
    TASK_ZOMBIE = 3,  
} task_state;

typedef struct context {
    uint32_t esp;
    uint32_t ebp;
    uint32_t ebx;
    uint32_t esi;
    uint32_t edi;
    uint32_t eflags;
} context_t;


typedef struct task_struct {
    uint32_t flags;             // flags
    uint32_t priority;           // its priority in scheduler
    pid_t     pid;              // unique pid
    pid_t     ppid;             // parant's pid
    uint32_t     stack;            // thread stack
    context_t context;          // its running context
    page_directory_t* page_dir; // its page directory physic address
    volatile task_state state;  // its state
    struct list_head list_head;
} task_struct_t;



int32_t thread_create(int (*fn)(void *), void *arg);

void thread_exit(pid_t pid);

void init_task(void);



#endif // KERNEL_TASK_H_