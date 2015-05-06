#include <assert.h>
#include <stdio.h>
#include "../vector.h"

int main () {
  struct vector vec;

  puts("testing vector creation");
  int rc = vector_create(&vec, 4);
  assert(rc == 0);
  assert(vec.size == 0);
  assert(vec.capacity == 4);
  assert(vec.data != NULL);

  puts("testing initial non-resizing push");
  rc = vector_push(&vec, (char [2]) { 0xF1, 0xF2 }, 2);
  assert(rc == 0);
  assert(vec.size == 2);
  assert(vec.capacity == 4);
  assert(vec.data != NULL);
  assert(vec.data[0] == (char) 0xF1);
  assert(vec.data[1] == (char) 0xF2);

  puts("testing resizing push");
  rc = vector_push(&vec, (char [4]) { 0x55, 0x54, 0x22, 0x3A }, 4);
  assert(rc == 0);
  assert(vec.size == 6);
  assert(vec.capacity == 8);
  assert(vec.data != NULL);
  assert(vec.data[0] == (char) 0xF1);
  assert(vec.data[1] == (char) 0xF2);
  assert(vec.data[2] == (char) 0x55);
  assert(vec.data[3] == (char) 0x54);
  assert(vec.data[4] == (char) 0x22);
  assert(vec.data[5] == (char) 0x3A);

  puts("testing vector destruction");
  rc = vector_destroy(&vec);
  assert(rc == 0);
  assert(vec.data == NULL);
  assert(vec.size == 0);
  assert(vec.capacity == 0);

  puts("testing write32LE failure");
  // should be able to reuse destroyed vector
  rc = vector_create(&vec, 2); // 2 B
  assert(rc == 0);
  rc = vector_write32LE(&vec, 0, 20);
  assert(rc == -1);
  assert(vec.size == 0);
  assert(vec.capacity == 2);
  rc = vector_destroy(&vec);
  assert(rc == 0);

  puts("testing writing 32 b little endian 2's compliment");
  rc = vector_create(&vec, 10);
  assert(rc == 0);
  rc = vector_push(&vec, (char[]) { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }, 10);
  assert(rc == 0);
  rc = vector_write32LE(&vec, 2, 10);
  assert(rc == 0);
  assert(vec.data[2] == 0x0A);
  assert(vec.data[3] == 0x00);
  assert(vec.data[4] == 0x00);
  assert(vec.data[5] == 0x00);
  rc = vector_write32LE(&vec, 4, -32);
  assert(rc == 0);
  assert(vec.data[2] == 0x0A);
  assert(vec.data[3] == 0x00);
  assert(vec.data[4] == (char) 0xE0);
  assert(vec.data[5] == (char) 0xFF);
  assert(vec.data[6] == (char) 0xFF);
  assert(vec.data[7] == (char) 0xFF);
  rc = vector_destroy(&vec);
  assert(rc == 0);


  puts("tests pass");
}
