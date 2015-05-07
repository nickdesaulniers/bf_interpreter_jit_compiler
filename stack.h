#define STACKSIZE 100

struct stack {
  int size;
  int items [STACKSIZE];
};

int stack_push (struct stack* const, const int);
int stack_pop (struct stack* const, int*);

