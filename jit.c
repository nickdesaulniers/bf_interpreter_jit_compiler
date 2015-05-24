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
    // backup args to callee saved registers
    0x49, 0xBC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // movq $x, %r12
    0x49, 0xBD, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // movq $x, %r13
    0x49, 0xBE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // movq $x, %r14
  };
  GUARD(vector_push(&instruction_stream, prologue, sizeof(prologue)));
  GUARD(vector_write64LE(&instruction_stream, instruction_stream.size - 28,
        (long long int) &memset));
  GUARD(vector_write64LE(&instruction_stream, instruction_stream.size - 18,
        (long long int) &putchar));
  GUARD(vector_write64LE(&instruction_stream, instruction_stream.size - 8,
        (long long int) &getchar));
  // %r12 = memset
  // %r13 = putchar
  // %r14 = getchar

  char prologue2 [] = {
    // allocate 30,008 B on stack
    0x48, 0x81, 0xEC, 0x38, 0x75, 0x00, 0x00, // subq $30000, %rsp
    // address of beginning of tape
    0x48, 0x8D, 0x3C, 0x24, // leaq (%rsp), %rdi
    // fill with 0's
    0xBE, 0x00, 0x00, 0x00, 0x00, // movl $0, %esi
    // length 30,000 B
    0x48, 0xC7, 0xC2, 0x30, 0x75, 0x00, 0x00, // movq $30000, %rdx
    // memset
    0x41, 0xFF, 0xD4, // callq *%r12
    0x49, 0x89, 0xE4 // movq %rsp, %r12
    // %r12 = &tape[0];
  };
  GUARD(vector_push(&instruction_stream, prologue2, sizeof(prologue2)));

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
            0x41, 0x0F, 0xB6, 0x3C, 0x24, // movzbl (%r12), %edi
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
    0x48, 0x81, 0xC4, 0x38, 0x75, 0x00, 0x00, // addq $30008, %rsp
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
  void (*jitted_func) () = mem;
  jitted_func();
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

