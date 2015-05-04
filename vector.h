struct vector {
  int size;
  int capacity;
  char* data;
};

int vector_create (struct vector* vec, int capacity);
int vector_destroy (struct vector* vec);
int vector_push (struct vector* vec, char* bytes, int len);

