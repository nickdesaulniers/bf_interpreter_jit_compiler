#include <stdio.h>
#define STACKSIZE 100

struct stack {
  size_t size;
  int items [STACKSIZE];
};

int push (struct stack*, int);
int pop (struct stack*, int*);

