# http://www.puxan.com/Computers/Howto-write-generic-Makefiles
CC=clang
CFLAGS=-O3 -Wall -Wextra

# Generic rule
# $< is first dependency filename
# $@ is the target filename
%.o: %.c
	$(CC) $(CFLAGS) $< -c -o $@

v1: file_io.o bf.o main.o
	$(CC) $(CFLAGS) file_io.o bf.o main.o -o main

clean:
	rm a.out main *.o
