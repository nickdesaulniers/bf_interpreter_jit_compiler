#include "file_io.h"

void err (const char* const msg) {
  fprintf(stderr, "%s\n", msg);
  exit(1);
}

char* read_file (FILE* fp) {
  long file_size = 0;
  size_t code_size;
  fseek(fp, 0, SEEK_END);
  file_size = ftell(fp);
  rewind(fp);
  code_size = sizeof(char) * file_size;
  char* code = malloc(code_size);
  fread(code, 1, file_size, fp);
  return code;
}

