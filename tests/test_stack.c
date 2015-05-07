#include <stdio.h>
#include <assert.h>
#include "../stack.h"

// return 0 is success
#define GUARD(expr) assert(!(expr))

int main () {
  struct stack stack = { .size = 0, .items = { 0 } };

  puts("testing STACKSIZE");
  assert(STACKSIZE == 100);

  puts("testing pushes");
  GUARD(stack_push(&stack, 1));
  assert(stack.size == 1);
  assert(stack.items != NULL);
  assert(stack.items[0] == 1);

  GUARD(stack_push(&stack, 2));
  assert(stack.size == 2);
  assert(stack.items != NULL);
  assert(stack.items[0] == 1);
  assert(stack.items[1] == 2);

  puts("testing pops");
  int x = 0;
  GUARD(stack_pop(&stack, &x));
  assert(x == 2);
  assert(stack.size == 1);
  assert(stack.items != NULL);
  assert(stack.items[0] == 1);

  GUARD(stack_pop(&stack, &x));
  assert(x == 1);
  assert(stack.size == 0);
  assert(stack.items != NULL);

  puts("testing excessive pops");
  int rc = stack_pop(&stack, &x);
  assert(rc == -1);
  assert(x == 1);
  assert(stack.size == 0);
  assert(stack.items != NULL);

  puts("tests pass");
}

