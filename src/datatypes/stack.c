#include "stack.h"
#include <malloc.h>
#include <stdlib.h>

struct Node *top;

void push(void *value) {
  struct Node *temp = malloc(sizeof(struct Node));

  if (temp != NULL) {
    temp->data = value;
    temp->link = top;
    top = temp;
  }
}

void pop() {
  struct Node *temp;
  if (top != NULL) {
    temp = top;
    top = top->link;
    temp->link = NULL;

    free(temp);
  }
}

int isEmpty() { return top == NULL; }

// just for testing some stuff
void displayInt() {
  struct Node *temp;

  if (top != NULL) {
    temp = top;
    while (temp != NULL) {
      printf("%d:\n", *(int *)temp->data);
      temp = temp->link;
    }
  }
}

void step(int a, int b) {
  int (*fptr)(int, int) = top->data;
  int r = fptr(a, b);
  printf(":%d\n", r);
  pop();
}

int multiply(int a, int b) { return a * b; }
int add(int a, int b) { return a + b; }
int subtract(int a, int b) { return a - b; }

int test() {

  void *ptr[] = {&multiply, &add, &add, &subtract};
  push(ptr[0]);
  push(ptr[1]);
  push(ptr[2]);
  push(ptr[3]);

  step(10, 10); // 0 substract!
  step(10, 20); // 30 add!
  step(10, 50); // 60 add!
  step(10, 20); // 200 multiply!

  return EXIT_SUCCESS;
}
