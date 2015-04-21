// https://gist.github.com/maxcountryman/1699708
// http://www.muppetlabs.com/~breadbox/bf/
#include "file_io.h"

void interpret (const char* const input) {
  // Initialize the tape with 30,000 zeroes.
  unsigned char tape [30000] = {0};

  // Set the pointer to point at the left most cell of the tape.
  unsigned char* ptr = tape;

  char current_char;
  for (int i = 0, loop; (current_char = input[i]) != '\0'; ++i) {
    switch (current_char) {
      case '>': ++ptr; break;
      case '<': --ptr; break;
      case '+': ++(*ptr); break;
      case '-': --(*ptr); break;
      case '.': putchar(*ptr); break;
      case ',': *ptr = getchar(); break;
      case '[':
        if (!(*ptr)) {
          loop = 1;
          while (loop > 0) {
            current_char = input[++i];
            if (current_char == ']') {
              --loop;
            } else if (current_char == '[') {
              ++loop;
            }
          }
        }
        break;
      case ']':
        if (*ptr) {
          loop = 1;
          while (loop > 0) {
            current_char = input[--i];
            if (current_char == '[') {
              --loop;
            } else if (current_char == ']') {
              ++loop;
            }
          }
        }
        break;
    }
  }
}

int main (int argc, char* argv []) {
  if (argc != 2) err("Usage: interpret inputfile");

  FILE* fp = fopen(argv[1], "r");
  if (fp == NULL) err("Couldn't open file");

  char* file_contents = read_file(fp);
  interpret(file_contents);

  free(file_contents);
  fclose(fp);
}

