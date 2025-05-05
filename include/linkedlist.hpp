#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef _cplusplus
extern "C"{
#endif
typedef struct Node{
	void* item;
	struct Node*  next;
} Node;

typedef struct {
	Node* head;
	Node* tail;
    int size;
}List;

// Initialize an empty list
void initList(List* list_pointer);

// Create node containing item, return reference of it.
Node* createNode(void* item);

// Insert new item at the end of list.
int insertAtTail(List* list_pointer, void* item);

// Insert item at start of the list.
int insertAtHead(List* list_pointer, void* item);

// Insert item at a specified index.
int insertAtIndex(List* list_pointer, int index, void* item);

// Remove item from the end of list and return a reference to it
void* removeTail(List* list_pointer);

// Remove item from start of list and return a reference to it
void* removeHead(List* list_pointer);

// Insert item at a specified index and return a reference to it
void* removeAtIndex(List* list_pointer, int index);

// Return item at index
void* itemAtIndex(List* list_pointer, int index);

// Return true if the list contains the given item at least once, false otherwise.
bool containsItem(List* list_pointer, void* item);

// Returns the size of the list, measured in nodes.
int size(List* list_pointer);

// Print List
void printList(List* list_pointer);

#ifdef _cplusplus
}
#endif