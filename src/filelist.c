#include "filelist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// // could use void pointers to make this a bit more generic..
// int main(int argc, char *argv[]) {
//   ListNodePtr head = NULL;
//
//   insert(&head, "1_hello");
//   insert(&head, "4_hello");
//   insert(&head, "hello");
//   insert(&head, "hey");
//   printList(head);
//   freeList(head);
//
//   return EXIT_SUCCESS;

/* this will try to insert in correct order */
void insert(ListNodePtr *head, char *value) {
  int intVal = 0;
  intVal = atoi(value);

  ListNodePtr currentPtr = *head;

  if (currentPtr != NULL) {
    int checkInt = 0;
    checkInt = atoi(currentPtr->data);
    if (checkInt < intVal) {
      insertAtStart(head, value);
    } else {
      while (currentPtr->nxtPtr != NULL) {
        checkInt = atoi(currentPtr->nxtPtr->data);
        if (checkInt < intVal) {
          int len = strlen(value);
          ListNodePtr storePtr = currentPtr->nxtPtr;
          currentPtr->nxtPtr = NULL;
          currentPtr->nxtPtr = malloc(sizeof(node_t));
          currentPtr->nxtPtr->data = (char *)malloc(len + 1);
          strcpy(currentPtr->nxtPtr->data, value);
          currentPtr->nxtPtr->nxtPtr = storePtr;
          return;
        }
        currentPtr = currentPtr->nxtPtr;
      }

      // if we get here we just store it at end
      insertAtEnd(head, value);
    }

  } else {
    currentPtr = malloc(sizeof(node_t));
    int len = strlen(value);
    currentPtr->data = (char *)malloc(len + 1);
    strcpy(currentPtr->data, value);
    currentPtr->nxtPtr = NULL;
    *head = currentPtr;
  }
}

void freeList(ListNodePtr head) {
  ListNodePtr currentPtr = head;
  ListNodePtr tmpPtr = NULL;

  while (currentPtr) {
    free(currentPtr->data);
    currentPtr->data = NULL;
    tmpPtr = currentPtr;
    currentPtr = currentPtr->nxtPtr;
    free(tmpPtr);
    tmpPtr = NULL;
  }
}

void insertAtEnd(ListNodePtr *head, char *value) {
  ListNodePtr currentPtr = *head;

  if (currentPtr != NULL) {
    while (currentPtr->nxtPtr != NULL) {
      currentPtr = currentPtr->nxtPtr;
    }

    int len = strlen(value);
    currentPtr->nxtPtr = malloc(sizeof(node_t));
    currentPtr->nxtPtr->data = (char *)malloc(len + 1);
    strcpy(currentPtr->nxtPtr->data, value);
    currentPtr->nxtPtr->nxtPtr = NULL;
  } else {
    currentPtr = malloc(sizeof(node_t));
    int len = strlen(value);
    currentPtr->data = (char *)malloc(len + 1);
    strcpy(currentPtr->data, value);
    currentPtr->nxtPtr = NULL;
    *head = currentPtr;
  }
}

void insertAtStart(ListNodePtr *head, char *value) {
  ListNodePtr newPtr = malloc(sizeof(node_t));
  int len = strlen(value);
  newPtr->data = (char *)malloc(len + 1);
  strcpy(newPtr->data, value);
  newPtr->nxtPtr = *head;
  *head = newPtr;
}

void printList(ListNodePtr head) {
  ListNodePtr currentPtr = head;

  while (currentPtr) {
    printf("%s->", currentPtr->data);
    currentPtr = currentPtr->nxtPtr;
  }
  printf("NULL");
}
