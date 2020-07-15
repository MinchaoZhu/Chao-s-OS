#ifndef KERNEL_SCHEDULE_H_
#define KERNEL_SCHEDULE_H_

#include "kernel/task.h"

#define TASK_SWITCH_FREQUENCY 500


void init_schedule();

void task_schedule();




#endif // KERNEL_SCHEDULE_H_