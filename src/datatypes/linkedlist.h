#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

typedef struct node {
  void *data;
  struct node *next;
  struct node *previous;
} node_t;

typedef node_t *LinkedListPtr;

/*Insert at beginning of list*/
void insertAtBeginning(LinkedListPtr *head, void *value);
/*insert at end of list*/
void insertAtEnd(LinkedListPtr *head, void *value);
/*return value at index of list, if outside returns null*/
void *getValueAtIndex(LinkedListPtr *head, int index);
/*returns node at index, if outside return null*/
LinkedListPtr getNodeAtIndex(LinkedListPtr *head, int index);
/*removes node from list, returns current so it can be freed*/
LinkedListPtr deleteNode(LinkedListPtr *current);

// for testing

/*print list for int types*/
void printListInt(LinkedListPtr head);

/*simple test using ints*/
int test();

#endif // !linkedlist_h_
