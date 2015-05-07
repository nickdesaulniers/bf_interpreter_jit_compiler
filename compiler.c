#include <stdio.h> // puts, fprintf, stderr
#include <stdlib.h> // free
#include "file_io.h"
#include "stack.h"

void compile (const char* const file_contents) {
  int num_brackets = 0;
  int matching_bracket = 0;
  struct stack stack = {
    .size = 0,
    .items = {0}
  };
  const char* const prolog =
    ".text\n"
    ".globl _main\n"
    "_main:\n"
    "  pushq %rbp\n"
    "  movq %rsp, %rbp\n"
    "  subq $30000, %rsp\n" // allocate 30,000 B on stack
    "  leaq (%rsp), %rdi\n" // address of beginning of tape
    "  movq $0, %rsi\n"     // fill with 0's
    "  movq $30000, %rdx\n" // length 30,000 B
    "  call _memset\n"      // memset
    "  movq %rsp, %r12";
  puts(prolog);

  for (unsigned long i = 0; file_contents[i] != '\0'; ++i) {
    fprintf(stderr, "%c", file_contents[i]);
    switch (file_contents[i]) {
      case '>':
        puts("  inc %r12");
        break;
      case '<':
        puts("  dec %r12");
        break;
      case '+':
        puts("  incb (%r12)");
        break;
      case '-':
        puts("  decb (%r12)");
        break;
      case '.':
        // zero out the register since putchar takes an int
        // otherwise use movzbq
        puts("  movq $0, %rdi");
        puts("  movb (%r12), %dil");
        puts("  call _putchar");
        break;
      case ',':
        puts("  call _getchar");
        puts("  movb %al, (%r12)");
        break;
      case '[':
        if(stack_push(&stack, num_brackets) == 0) {
          puts("  cmpb $0, (%r12)");
          printf("  je bracket_%d_end\n", num_brackets);
          printf("bracket_%d_start:\n", num_brackets++);
        } else {
          err("out of stack space, too much nesting");
        }
        break;
      case ']':
        if (stack_pop(&stack, &matching_bracket) == 0) {
          puts("  cmpb $0, (%r12)");
          printf("  jne bracket_%d_start\n", matching_bracket);
          printf("bracket_%d_end:\n", matching_bracket);
        } else {
          err("stack underflow, unmatched brackets");
        }
        break;
    }
  }
  const char* const epilog =
    "  addq $30000, %rsp\n" // clean up tape from stack.
    "  popq %rbp\n"
    "  ret\n";
  puts(epilog);
}

int main (int argc, char* argv []) {
  if (argc != 2) err("Usage: compile inputfile");
  char* file_contents = read_file(argv[1]);
  if (file_contents == NULL) err("Unable to read file");
  compile(file_contents);
  free(file_contents);
}
