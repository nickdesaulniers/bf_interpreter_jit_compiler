#include <assert.h>
#include "../stack.h"

int main () {
  struct stack stack = {
    .size = 0,
    .items = {0}
  };

  push(&stack, 1);
  push(&stack, 2);
  push(&stack, 3);

  int x = 0;
  if (pop(&stack, &x) == 0) {
    printf("%d\n", x);
    assert(x == 3);
  }
  if (pop(&stack, &x) == 0) {
    printf("%d\n", x);
    assert(x == 2);
  }
}
