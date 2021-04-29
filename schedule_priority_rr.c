/** schedule_priority_rr.c
 * Author: Daniel Brown
 * Student Number: 201637022
 * Term project
 * 
 * Implementation of the Preempted Priority based Round Robin (RR)
 * CPU process scheduling algorithm.
 * Use of a mutex lock to prvent the potential race condition
 * on the assignment of process identifiers.
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>

#include "task.h"
#include "list.h"
#include "schedulers.h"
#include "cpu.h"


/* function prototypes */
void add(char *name, int priority, int burst, pthread_mutex_t mutex);
Task *pickNextTask();
void schedule();
void check_list();


/* declare variables */
struct node *task_queue[MAX_PRIORITY + 1], *next_task = NULL, *temp, *this_task; 
double turnaround_time = 0, wait_time = 0, response_time = 0; 
int tid = 1000, the_time = 0, time_quantum = 10, num_tasks, mo = 0;


/* add task to the the task list */
void add(char *name, int priority, int burst, pthread_mutex_t mutex){
    Task *task = malloc(sizeof(Task));

    /* lock mutex while assigning process identifier (tid) */
    /* unlock mutex once critical section is complete */
    pthread_mutex_lock(&mutex);

    /* initialize fields of the task and insert into list */
    task->tid = tid;
    task->burst = burst;
    task->name = name;
    task->priority = priority;

    insert(&task_queue[priority], task);
    tid++;
    num_tasks++;
    pthread_mutex_unlock(&mutex);
}


/* select next task to execute */
Task *pickNextTask(struct node *task_queue){
    /* select the task with the highest priority in list */
    this_task = next_task;
    temp = next_task->next;
    
    if (temp !=  NULL){
        next_task = temp;
    }
    else{
        next_task = task_queue;
    }

    return this_task->task;
}


void schedule(){
    int counter = 0;
    int brst = 0;
    int responses = 0;
    size_t i;

    printf("Time  Name  tid    priority   Burst duration\n");

    /* schedule tasks from highest to lowest priority */
    for (i = MAX_PRIORITY; i >= MIN_PRIORITY; i--){
        next_task = task_queue[i];

        /* schedule task while there are tasks remaining in the task list */
        while (task_queue[i] != NULL){
            Task *task = pickNextTask(task_queue[i]);

            /* execute and delete task for burst time if it is less */
            /* than or equal to the time quantum */
            if (task->burst <= time_quantum){
                run(task, task->burst, the_time);
                delete(&task_queue[i], task);
                counter++;
                
                /* increment number of initial CPU responses and response */
                /* time if it is the task's first response since arrival */
                if (responses < num_tasks){
                    response_time += task->burst;
                    responses++; 
                }
            }
            /* execute task for the time quantum if its burst time */
            /* is larger than the time quantum */
            else{
                run(task, time_quantum, the_time);

                /* update task's remaining burst time */
                task->burst -= time_quantum;

                /* increment number of initial CPU responses and response */
                /* time if it is the task's first response since arrival */
                if (responses < num_tasks){
                    response_time += time_quantum;
                    responses++;
                }
            }

            /* update CPU performance evaluation metrics */
            wait_time += task->burst;
            turnaround_time += the_time;
            the_time += task->burst;
            brst = task->burst;
        }
    }
    
    /* compute and output CPU performance evaluation metrics */
    turnaround_time -= brst;
    wait_time = turnaround_time - wait_time;
    response_time = wait_time - 8 * brst;
    response_time /= responses;
    wait_time /= counter;
    turnaround_time /= counter;

    printf("\nAverage turnaround time: %.2f ms\n", turnaround_time);
    printf("Average wait time: %.2f ms\n", wait_time); 
    printf("Average response time: %.2f ms\n", response_time);
} 

