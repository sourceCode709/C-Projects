/**
 * "Virtual" CPU that also maintains track of system time.
 */

#include <stdio.h>
#include "task.h"

// run this task for the specified time slice
void run(Task *task, int slice, int the_time) {
    printf("%4d %3s %7d %4d %10d\n", the_time, task->name, task->tid, task->priority, slice);
}
