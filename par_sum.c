    /*
 * sum.c
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

// aggregate variables
long sum = 0;
long odd = 0;
long min = INT_MAX;
long max = INT_MIN;
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
    sum += number;
    if (number % 2 == 1) {
        odd++;
    }
    if (number < min) {
        min = number;
    }
    if (number > max) {
        max = number;
    }
}

void* process(void* para)
{
    int list_obj = 0;//linked list type object or struct or whatever
                //C:" = 0"
    while(!done)
    {
        while(!done)//linked list is empty do nothing
        {     //C:"!done"
            //get mutex then grab from linked list......this may be done in the while loop need to understand how the stuff works
            //i believe it is done seperatly but maybe they are able to be done together
        }
        sleep(list_obj);//sleep for linked list object amount of time
    }
    pthread_exit(NULL);//or however you cleanup
    return NULL;//remove if not needed
}

int main(int argc, char* argv[])
{
    // check and parse command line options
    if (argc != 3) {
        printf("Usage: sum <infile> <thread_number>\n");
        exit(EXIT_FAILURE);
    }

    struct timeval tv;
    double start, end;
    srand((unsigned)time(NULL));

    char *fn = argv[1];
    int thread_count = atoi(argv[2]);

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
            update(num);//instead add to linked list
        } else if (action == 'w') {     // wait
            sleep(num);
        } else {
            printf("ERROR: Unrecognized action: '%c'\n", action);
            exit(EXIT_FAILURE);
        }
    }

    // start timer
    gettimeofday(&tv, NULL);
    start = tv.tv_sec+(tv.tv_usec/1000000.0);

    // wait for thread to finish
    for(i = 0; i < thread_count; i++)
    {
        pthread_join(thread_handle[i], NULL);//&(hits[i]));
    }

    // stop timer
    gettimeofday(&tv, NULL);
    end = tv.tv_sec+(tv.tv_usec/1000000.0);

    fclose(fin);

    // print results
    printf("%ld %ld %ld %ld\n", sum, odd, min, max);
    printf("Time elapsed: %.3lfs  w/  %d thread(s)\n",
            end-start, thread_count);

    // clean up and return
    return (EXIT_SUCCESS);
}

