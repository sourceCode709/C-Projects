/** schedule_rr.c
 * Author: Daniel Brown
 * Student Number: 201637022
 * Term project
 * 
 * Implementation of the Round-Robin (RR) CPU process scheduling algorithm.
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
struct node *task_queue = NULL, *next_task = NULL, *temp, *this_task; 
double turnaround_time = 0, wait_time = 0, response_time = 0; 
int tid = 1000, the_time = 0, time_quantum = 10, cycle = 0, num_tasks = 0;


/* reverse the order of the list */
static void organize(struct node** head_ref){
    struct node* prev = NULL;
    struct node* current = *head_ref;
    struct node* next = NULL;

    while (current != NULL) {
        next = current->next;
        current->next = prev;

        prev = current;
        current = next;
    }
    *head_ref = prev;
}


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
    num_tasks++;
    pthread_mutex_unlock(&mutex);
}


/* select next task to execute */
Task *pickNextTask(){
    /* select most recent task to arrive in list */
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


/* schedule next task to execute */
void schedule(){
    int counter = 0;
    int responses = 0;
    int initial = 0;
    int resp = 0;

    /* reverse order of list */
    organize(&task_queue);

    /* set next task to the first task to arrive in list */ 
    next_task = task_queue;

    printf("Time  Name  tid    priority   Burst duration\n");

    /* schedule task while there are tasks remaining in the task list */
    while (task_queue != NULL){
        Task *task = pickNextTask();

        /* execute and delete task for burst time if it is less */
        /* than or equal to the time quantum */
        if (task->burst <= time_quantum){
            run(task, task->burst, the_time);
            delete(&task_queue, task);
            counter++;

            /* increment number of initial CPU responses and response */
            /* time if it is the task's first response since arrival */
            if (responses < num_tasks && initial){
                response_time += resp;
                resp += task->burst;
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
            if (responses < num_tasks && initial){
                response_time += resp;
                resp += time_quantum;
                responses++;
            }
        }

        /* update CPU performance evaluation metrics */
        wait_time += task->burst;
        turnaround_time += the_time;
        the_time += task->burst;
        initial = 1;
    }
    
    /* compute and output CPU performance evaluation metrics */
    wait_time = turnaround_time - wait_time;
    wait_time /= counter;
    turnaround_time /= counter;
    response_time /= responses;
    
    printf("\nAverage turnaround time: %.2f ms\n", turnaround_time);
    printf("Average wait time: %.2f ms\n", wait_time); 
    printf("Average response time: %.2f ms\n", response_time);
} 


