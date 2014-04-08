#include <stdio.h>
#define STACKSIZE 100

typedef struct {
  size_t size;
  int items [STACKSIZE];
} n_stack_t;

int push (n_stack_t*, int);
int pop (n_stack_t*, int*);

