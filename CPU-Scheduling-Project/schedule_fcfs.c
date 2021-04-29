/** schedule_fcfs.c
 * Author: Daniel Brown
 * Student Number: 201637022
 * Term project
 * 
 * Implementation of the First-Come-First-Serve (FCFS) CPU process scheduling algorithm.
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


/* declare variables */
struct node *task_queue = NULL, *next_task, *temp; 
double turnaround_time = 0, wait_time = 0, response_time = 0; 
int tid = 1000, the_time = 0;


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

    insert(&task_queue, task);
    tid++;
    pthread_mutex_unlock(&mutex);
}


/* select next task to execute */
Task *pickNextTask(){
    /* select first task to arrive in list */
    next_task = task_queue;
    temp = next_task->next;

    while (temp != NULL){
        next_task = temp;
        temp = temp->next;
    }

    return next_task->task;
}


/* schedule next task to execute */
void schedule(){
    int counter = 0;

    printf("Time  Name  tid    priority   Burst duration\n");

    /* schedule task while there are tasks remaining in the task list */
    while (task_queue != NULL){
        Task *task = pickNextTask();
        run(task, task->burst, the_time);

        /* update CPU performance evaluation metrics */
        wait_time += task->burst;
        the_time += task->burst;
        turnaround_time += the_time;
        counter++;

        delete(&task_queue, task);
    }

    /* compute and output CPU performance evaluation metrics */
    wait_time = turnaround_time - wait_time;
    wait_time /= counter;
    response_time = wait_time;
    turnaround_time /= counter;

    printf("\nAverage turnaround time: %.2f ms\n", turnaround_time); 
    printf("Average wait time: %.2f ms\n", wait_time); 
    printf("Average response time: %.2f ms\n", response_time);    
}