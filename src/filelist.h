#ifndef FILELIST_H_
#define FILELIST_H_

typedef struct node {
  char *data;
  struct node *nxtPtr;
} node_t;

typedef node_t *ListNodePtr;

void insert(ListNodePtr *head, char *value);
void insertAtEnd(ListNodePtr *head, char *value);
void insertAtStart(ListNodePtr *head, char *value);
void printList(ListNodePtr head);
void freeList(ListNodePtr head);

#endif // !FILELIST_H_
