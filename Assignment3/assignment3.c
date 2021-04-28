/*
 ============================================================================
 Filename    : assignment3.c
 Author      : Vincent Rinaldi 239759
 Date        : Nov. 13th, 2017
 ============================================================================
 */


#include <stdio.h>
#include <stdlib.h>
#include <omp.h>	// We include the OMP library

typedef struct node {
	
    int val;
    struct node *next;
    omp_lock_t lock;	// We add a lock pointer in the node structure
    
} node_t;


/*
 * This function initializes a new linked list.
 */
void init_list(node_t **head, int val) {
	
	(*head) = malloc(sizeof(node_t));
	(*head)->val = val;
	(*head)->next = NULL;
	
	omp_init_lock(&((*head)->lock));	// We initialize the lock pointer of the new node which means a node has been created
}

/*
 * This function prints all the elements of a given linked list.
 */
void print_list(node_t *head) {
	
	node_t *current = head;
    
    node_t *prev = NULL;										// We will use an other node initialized to NULL that will act as the previous node of the list to iterate safely on it
    
    if (current != NULL) omp_set_lock(&(current->lock));		// We lock the head of the list if it exists
    
	while (current != NULL) {
		printf("%d\n", current->val);
		
		prev = current;											// After printing the value of the node we set the previous node as the current one	
		current = current->next;
		
		if (current != NULL) omp_set_lock(&(current->lock));	// If it exists we lock the new current node which was the next one before
        omp_unset_lock(&(prev->lock));							// and unlock the old current one which is now the previous one
	}
}

/*
 * This function counts the elements of a given linked list and returns the counted number.
 */
int count(node_t *head)
{
    node_t *current = head;
    
    int count = 0;
    
    node_t *prev = NULL;										// We will use an other node initialized to NULL that will act as the previous node of the list to iterate safely on it
    
    if (current != NULL) omp_set_lock(&(current->lock));		// We lock the head of the list if it exists
    
    while (current != NULL) {
        count++;
        
        prev = current;											// After increasing the count we set the previous node as the current one
        current = current->next;
        
        if (current != NULL) omp_set_lock(&(current->lock));	// If it exists we lock the new current node which was the next one before
        omp_unset_lock(&(prev->lock));							// and unlock the old current one which is now the previous one
    }  
     
    return count; 
}

/*
 * This function appends a new given element to the end of a given linked list.
 */
void append(node_t *head, int val) {
	
    node_t *current = head;
    
    node_t *prev = NULL;						// We will use an other node initialized to NULL that will act as the previous node of the list to iterate safely on it
    
    if (current != NULL) {						// Only if the head exists we proceed
		omp_set_lock(&(current->lock));			// We lock the head
    
		while (current->next != NULL) {
			prev = current;						// After checking if there is a next node we set the previous node as the current one
			current = current->next;
        
			omp_set_lock(&(current->lock));		// We lock the new current node which was the next one before
			omp_unset_lock(&(prev->lock));		// and unlock the old current one which is now the previous one
		}
		
		current->next = malloc(sizeof(node_t));
		current->next->val = val;
		current->next->next = NULL;
    
		omp_init_lock(&(current->next->lock));	// We initialize the lock pointer of the new node which means a node has been created
		omp_unset_lock(&(current->lock));		// We unlock the current node
	}											// We end our if condition and return
}

/*
 * This function adds a new given element to the beginning of a given linked list.
 */
void add_first(node_t **head, int val) {
	
    node_t *new_node;
    
    if (*head != NULL) {					// Only if the head exists we proceed
		omp_set_lock(&((*head)->lock));		// We lock the node that act as the current head to avoid other threads deleting it
		
		new_node = malloc(sizeof(node_t));
		new_node->val = val;
		new_node->next = *head;
    
		omp_init_lock(&(new_node->lock));	// We initialize the lock pointer of the new node which means a node has been created
		
		omp_unset_lock(&((*head)->lock));	// We can unlock the node which is now the old head
    
		*head = new_node;	
	}										// We end our if condition and return
}

/*
 * This function inserts a new given element at the specified position of a given linked list.
 * It returns 0 on a successful insertion, and -1 if the list is not long enough.
 */
