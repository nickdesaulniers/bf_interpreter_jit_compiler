#include <stdio.h>
#include <assert.h>
#include "../stack.h"

int main () {
  struct stack stack = {
    .size = 0,
    .items = { 0 }
  };

  puts("testing STACKSIZE");
  assert(STACKSIZE == 100);

  puts("testing pushes");
  int rc = push(&stack, 1);
  assert(rc == 0);
  assert(stack.size == 1);
  assert(stack.items != NULL);
  assert(stack.items[0] == 1);

  rc = push(&stack, 2);
  assert(rc == 0);
  assert(stack.size == 2);
  assert(stack.items != NULL);
  assert(stack.items[0] == 1);
  assert(stack.items[1] == 2);

  puts("testing pops");
  int x = 0;
  rc = pop(&stack, &x);
  assert(rc == 0);
  assert(x == 2);
  assert(stack.size == 1);
  assert(stack.items != NULL);
  assert(stack.items[0] == 1);

  rc = pop(&stack, &x);
  assert(rc == 0);
  assert(x == 1);
  assert(stack.size == 0);
  assert(stack.items != NULL);

  puts("testing excessive pops");
  rc = pop(&stack, &x);
  assert(rc == -1);
  assert(x == 1);
  assert(stack.size == 0);
  assert(stack.items != NULL);

  puts("tests pass");
}
