#include <assert.h> // assert
#include <stdio.h> // putchar, getchar
#include <stdlib.h> // free
#include <string.h> // memcpy
#include <sys/mman.h> // mmap
#include "vector.h"
#include "stack.h"
#include "file_io.h"
#include "debug.h"

// return 0 is success
#define GUARD(expr) assert(!(expr))

typedef void* fn_memset (void*, int, size_t);
typedef int fn_putchar (int);
typedef int fn_getchar ();

void jit (const char* const file_contents) {
  struct vector instruction_stream;
  struct stack relocation_table = { .size = 0, .items = { 0 } };
  int relocation_site = 0;
  int relative_offset = 0;
  GUARD(vector_create(&instruction_stream, 100));
  char prologue [] = {
    0x55, // push rbp
    0x48, 0x89, 0xE5, // mov rsp, rbp
    // backup callee saved registers
    0x41, 0x54, // pushq %r12
    0x41, 0x55, // pushq %r13
    0x41, 0x56, // pushq %r14
    // %rdi = memset
    // %rsi = putchar
    // %rdx = getgetchar
    // backup args to callee saved registers
    0x49, 0x89, 0xFC, // movq %rdi, %r12
    0x49, 0x89, 0xF5, // movq %rsi, %r13
    0x49, 0x89, 0xD6, // movq %rdx, %r14
    // %r12 = memset
    // %r13 = putchar
    // %r14 = getchar
    // allocate 30,000 B on stack
    0x48, 0x81, 0xEC, 0x30, 0x75, 0x00, 0x00, // subq $30000, %rsp
    // address of beginning of tape
    0x48, 0x8D, 0x3C, 0x24, // leaq (%rsp), %rdi
    // fill with 0's
    0x48, 0xC7, 0xC6, 0x00, 0x00, 0x00, 0x00, // movq $0, %rsi
    // length 30,000 B
    0x48, 0xC7, 0xC2, 0x30, 0x75, 0x00, 0x00, // movq $30000, %rdx
    // memset
    0x41, 0xFF, 0xD4, // callq *%r12
    0x49, 0x89, 0xE4 // movq %rsp, %r12
    // %r12 = &tape[0];
  };
  GUARD(vector_push(&instruction_stream, prologue, sizeof(prologue)));

  for (unsigned long i = 0; file_contents[i] != '\0'; ++i) {
    switch (file_contents[i]) {
      case '>':
        // see: http://stackoverflow.com/a/8550253/1027966
        {
          char opcodes [] = {
            0x49, 0xFF, 0xC4 // inc %r12
          };
          GUARD(vector_push(&instruction_stream, opcodes, sizeof(opcodes)));
        }
        break;
      case '<':
        {
          char opcodes [] = {
            0x49, 0xFF, 0xCC // dec %r12
          };
          GUARD(vector_push(&instruction_stream, opcodes, sizeof(opcodes)));
        }
        break;
      case '+':
        {
          char opcodes [] = {
            0x41, 0xFE, 0x04, 0x24 // incb (%r12)
          };
          GUARD(vector_push(&instruction_stream, opcodes, sizeof(opcodes)));
        }
        break;
      case '-':
        {
          char opcodes [] = {
            0x41, 0xFE, 0x0C, 0x24 // decv (%r12)
          };
          GUARD(vector_push(&instruction_stream, opcodes, sizeof(opcodes)));
        }
        break;
      case '.':
        {
          char opcodes [] = {
            0x48, 0xC7, 0xC7, 0x00, 0x00, 0x00, 0x00, // movq $0, %rdi
            0x41, 0x8A, 0x3C, 0x24, // movb (%r12), %dil
            0x41, 0xFF, 0xD5 // callq *%r13
          };
          GUARD(vector_push(&instruction_stream, opcodes, sizeof(opcodes)));
        }
        break;
      case ',':
        {
          char opcodes [] = {
            0x41, 0xFF, 0xD6, // callq *%r14
            0x41, 0x88, 0x04, 0x24 // movb %al, (%r12)
          };
          GUARD(vector_push(&instruction_stream, opcodes, sizeof(opcodes)));
        }
        break;
      case '[':
        {
          char opcodes [] = {
            0x41, 0x80, 0x3C, 0x24, 0x00, // cmpb $0, (%r12)
            // Needs to be patched up
            0x0F, 0x84, 0x00, 0x00, 0x00, 0x00 // je <32b relative offset, 2's compliment, LE>
          };
          GUARD(vector_push(&instruction_stream, opcodes, sizeof(opcodes)));
        }
        GUARD(stack_push(&relocation_table, instruction_stream.size)); // create a label after
        break;
      case ']':
        {
          char opcodes [] = {
            0x41, 0x80, 0x3C, 0x24, 0x00, // cmpb $0, (%r12)
            // Needs to be patched up
            0x0F, 0x85, 0x00, 0x00, 0x00, 0x00 // jne <32b relative offset, 2's compliment, LE>
          };
          GUARD(vector_push(&instruction_stream, opcodes, sizeof(opcodes)));
        }
        // patches self and matching open bracket
        GUARD(stack_pop(&relocation_table, &relocation_site));
        relative_offset = instruction_stream.size - relocation_site;
        vector_write32LE(&instruction_stream, instruction_stream.size - 4, -relative_offset);
        vector_write32LE(&instruction_stream, relocation_site - 4, relative_offset);
        break;
    }
  }

  char epilogue [] = {
    0x48, 0x81, 0xC4, 0x30, 0x75, 0x00, 0x00, // addq $30000, %rsp
    // restore callee saved registers
    0x41, 0x5E, // popq %r14
    0x41, 0x5D, // popq %r13
    0x41, 0x5C, // popq %r12
    0x5d, // pop rbp
    0xC3 // ret
  };
  GUARD(vector_push(&instruction_stream, epilogue, sizeof(epilogue)));
  /*print_instruction_stream(&instruction_stream);*/

  void* mem = mmap(NULL, instruction_stream.size, PROT_WRITE | PROT_EXEC,
    MAP_ANON | MAP_PRIVATE, -1, 0);
  memcpy(mem, instruction_stream.data, instruction_stream.size);
  void (*jitted_func) (fn_memset, fn_putchar, fn_getchar) = mem;
  jitted_func(memset, putchar, getchar);
  munmap(mem, instruction_stream.size);
  vector_destroy(&instruction_stream);
}

int main (int argc, char* argv []) {
  if (argc != 2) err("Usage: jit inputfile");
  char* file_contents = read_file(argv[1]);
  if (file_contents == NULL) err("Couldn't open file");
  jit(file_contents);
  free(file_contents);
}