int insert(node_t **head, int val, int index) {
	
    if (index == 0) {
        add_first(head, val);
        
        return 0;
    }
    
    node_t * current = *head;
    
    node_t *prev = NULL;						// We will use an other node initialized to NULL that will act as the previous node of the list to iterate safely on it
    
    if (current == NULL) return -1;				// If the head doesn't exist then the list is not long enough
    
    omp_set_lock(&(current->lock));				// We lock the head
    
    for (int i = 0; i < index-1; i++) {
		
        if (current->next == NULL) {	
			omp_unset_lock(&(current->lock));	// If the list is not long enough we don't forget to unlock the current node before returning
					
            return -1;
        }
        
        prev = current;							// After checking if there is a next node we set the previous node as the current one
        current = current->next;
        
        omp_set_lock(&(current->lock));			// We lock the new current node which was the next one before
        omp_unset_lock(&(prev->lock));			// and unlock the old current one which is now the previous one  
    }
    
    node_t *new_node;
    
    new_node = malloc(sizeof(node_t));
    new_node->val = val;
    new_node->next = current->next;
    
	omp_init_lock(&(new_node->lock));			// We initialize the lock pointer of the new node which means a node has been created
    
    current->next = new_node;
    
    omp_unset_lock(&(current->lock));			// We unlock the current node and return
    
    return 0;
}

/*
 * This function pops the first element of a given linked list.
 * The value of that element is returned (if list is not empty), and the element is removed.
 */
int pop(node_t **head) {
	
    int retval = -1;
    
    node_t* next_node = NULL;
    
    if (*head == NULL) {
        return -1;
    }
    
    omp_set_lock(&((*head)->lock));		// We lock the head
    
    next_node = (*head)->next;
    
    retval = (*head)->val;
    
    omp_unset_lock(&((*head)->lock));	// We unlock the head
    omp_destroy_lock(&((*head)->lock));	// to disassociate it from any locks
    
    free(*head);
    *head = NULL;						// We shouldn't forget to set the removed node to NULL after freeing it
    
    *head = next_node;
    
    return retval;
}

/*
 * This function removes the specified element of a given linked list.
 * The value of that element is returned if the list is long enough; otherwise it returns -1.
 * Please note the index starts from 0.
 */
int remove_by_index(node_t **head, int index) {
    
    if (index == 0) {
        return pop(head);
    }
    
    int retval = -1;
    
    if (*head == NULL) return -1;										// If the head doesn't exist then the list is not long enough
    
    omp_set_lock(&((*head)->lock));										// We lock the head
    
    node_t * current = *head;
    
    node_t * previous = NULL;
    
    for (int i = 0; i <= index-1; i++) {
		
        if (current->next == NULL) {
				
			if (previous != NULL) omp_unset_lock(&(previous->lock));	// Because the index doesn't exist we have to unlock both the previous node if there is one
			omp_unset_lock(&(current->lock));							// and the current node before returning
			
            return -1;
        }
        
        if (previous != NULL) omp_unset_lock(&(previous->lock));		// If there is a previous node we know that he has been locked previously and so we have to unlock it before reassigning the previous and current nodes
		
        previous = current;
        current = current->next;
        
        omp_set_lock(&(current->lock));									// We lock the new current node which was the next one before such that the thread holds a lock on two nodes at the same time
    }
    
    // Current node has to be removed!
    previous->next = current->next;
    retval = current->val;
    
    omp_unset_lock(&(current->lock));									// We unlock the current node
    omp_destroy_lock(&(current->lock));									// to disassociate it from any locks
    
    free(current);
    current = NULL;														// We shouldn't forget to set the removed node to NULL after freeing it
    
    omp_unset_lock(&(previous->lock));									// We also have to unlock the previous node before returning the value of the removed node
    
    return retval;
}

/*
 * This function removes the specified element of a given linked list.
 * The value of that element is returned if the element is found; otherwise it returns -1.
 */
