#include <stdlib.h> // malloc, free, realloc
#include <string.h> // memcpy
#include <stdint.h> // int32_t
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

int vector_write32LE (struct vector* const vec, int offset, int32_t value) {
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

