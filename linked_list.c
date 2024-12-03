#include <stdlib.h>
#include "linked_list.h"

// Creates and returns a new list
// If compare is NULL, list_insert just inserts at the head
list_t* list_create(compare_fn compare) {
    list_t* list = (list_t*)malloc(sizeof(list_t));
    if (list == NULL) {
        return NULL;
    }

    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
    list->compare = compare; 

    return list;
}

// Destroys a list
// Destroys a list
//Must free everything, use malloc free
void list_destroy(list_t* list)
{
    //exit if no list
    if(!list){
        return;
    }
    //Keep track of current node (start at the head) and itterate through the list, each node free it
    list_node_t* current_node  = list->head;
    while(current_node != NULL){
        list_node_t* next_node = current_node->next;
        free(current_node);
        current_node = next_node;
    }
    free(list);
}

// Returns head of the list
list_node_t* list_head(list_t* list)
{
    if(list == NULL)return NULL;
    return list->head;
}

// Returns tail of the list
list_node_t* list_tail(list_t* list)
{
    if(list == NULL)return NULL;
    return list->tail;
}

// Returns next element in the list
list_node_t* list_next(list_node_t* node)
{
    if(node == NULL)return NULL;
    return node->next;
}
// Returns prev element in the list
list_node_t* list_prev(list_node_t* node)
{
    if(node == NULL)return NULL;
    return node->prev;
}

// Returns end of the list marker
list_node_t* list_end(list_t* list)
{
    /* IMPLEMENT THIS IF YOU WANT TO USE LINKED LISTS */
    return NULL;
}

// Returns data in the given list node
void* list_data(list_node_t* node)
{
    if(node == NULL)return NULL;
    return node->data;
}

// Returns the number of elements in the list
size_t list_count(list_t* list)
{
    if(list == NULL)return 0;
    return list->count;
}

// Finds the first node in the list with the given data
// Returns NULL if data could not be found
list_node_t* list_find(list_t* list, void* data)
{
    // make sure its not null, 
    //start from the head and itterate through the list and compare each node data with the diven data.
    // if matches return, else move to the next
    if(list != NULL){
        list_node_t* current_node = list_head(list);
        while (current_node != NULL)
        {
            if(current_node->data == data){
                return current_node;
            }
            current_node =  list_next(current_node);
        }
        
    }

    return NULL;
}

list_node_t* list_insert_tail(list_t* list, void* data) {
    if (list == NULL) {
        return NULL; // Return NULL if the list is NULL
    }

    list_node_t* new_node = (list_node_t*)malloc(sizeof(list_node_t));
    if (new_node == NULL) {
        return NULL; 
    }

    // Initialize the new node
    new_node->data = data;
    new_node->next = NULL; // will be the last node

    if (list->tail == NULL) {
        // epmty list so this new node is both head and tail
        new_node->prev = NULL; // Since its  also the head
        list->head = new_node;
        list->tail = new_node;
    } else {
        // not empty so append this node at the end
        new_node->prev = list->tail;
        list->tail->next = new_node;
        list->tail = new_node;
    }

    list->count++;
    return new_node; 
}

list_node_t* list_insert_head(list_t* list, void* data) {
    if (list == NULL) {
        return NULL;
    }

    list_node_t* new_node = (list_node_t*)malloc(sizeof(list_node_t));
    if (new_node == NULL) {
        return NULL;
    }

    new_node->data = data;
    new_node->next = list->head;
    new_node->prev = NULL;

    if (list->head != NULL) {
        list->head->prev = new_node;
    } else {
        // List was empty, so this new node is also the tail
        list->tail = new_node;
    }

    list->head = new_node;
    list->count++;

    return new_node;
}




list_node_t* list_insert(list_t* list, void* data) {
    if (list == NULL) {
        return NULL;
    }

    list_node_t* new_node = (list_node_t*)malloc(sizeof(list_node_t));
    if (new_node == NULL) {
        return NULL;
    }

    new_node->data = data;

    // check if the list is empty
    if (list->head == NULL) {
        list->head = new_node;
        list->tail = new_node;
        new_node->prev = NULL;
        new_node->next = NULL;
    } else if (list->compare == NULL) {
        // IMPORTANT: insert at the head if no comparison function is specified
        new_node->next = list->head;
        new_node->prev = NULL;
        if (list->head != NULL) {
            list->head->prev = new_node;
        }
        list->head = new_node;
        if (list->tail == NULL) {
            list->tail = new_node;
        }
    } else {
        // Insert in a sorted manner according to the comparison function

        list_node_t* current = list->head;
        while (current != NULL && list->compare(current->data, data) < 0) {
            current = current->next;
        }
        if (current == NULL) {
            // Insert at the end
            new_node->prev = list->tail;
            new_node->next = NULL;
            if (list->tail != NULL) {
                list->tail->next = new_node;
            }
            list->tail = new_node;
        } else {
            // Insert before the current node
            new_node->next = current;
            new_node->prev = current->prev;
            if (current->prev != NULL) {
                current->prev->next = new_node;
            } else {
                list->head = new_node;
            }
            current->prev = new_node;
        }
    }

    list->count++;
    return new_node;
}






// Removes a node from the list and frees the node resources
void list_remove(list_t* list, list_node_t* node) {
    if (list == NULL || node == NULL) {
        return;
    }

    //check the first node in the list is not the one to be removed
    if (node->prev != NULL) {
        node->prev->next = node->next;
    } else {
        list->head = node->next;
    }
    //check if the node to be removed is not the last in the list
    if (node->next != NULL) {
        node->next->prev = node->prev;
    } else {
        list->tail = node->prev;
    }

    free(node);
    list->count--;
}
