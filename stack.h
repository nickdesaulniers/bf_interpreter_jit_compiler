#define STACKSIZE 100

struct stack {
  int size;
  int items [STACKSIZE];
};

int push (struct stack*, int);
int pop (struct stack*, int*);