int remove_by_value(node_t **head, int val) {
	
    node_t *previous, *current;
    
    if (*head == NULL) {
        return -1;
    }
    
    if ((*head)->val == val) {
        return pop(head);
    }
    
    omp_set_lock(&((*head)->lock));								// We lock the head
    
    previous = *head;
    current = (*head)->next;
    
    if (current != NULL) omp_set_lock(&(current->lock));		// If there is a next node we lock it such that the thread holds a lock on two nodes at the same time
    
    while (current) {
		
        if (current->val == val) {
            previous->next = current->next;
            
            omp_unset_lock(&(current->lock));					// We unlock the current node
            omp_destroy_lock(&(current->lock));					// to disassociate it from any locks
            
            free(current);
            current = NULL;										// We shouldn't forget to set the removed node to NULL after freeing it
            
            omp_unset_lock(&(previous->lock));					// We also have to unlock the previous node before returning the value of the removed node
            
            return val;
        }
        
        omp_unset_lock(&(previous->lock));						// We unlock the previous node before reassigning the previous and current nodes
        
        previous = current;
        current  = current->next;
        
        if (current != NULL) omp_set_lock(&(current->lock));	// If it exists we lock the new current node which was the next one before such that the thread holds a lock on two nodes at the same time
    }
    
    omp_unset_lock(&(previous->lock));							// If there are no more nodes we unlock the previous node and return
    
    return -1;
}

/*
 * This function searched for a specified element in a given linked list.
 * The index of that element is returned if the element is found; otherwise it returns -1.
 */
int search_by_value(node_t *head, int val) {
	
    node_t *current = head;
    
    int index = 0;
    
    if (current == NULL) {
        return -1;
    }
    
    node_t *prev = NULL;										// We will use an other node initialized to NULL that will act as the previous node of the list to iterate safely on it
    
    omp_set_lock(&(current->lock));								// We lock the head
    
    while (current) {
		
        if (current->val == val) {
			omp_unset_lock(&(current->lock));					// If the value matches we don't forget to unlock the current node before returning the index
			
            return index;
        }
        
        prev = current;											// After checking the value of the node we set the previous node as the current one
        current  = current->next;
        
        if (current != NULL) omp_set_lock(&(current->lock));	// If it exists we lock the new current node which was the next one before
        omp_unset_lock(&(prev->lock));							// and unlock the old current one which is now the previous one
        
        index++;
    }
    
    return -1;  
}

/*
 * This function deletes all element of a given linked list, and frees up all the allocated resources.
 */
void delete_list(node_t *head) {
	
    node_t *current = head;
    
    node_t *next;
    
    if (current != NULL) {					// If the head exists
		omp_set_lock(&(current->lock));		// we lock it
		next = current->next;				// then we set the next node
		
		while (next != NULL) {				// While a next node exists
			omp_set_lock(&(next->lock));	// we also lock it
			next = next->next;				// and we iterate through the list
		}									// until every nodes are locked by the thread
	}										// at the same time
    
    while (current) {
        next = current->next;
        
        omp_unset_lock(&(current->lock));	// We unlock the current node
        omp_destroy_lock(&(current->lock));	// to disassociate it from any locks
        
        free(current);      
        current = NULL;						// We shouldn't forget to set the removed node to NULL after freeing it
        
        current = next;
    }
}

int main(void) {
	
	node_t *test_list;
	
	omp_lock_t single_init_lock;												// We declare a lock which will be used to initialize the list only once in our multithreaded region
	omp_init_lock(&single_init_lock);											// and we initialize this lock
	
	#pragma omp parallel num_threads(4) shared(test_list)						// We create our parallel region to work with 4 threads at the same time
	{																			// which share the same list		
		if (omp_test_lock(&single_init_lock)) {									// We set the lock for the initialization of the list without blocking the other threads
			init_list(&test_list, 2);
		}																		// The list will be initialized only once because all the other threads which didn't get the lock are not blocked
		
		add_first(&test_list, 1);
		
		append(test_list, 3);		
		append(test_list, 4);		
		append(test_list, 5);
		
		printf("Count = %d\n", count(test_list));
		remove_by_index(&test_list, 2);
		printf("Count = %d\n", count(test_list));
		
		printf("Search for 5 -> index = %d\n", search_by_value(test_list, 5));
		remove_by_value(&test_list, 5);
		printf("Search for 5 -> index = %d\n", search_by_value(test_list, 5));
		
		print_list(test_list);
		
		delete_list(test_list);
	}																			// We end our parallel region
	
	omp_unset_lock(&single_init_lock);											// It's not mandatory but if the lock that concerns the initialization of the list has been set by the master thread we then can unset it
	omp_destroy_lock(&single_init_lock);										// to disassociate it from any locks
	
    return 0;   
}
