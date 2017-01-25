#include "linked_list.h"

/****
* Linked list implementation in the form of a queue.
*
* Author: Nathan Johnson
*/

/* function declarations */

/*
* Adds an action to the back of the list.
* 
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
* Pulls a node from the front of the 
*
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

/***
* for testing purposes
****
int main (int argc, char ** argv) {
	print_list();
	add_action(5);
	add_action(6);
	print_list();
	printf("pulled_action: %ld\n",pull_action());
	print_list();
	
	destroy();
	return 0;
}
*/


