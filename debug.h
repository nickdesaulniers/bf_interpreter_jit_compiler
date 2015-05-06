#include <stdio.h>
void print_instruction_stream (struct vector* v) {
  printf("instruction stream size: %d B\n", v->size);
  for (int i = 0; i < v->size; ++i) {
      if (i % 8 == 0 && i != 0) {
            puts("");
          }
      printf("0x%02hhX ", v->data[i]);
    }
  puts("");
}

void print_executable (char* mem, int len) {
  printf("executable len: %d B\n", len);
  for (int i = 0; i < len; ++i) {
      if (i % 8 == 0 && i != 0) {
            puts("");
          }
      printf("0x%02hhX ", mem[i]);
    }
  puts("");
}

