/*
 * par_sum.c
 *
 * CS 470 Project 1 (Pthreads)
 * Serial version
 *
 * Compile with --std=c99
 *
 * Authors: Thompson and Nathan
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


//struct defining the node type of the linked list
typedef struct node {
    struct node * next;
    long action;
}node_t;

//first and last pointers to the first and last nodes
node_t * first;
node_t * last;

//mutexes
pthread_mutex_t sum_mut;
pthread_mutex_t odd_mut;
pthread_mutex_t min_mut;
pthread_mutex_t max_mut;
pthread_mutex_t list_mut;
pthread_mutex_t cond_mut;

//conditions
pthread_cond_t empty_cond;


//boolean indicating whether the master thread is done
volatile bool done = false;

// function prototypes
void update(long number);
void add_action(long act);
long pull_action();
bool is_empty();
void print_list();
void destroy();

/**
 * update global aggregate variables given a number
 *
 * @param number the value to update with global values
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

/**
* Process acts as a worker and waits for commands from the master thread
* calls update to update the global values
*
**/
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


/**
* Main spawns worker threads and acts as the master thread.
*
* @param argc number of arguments 
* @param argv: contains a text file of commands and number of threads
* the master thread needs to spawn
*/
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
    
    //initial cond
    pthread_cond_init(&empty_cond,NULL);

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
        usleep(10);//busy waiting
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
    pthread_cond_destroy(&empty_cond);
    
    return (EXIT_SUCCESS);
}

/*
* Adds an action to the back of the list.
* 
* @param act a value indicating the type of action to take
*/
void add_action(long act) {
    node_t * newNode = malloc(sizeof(node_t));
    //printf("Action: %ld\n",act);

    newNode->action = act;
    newNode->next = NULL;
    
    if (first == NULL) {
        first = newNode;
    } else {
        last->next = newNode;
    }
    last = newNode;
}

/*
* Pulls a node from the front of the linked list
*
* @return action is a long that indicates the action to be taken
*/
long pull_action() {
    node_t * temp;
    temp = first;
    long action = -1;

    if(first != NULL) {
        if (temp->next != NULL) {
            first = temp->next;
        } else {
            first = NULL;
        }
        action = temp->action;
    }
    return action;
}

/*
* Returns bool indicating whether the list is empty or not
*
* @return a boolean indicating if the list is null
*/
bool is_empty() {
    return first == NULL;
}

/*
* Prints the list from first node to last
*
*/
void print_list() {
    node_t * current;
    
    current = first;
    
    while (current != NULL) {
        printf("Action: %ld\n", current->action);
        current = current->next;
    }
}

/*
* Frees the malloc'd memory for each node
*
*/
void destroy() {
    node_t * temp;
    
    while(first != NULL) {
        temp = first;
        first = first->next;
        free(temp);
    }
    last = NULL;
}



