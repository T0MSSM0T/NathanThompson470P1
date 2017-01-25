/*
 * par_sum.c
 *
 * CS 470 Project 1 (Pthreads)
 * Serial version
 *
 * Compile with --std=c99
 */

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include "linked_list.c"

// aggregate variables
long sum = 0;
long odd = 0;
long min = INT_MAX;
long max = INT_MIN;

//mutex
pthread_mutex_t sum_mut;
pthread_mutex_t odd_mut;
pthread_mutex_t min_mut;
pthread_mutex_t max_mut;
pthread_mutex_t list_mut;
pthread_mutex_t cond_mut;

//conditions
pthread_cond_t empty_cond;

volatile bool done = false;

// function prototypes
void update(long number);

/*
 * update global aggregate variables given a number
 */
void update(long number)
{
    // simulate computation
    //sleep(number);

    // update aggregate variables
    pthread_mutex_lock(&sum_mut);
    sum += number;
    pthread_mutex_unlock(&sum_mut);
    if (number % 2 == 1) {
        pthread_mutex_lock(&odd_mut);
        odd++;
        pthread_mutex_unlock(&odd_mut);
    }

    pthread_mutex_lock(&min_mut);
    if (number < min) {
        
        min = number;
    }
    pthread_mutex_unlock(&min_mut);

    pthread_mutex_lock(&max_mut);
    if (number > max) {
        max = number;
    }
    pthread_mutex_unlock(&max_mut);
}

void* process(void* para)
{
    while(!done)
    {
        pthread_mutex_lock(&cond_mut);
        while(is_empty() && !done) {
            while(pthread_cond_wait(&empty_cond, &cond_mut) != 0);
        }
        pthread_mutex_unlock(&cond_mut);

        //protects list
        pthread_mutex_lock(&list_mut);
        long action = pull_action();
        pthread_mutex_unlock(&list_mut);

        if (action != -1)
        {
            update(action);
        }
    }
    pthread_exit(NULL);//or however you cleanup
}

int main(int argc, char* argv[])
{
    
    // check and parse command line options
    if (argc != 3) {
        printf("Usage: par_sum <infile> <thread_number>\n");
        exit(EXIT_FAILURE);
    }
    
    struct timeval tv;
    double start, end;
    srand((unsigned)time(NULL));

    char *fn = argv[1];
    int thread_count = atoi(argv[2]);
    
    //initialize mutexes
    pthread_mutex_init(&sum_mut, NULL);
    pthread_mutex_init(&odd_mut, NULL);
    pthread_mutex_init(&min_mut, NULL);
    pthread_mutex_init(&max_mut, NULL);
    pthread_mutex_init(&list_mut, NULL); 
    pthread_mutex_init(&cond_mut, NULL); 

    // initialize threading
    pthread_t* thread_handle = malloc(sizeof(pthread_t)*thread_count);

    //thread create
    int i;
    for(i = 0; i < thread_count; i++)
    {
        pthread_create(&thread_handle[i], NULL, process, (void*)0);
    }
    

    // load numbers and add them to the queue
    FILE* fin = fopen(fn, "r");
    char action;
    long num;
    while (fscanf(fin, "%c %ld\n", &action, &num) == 2) {
        if (action == 'p') {            // process
            add_action(num);
            pthread_cond_signal(&empty_cond);//add to linked list
        } else if (action == 'w') {     // wait
            sleep(num);
        } else {
            printf("ERROR: Unrecognized action: '%c'\n", action);
            exit(EXIT_FAILURE);
        }
    }

    while(!is_empty())
    {
        sleep(.1);//busy waiting
    }
    done = true;

    pthread_cond_broadcast(&empty_cond);

    // wait for thread to finish
    for(i = 0; i < thread_count; i++)
    {
        pthread_join(thread_handle[i], NULL);//&(hits[i]));
  //      printf("thread: %d\n",i);
    }

    fclose(fin);

    // start timer
    gettimeofday(&tv, NULL);
    start = tv.tv_sec+(tv.tv_usec/1000000.0);

    // stop timer
    gettimeofday(&tv, NULL);
    end = tv.tv_sec+(tv.tv_usec/1000000.0);

    // print results
    printf("%ld %ld %ld %ld\n", sum, odd, min, max);
    printf("Time elapsed: %.3lfs  w/  %d thread(s)\n",
            end-start, thread_count);

    // clean up and return
    
    //destory mutexes
    pthread_mutex_destroy(&sum_mut);
    pthread_mutex_destroy(&odd_mut);
    pthread_mutex_destroy(&min_mut);
    pthread_mutex_destroy(&max_mut);
    pthread_mutex_destroy(&list_mut);
    pthread_mutex_destroy(&cond_mut);
    
    return (EXIT_SUCCESS);
}

