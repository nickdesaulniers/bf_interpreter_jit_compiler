#include "stack.h"

int stack_push (struct stack* const p, const int x) {
  if (p->size == STACKSIZE) {
    return -1;
  } else {
    p->items[p->size++] = x;
    return 0;
  }
}

int stack_pop (struct stack* const p, int* x) {
  if (p->size == 0) {
    return -1;
  } else {
    *x = p->items[--p->size];
    return 0;
  }
}

