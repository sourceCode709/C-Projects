/** Author: Daniel Brown
 *  Date: March 1, 2021
 * 
 * This code uses two threads running concurrently. One thread
 * accepts user input and the other thread computes the sum of
 * the numbers as they are input.
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/** global variables */
int sum = 0;
int num = 0;
static pthread_mutex_t mymutex;

/** function prototypes */
void *user_input();
void *sum_nums();

int main(void){
    /** initialize mutex, threads and attributes */
    pthread_mutex_init(&mymutex, NULL);

    pthread_t tid1;
    pthread_t tid2;

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    /** create and join threads */
    pthread_create(&tid1, &attr, user_input, NULL);
    pthread_create(&tid2, &attr, sum_nums, NULL);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    return 0;
}

void *user_input(){
    /** lock mutex before critical section and unlock after */
    /** get user input */
    while (num >= 0){
        pthread_mutex_lock(&mymutex);
        printf("Input a number:\n");
        scanf("%d", &num);
        pthread_mutex_unlock(&mymutex);
        sleep(1);
    }

    /** exit thread upon completion */
    pthread_exit(0);
}

void *sum_nums(){
    /** lock mutex before critical section and unlock after */
    /** compute sum as input is given by user */
    while (num >= 0){
        if (num > 0){
            pthread_mutex_lock(&mymutex);
            sum = sum + num;
            printf("Current total: %d\n", sum);
            pthread_mutex_unlock(&mymutex);
            num = 0;
            sleep(1);
        }
    }
    
    /** display the sum of the input numbers upon completion */
    printf("The sum of the input numbers is %d\n", sum);
    pthread_exit(0);
    
}
