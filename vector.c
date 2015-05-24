#include <stdlib.h> // malloc, free, realloc
#include <string.h> // memcpy
#include "vector.h"

int vector_create (struct vector* const vec, int capacity) {
  vec->size = 0;
  vec->capacity = capacity;
  vec->data = malloc(capacity * sizeof(char));
  return vec->data == NULL ? -1 : 0;
}

int vector_destroy (struct vector* vec) {
  if (vec == NULL || vec->data == NULL) {
    return -1;
  }
  vec->size = 0;
  vec->capacity = 0;
  free(vec->data);
  vec->data = NULL;
  vec = NULL;
  return 0;
}

int vector_push (struct vector* const vec, char* bytes, int len) {
  if (vec->size + len > vec->capacity) {
    vec->capacity *= 2;
    vec->data = realloc(vec->data, vec->capacity * sizeof(char));
    if (vec->data == NULL) {
      return -1;
    }
  }
  memcpy(vec->data + vec->size, bytes, len);
  vec->size += len;
  return 0;
}

int vector_write32LE (struct vector* const vec, int offset, long int value) {
  if (offset >= vec->size) {
    return -1;
  }
  // offset opposite of usual since we explicitly want LE
  vec->data[offset + 3] = (value & 0xFF000000) >> 24;
  vec->data[offset + 2] = (value & 0x00FF0000) >> 16;
  vec->data[offset + 1] = (value & 0x0000FF00) >> 8;
  vec->data[offset + 0] = (value & 0x000000FF);
  return 0;
}

int vector_write64LE (struct vector* const vec, int offset, long long int value) {
   if (offset >= vec->size) {
    return -1;
  }
  // offset opposite of usual since we explicitly want LE
  vec->data[offset + 7] = (value & 0xFF00000000000000) >> 56;
  vec->data[offset + 6] = (value & 0x00FF000000000000) >> 48;
  vec->data[offset + 5] = (value & 0x0000FF0000000000) >> 40;
  vec->data[offset + 4] = (value & 0x000000FF00000000) >> 32;

  vec->data[offset + 3] = (value & 0x00000000FF000000) >> 24;
  vec->data[offset + 2] = (value & 0x0000000000FF0000) >> 16;
  vec->data[offset + 1] = (value & 0x000000000000FF00) >> 8;
  vec->data[offset + 0] = (value & 0x00000000000000FF);
  return 0;
}

