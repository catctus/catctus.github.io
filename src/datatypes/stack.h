#ifndef STACK_H_
#define STACK_H_

struct Node {
  void *data;
  struct Node *link;
};

extern struct Node *top;
/*push onto stack*/
void push(void *value);
/*remove from stack, might want to return Node if data needs to be freed*/
void pop();
/* returns 1 if empty */
int isEmpty();
// for testing stuff started with simple ints
void displayInt();

// wanted to test function pointers
// in a stack, thought it could be nice
// excersie. just testing it out.
void step(int a, int b);
int multiply(int a, int b);
int add(int a, int b);
int subtract(int a, int b);
int test();

#endif // !STACK_H_
