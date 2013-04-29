#include "file_io.h"
#include "bf.h"

int main (int argc, char* argv []) {
  if (argc != 2) err("Usage: main inputfile");

  FILE* fp = fopen(argv[1], "r");
  if (fp == NULL) err("Couldn't open file");

  char* file_contents = read_file(fp);
  interpret(file_contents);

  free(file_contents);
  fclose(fp);
}
