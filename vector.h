struct vector {
  int size;
  int capacity;
  char* data;
};

int vector_create (struct vector* const vec, int capacity);
int vector_destroy (struct vector* vec);
int vector_push (struct vector* const vec, char* bytes, int len);
int vector_write32LE (struct vector* const vec, int offset, int value);

