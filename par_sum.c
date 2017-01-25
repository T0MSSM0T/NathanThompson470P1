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

bool done = false;

// function prototypes
void update(long number);

/*
 * update global aggregate variables given a number
 */
void update(long number)
{
    // simulate computation
    sleep(number);

    // update aggregate variables
    printf("Updating\n");
    pthread_mutex_lock(&sum_mut);
    sum += number;
    pthread_mutex_unlock(&sum_mut);
    if (number % 2 == 1) {
    	pthread_mutex_lock(&odd_mut);
        odd++;
        pthread_mutex_unlock(&odd_mut);
    }
    if (number < min) {
    	pthread_mutex_lock(&min_mut);
        min = number;
        pthread_mutex_unlock(&min_mut);
    }
    if (number > max) {
    	pthread_mutex_lock(&max_mut);
        max = number;
        pthread_mutex_unlock(&max_mut);
    }
}

void* process(void* para)
{
    while(!done)
    {
    	/*
        while(!done)//linked list is empty do nothing
        {     //C:"!done"
            //get mutex then grab from linked list......this may be done in the while loop need to understand how the stuff works
            //i believe it is done seperatly but maybe they are able to be done together
        }
        */
        pthread_mutex_lock(&cond_mut);
        if(is_empty()) {
        	while(pthread_cond_wait(&empty_cond, &cond_mut) != 0);
        }
        pthread_mutex_unlock(&cond_mut);

        //perfect protects list
        pthread_mutex_lock(&list_mut);
        long action = pull_action();
        pthread_mutex_unlock(&list_mut);
        //*****

        update(action);//sleep for linked list object amount of time

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

    done = true;

    // wait for thread to finish
    for(i = 0; i < thread_count; i++)
    {
        pthread_join(thread_handle[i], NULL);//&(hits[i]));
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

