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

