#include "stack.h"

int push (struct stack* p, int x) {
  if (p->size == STACKSIZE) {
    return -1;
  } else {
    p->items[p->size++] = x;
    return 0;
  }
}

int pop (struct stack* p, int* x) {
  if (p->size == 0) {
    return -1;
  } else {
    *x = p->items[--p->size];
    return 0;
  }
}
