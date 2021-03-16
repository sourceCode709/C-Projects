/** Author: Daniel Brown
 *  Date: March 10, 2021
 * 
 * This code simulates farmers headed north and south crossing
 * a single lane bridge using two threads running concurrently
 * with two unnamed semaphores to avoid any potential race conditions.
 */  


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <semaphore.h>

/** declare global semaphores and variables */
sem_t sem_north;
sem_t sem_south;

bool crossing_bridge = false;
int north_waitlist;
int south_waitlist;
int max_time;
int num_north, north_crossed;
int num_south, south_crossed;

/** function prototypes */
void *cross_bridge_north();
void *cross_bridge_south();
int exit_bridge_north();
int exit_bridge_south();

int main(int argc, char *argv[]){
    /** initialize variables */
    max_time = atoi(argv[1]);
    num_north = atoi(argv[2]);
    num_south = atoi(argv[3]);

    /** display number of travalers in each direction */
    printf("%d northbound farms.\n", num_north);
    printf("%d southbound farms.\n", num_south);

    /** initialize semaphores */
    sem_init(&sem_north, 0, 1);
    sem_init(&sem_south, 0, 1);

    /** declare, initialize, create and join threads */
    pthread_t tid1;
    pthread_t tid2;

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    /** create and join threads */
    pthread_create(&tid1, &attr, cross_bridge_north, NULL);
    pthread_create(&tid2, &attr, cross_bridge_south, NULL);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    return 0;
}


void *cross_bridge_north(){
    /** cross bridge while there are northbound travelers ramining */
    while (num_north > 0){
        if (south_waitlist > 0 || crossing_bridge){
            north_waitlist++;
            sem_wait(&sem_north);

            while (crossing_bridge){
                sem_wait(&sem_north);
            }
        
            north_waitlist--;
        }

        /** cross and exit bridge */
        crossing_bridge = true;
        exit_bridge_north(); 

        if (south_waitlist == 0){
            sem_post(&sem_north);
        }
    }

    /** exit thread upon completion */
    pthread_exit(0);
}


void *cross_bridge_south(){
    /** cross bridge while there are southbound travelers ramining */
    while (num_south > 0){
        if (north_waitlist > 0 || crossing_bridge){
            south_waitlist++;
            sem_wait(&sem_south);

            while (crossing_bridge){
                sem_wait(&sem_south);
            }

            south_waitlist--;
        }

        /** cross and exit bridge */
        crossing_bridge = true;
        exit_bridge_south();

        if (north_waitlist == 0){
            sem_post(&sem_south);
        }
    }

    /** exit thread upon completion */
    pthread_exit(0);
}


/** exit north of bridge after random amount of commuting time */
int exit_bridge_north(){
    int delay = rand() % max_time;
    sleep(delay);

    north_crossed++;
    num_north--;
    crossing_bridge = false;

    printf("Farmer %d north crossed the bridge in %d time.\n", north_crossed, delay);

    if (south_waitlist == 0){
        sem_post(&sem_north);
    }

    else{
        sem_post(&sem_south);
    }
}


/** exit south of bridge after random amount of commuting time */
int exit_bridge_south(){
    int delay = rand() % max_time;
    sleep(delay);

    south_crossed++;
    num_south--;
    crossing_bridge = false;

    printf("Farmer %d south crossed the bridge in %d time.\n", south_crossed, delay);

    if (north_waitlist == 0){
        sem_post(&sem_south);
    }

    else{
        sem_post(&sem_north);
    }
}
