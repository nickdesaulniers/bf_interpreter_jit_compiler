#define STACKSIZE 100

struct stack {
  int size;
  int items [STACKSIZE];
};

int push (struct stack* const, const int);
int pop (struct stack* const, int*);

