// https://gist.github.com/maxcountryman/1699708
// http://www.muppetlabs.com/~breadbox/bf/
#include "bf.h"

void interpret (const char* const input) {
  // Initialize the tape with 30,000 zeroes.
  unsigned char tape [3000] = {0};

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
