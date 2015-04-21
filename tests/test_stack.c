#include "../stack.h"

int main () {
  n_stack_t stack = {
    .size = 0,
    .items = {0}
  };

  push(&stack, 1);
  push(&stack, 2);
  push(&stack, 3);

  int x = 0;
  if (pop(&stack, &x) == 0) {
    printf("%d\n", x);
  }
  if (pop(&stack, &x) == 0) {
    printf("%d\n", x);
  }
}
