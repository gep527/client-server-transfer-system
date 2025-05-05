/* lib file example from HW2 submission of nnm227 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../include/linkedlist.hpp"
#ifdef _cplusplus
extern "C"{
#endif
// Initialize an empty list
void initList(List* list_pointer) {
	list_pointer->head = NULL;
	list_pointer->tail = NULL;
	list_pointer->size = 0;
}

//Clear the list
void clearList(List* list_pointer) {
	Node* current = list_pointer->head;
	while(current != NULL) {
		Node* next = current->next;
		free(current->item);
		free(current);
		current = next;
	}
	initList(list_pointer);
}

// Create node containing item, return reference of it.
Node* createNode(void* item) {
    Node* newNode = (Node*)malloc(sizeof(Node)); // Allocate memory to match size of Node
    if (newNode != NULL) { // Check if malloc was successful
        newNode->item = item;
        newNode->next = NULL;
    }
    return newNode;
}

// Insert item at start of the list.
int insertAtHead(List* list_pointer, void* item){
	Node* newNode = createNode(item);

	if(list_pointer->head == NULL) {
		list_pointer->head = newNode;
		list_pointer->tail = newNode;
	} else {
		newNode->next = list_pointer->head;
		list_pointer->head = newNode;
	}
	list_pointer->size++;
	return 1; //returning 1 means success. 
}

// Insert new item at the end of list.
int insertAtTail(List* list_pointer, void* item) {
	Node* newNode = createNode(item);

	if(list_pointer->head == NULL) {
		list_pointer->head = newNode;
		list_pointer->tail = newNode;
	} else {
		list_pointer->tail->next = newNode;
		list_pointer->tail = newNode;
	}
	list_pointer->size++;
	return 1; //Success
}	

// Insert item at a specified index.
int insertAtIndex(List* list_pointer, int index, void* item) {
	if(index < 0 || index > list_pointer->size) {
		return 0; //Failure
	}

	if(index == 0) {
		return insertAtHead(list_pointer, item);
	} else if (index == list_pointer->size) {
		return insertAtTail(list_pointer, item);
	} else {
		Node* newNode = createNode(item);
		Node* node = list_pointer->head; // start at head
		for(int i = 0; i < index - 1; i++) { // iterate to the node before the index
			node = node->next;
		}
		newNode->next = node->next; // set the new node's next to the node at the index
		node->next = newNode; // set the node at the index to the new node
		list_pointer->size++;
		return 1; //Success
	}
}

// Remove item from start of list and return a reference to it
void* removeHead(List* list_pointer) {
	if (list_pointer->head == NULL) {
		printf("List is empty\n"); // since I'm not confident of how to throw an exception in C, I at least want to warn the user
		return NULL;
	}
	Node* removedNode = list_pointer->head;
	list_pointer->head = list_pointer->head->next;
	if (list_pointer->head == NULL) { //If we removed the only node in the LL
		list_pointer->tail = NULL;
	}
	list_pointer->size--;
	void* item = removedNode->item;
	free(removedNode); //prevent potential memory leak
	return item; // Reference to the item in the removed head.
}

// Remove item from the end of list and return a reference to it
void* removeTail(List* list_pointer) {
	if (list_pointer->head == NULL) {
		printf("List is empty\n"); 
		return NULL;
	}
	Node* removedNode = list_pointer->tail;
	if (list_pointer->size == 1) {
		return removeHead(list_pointer);
	}
	Node* current = list_pointer->head;
	while(current->next != list_pointer->tail) { // iterate to the node before the tail
		current = current->next;
	}
	list_pointer->tail = NULL; // set the tail to null
	current->next = NULL; // set the next of the node before the tail to null
	list_pointer->tail = current; // set the tail to the node before the tail
	list_pointer->size--;
	void* item = removedNode->item;
	free(removedNode); //prevent potential memory leak
	return item; // Reference to the item in the removed tail.
}

// Insert item at a specified index and return a reference to it
void* removeAtIndex(List* list_pointer, int index) {
	if(index < 0 || index > list_pointer->size) {
		return 0; //Failure
	}

	if(index == 0) {
		return removeHead(list_pointer);
	} else if (index == list_pointer->size) {
		return removeTail(list_pointer);
	} else {
		Node* current = list_pointer->head; // start at head
		for(int i = 0; i < index - 1; i++) { // iterate to the node before the index
			current = current->next;
		}
		Node* removedNode = current->next; 
		void* removedItem = removedNode->item;  // Save item before freeing
		current->next = removedNode->next; // set the next of the node before the index to the node after the index
		free(removedNode); //prevent potential memory leak
		list_pointer->size--;
		return removedItem; 
	}

}

// Return item at index
void* itemAtIndex(List* list_pointer, int index) {
	if(index < 0 || index >= list_pointer->size) {
		return NULL; //Failure	
	}

	if(index == 0) {
		return list_pointer->head->item;
	} else if (index == list_pointer->size) {
		return list_pointer->tail->item;
	} else {
		Node* current = list_pointer->head; // start at head
		for(int i = 0; i < index; i++) { // iterate to the node of the index
			current = current->next;
		}
		return current->item;  // Return item at index
	}
}
// Return true if the list contains the given item at least once, false otherwise.
bool containsItem(List* list_pointer, void* item) {

	if(list_pointer->head == NULL) {
		return false;
	}
	Node* current = list_pointer->head;
	while(current != NULL) {
		if(current->item == item) {
			printf("HERE");
			return true;
		}
		current = current->next;
	}
	return false;
}

// Returns the size of the list, measured in nodes.
int size(List* list_pointer) {
	return list_pointer->size;
}

void printList(List* list) {
	Node* node;

  	// Handle an empty node. Just print a message.
	if(list->head == NULL) {
		printf("\nEmpty List");
		return;
	}
	
  	// Start with the head.
	node = (Node*) list->head;

	printf("\nList: \n\n\t"); 
	while(node != NULL) {
		printf("[ %d ]", *(int*)(node->item));

		// Move to the next node
		node = (Node*) node->next;

		if(node !=NULL) {
			printf("-->");
		}
	}
	printf("\n\n");
}

#ifdef _cplusplus
}
#endif
