#include "linkedlist.h"
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

void insertAtBeginning(LinkedListPtr *head, void *value) {
  LinkedListPtr newPtr = malloc(sizeof(node_t));
  newPtr->data = value;
  newPtr->next = *head;
  newPtr->previous = NULL;
  *head = newPtr;

  if (newPtr->next != NULL)
    newPtr->next->previous = newPtr;
}

void insertAtEnd(LinkedListPtr *head, void *value) {

  if (head == NULL)
    insertAtBeginning(head, value);
  else {
    LinkedListPtr newPtr = malloc(sizeof(node_t));
    LinkedListPtr currentPtr = *head;

    while (currentPtr->next != NULL) {
      currentPtr = currentPtr->next;
    }

    newPtr->data = value;
    newPtr->next = NULL;
    newPtr->previous = currentPtr;
    currentPtr->next = newPtr;
  }
}

void *getValueAtIndex(LinkedListPtr *head, int index) {

  node_t *node = getNodeAtIndex(head, index);

  if (node == NULL)
    return NULL;

  return node->data;
}

LinkedListPtr deleteNode(LinkedListPtr *current) {
  LinkedListPtr next = (*current)->next;
  LinkedListPtr previous = (*current)->previous;

  if (next != NULL && previous != NULL) {
    previous->next = next;
    next->previous = previous;
  } else if (previous != NULL) {
    previous->next = NULL;
  }

  return *current;
}

LinkedListPtr getNodeAtIndex(LinkedListPtr *head, int index) {
  LinkedListPtr current = *head;
  int count = 0;
  while (current != NULL) {

    // break if we are at current index
    if (count == index)
      break;

    current = current->next;
    count++;
  }

  return current;
}

void printListInt(LinkedListPtr head) {
  printf("NULL->");
  while (head != NULL) {
    printf("%d->", *(int *)head->data);
    head = head->next;
  }
  printf("NULL");
}

int test() {
  LinkedListPtr head = NULL;

  int value = 10;
  void *vptr = &value;

  insertAtBeginning(&head, vptr);
  int n = 20;
  vptr = &n;
  insertAtBeginning(&head, vptr);
  int x = 30;
  vptr = &x;
  insertAtEnd(&head, vptr);

  LinkedListPtr node = getNodeAtIndex(&head, 1);
  deleteNode(&node);
  free(node);
  node = NULL;

  node = getNodeAtIndex(&head, 1);
  deleteNode(&node);
  free(node);
  node = NULL;

  printListInt(head);

  free(head);
  head = NULL;

  return EXIT_SUCCESS;
}
