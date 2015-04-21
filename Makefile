# http://www.puxan.com/Computers/Howto-write-generic-Makefiles
CC=clang
CFLAGS=-O3 -Wall -Wextra

# Generic rule
# $< is first dependency filename
# $@ is the target filename
%.o: %.c
	$(CC) $(CFLAGS) $< -c -o $@

v1: file_io.o bf.o interpret.o
	$(CC) $(CFLAGS) file_io.o bf.o interpret.o -o interpret

v2: file_io.o stack.o compile.o
	$(CC) $(CFLAGS) file_io.o stack.o compile.o -o compile

convert: v2
	./compile nick.bf > test.s

test: convert
	as -g test.s -o test.o
	ld test.o -e _main -lc -macosx_version_min 10.8.5

stack: stack.o test_stack.o
	$(CC) $(CFLAGS) stack.o test_stack.o -o test_stack


clean:
	rm a.out main *.o interpret compile test_stack
