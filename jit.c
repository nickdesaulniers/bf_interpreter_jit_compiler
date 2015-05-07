#include <stdio.h> // putchar, getchar
#include <string.h> // memcpy
#include <sys/mman.h> // mmap
#include "vector.h"
#include "stack.h"
#include "file_io.h"
#include "debug.h"

#define GUARD(expr) if ((expr) != 0) err("Failed guard\n");

typedef void* fn_memset (void*, int, size_t);
typedef int fn_putchar (int);
typedef int fn_getchar ();

static void jit (const char* const file_contents, fn_memset m, fn_putchar p,
                 fn_getchar g) {
  struct vector instruction_stream;
  struct stack relocation_table = { .size = 0, .items = { 0 } };
  int relocation_site = 0;
  int relative_offset = 0;
  GUARD(vector_create(&instruction_stream, 100));
  GUARD(vector_push(&instruction_stream, (char []) {
    // prolog
    0x55, // push rbp
    0x48, 0x89, 0xE5, // mov rbp, rsp
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
    0x4C, 0x89, 0xE1, // movq %r12, %rcx
    0xFF, 0xD1, // callq *%rcx
    0x49, 0x89, 0xE4 // movq %rsp, %r12
    // %r12 = &tape[0];
  }, 52));
  /*print_instruction_stream(&instruction_stream);*/

  for (unsigned long i = 0; file_contents[i] != '\0'; ++i) {
    /*fprintf(stderr, "%c", file_contents[i]);*/
    switch (file_contents[i]) {
      case '>':
        GUARD(vector_push(&instruction_stream, (char[]) {
          0x49, 0xFF, 0xC4 // inc %r12
        }, 3));
        break;
      case '<':
        GUARD(vector_push(&instruction_stream, (char[]) {
          0x49, 0xFF, 0xCC // dec %r12
        }, 3));
        break;
      case '+':
        GUARD(vector_push(&instruction_stream, (char[]) {
          0x41, 0xFE, 0x04, 0x24 // incb (%r12)
        }, 4));
        break;
      case '-':
        GUARD(vector_push(&instruction_stream, (char[]) {
          0x41, 0xFE, 0x0C, 0x24 // decv (%r12)
        }, 4));
        break;
      case '.':
        GUARD(vector_push(&instruction_stream, (char[]) {
          0x48, 0xC7, 0xC7, 0x00, 0x00, 0x00, 0x00, // movq $0, %rdi
          0x41, 0x8A, 0x3C, 0x24, // movb (%r12), %dil
          0x41, 0xFF, 0xD5 // callq *%r13
        }, 14));
        break;
      case ',':
        GUARD(vector_push(&instruction_stream, (char[]) {
          0x41, 0xFF, 0xD6, // callq *%r14
          0x41, 0x88, 0x04, 0x24 // movb %al, (%r12
        }, 7));
        break;
      case '[':
        printf("open bracket, instruction_stream.size: %d\n", instruction_stream.size);
        GUARD(vector_push(&instruction_stream, (char[]) {
          0x41, 0x80, 0x3C, 0x24, 0x00, // cmpb $0, (%r12)
          // Needs to be patched up
          0x0F, 0x84, 0x00, 0x00, 0x00, 0x00 // je <32b relative offset, 2's compliment, LE>
        }, 11));
        GUARD(push(&relocation_table, instruction_stream.size)); // create a label after
        break;
      case ']':
        printf("close bracket, instruction_stream.size before: %d\n", instruction_stream.size);
        GUARD(vector_push(&instruction_stream, (char[]) {
          0x41, 0x80, 0x3C, 0x24, 0x00, // cmpb $0, (%r12)
          // Needs to be patched up
          0x0F, 0x85, 0x00, 0x00, 0x00, 0x00 // jne <33b relative offset, 2's compliment, LE>
        }, 11));
        printf("close bracket, instruction_stream.size after: %d\n", instruction_stream.size);
        GUARD(pop(&relocation_table, &relocation_site));
        printf("matching relocation site: %d\n", relocation_site);
        relative_offset = instruction_stream.size - relocation_site ;
        printf("relative backwards offset: %d\n", -relative_offset);
        // set 4 bytes to 2's compliment little endian
        vector_write32LE(&instruction_stream, instruction_stream.size - 4, -relative_offset);
        // patch up [: set 4 bytes to 2's compliment little endian
        printf("relative forwards offset: %d\n", relative_offset);
        vector_write32LE(&instruction_stream, relocation_site - 4, relative_offset);
        break;
    }
  }

  GUARD(vector_push(&instruction_stream, (char []) {
    // epilog
    0x48, 0x81, 0xC4, 0x30, 0x75, 0x00, 0x00, // addq $30000, %rsp
    // restore callee saved registers
    0x41, 0x5E, // popq %r14
    0x41, 0x5D, // popq %r13
    0x41, 0x5C, // popq %r12
    0x5d, // pop rbp
    0xC3 // ret
  }, 15));
  print_instruction_stream(&instruction_stream);

  void* mem = mmap(NULL, instruction_stream.size, PROT_WRITE | PROT_EXEC,
    MAP_ANON | MAP_PRIVATE, -1, 0);

  memcpy(mem, instruction_stream.data, instruction_stream.size);

  void (*jitted_func) (fn_memset, fn_putchar, fn_getchar) = mem;

  jitted_func(m, p, g);

  jitted_func = NULL;
  munmap(mem, instruction_stream.size);
  vector_destroy(&instruction_stream);
}

int main (int argc, char* argv []) {
  if (argc != 2) err("Usage: jit inputfile");
  FILE* fp = fopen(argv[1], "r");
  if (fp == NULL) err("Couldn't open file");
  const char* const file_contents = read_file(fp);
  jit(file_contents, memset, putchar, getchar);
  free((void*) file_contents);
  fclose(fp);
}

