#ifndef __LINKED_LIST__
#define __LINKED_LIST__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/****
* Linked list implementation in the form of a queue.
*
* Author: Nathan Johnson
*/

typedef struct __attribute__((__packed__)) node {
        struct node * next; //next node in list
        long action; //number associated with node
}node_t;

node_t * first; //first node in list
node_t * last; //last node in list

void add_action(long act);
long pull_action();
bool is_empty();
void print_list();
void destroy();

#endif
