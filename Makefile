file_io:
	clang file_io.c -c -o file_io.o

bf:
	clang bf.c -c -o bf.o

main:
	clang main.c -c -o main.o

v1: file_io bf main
	clang file_io.o bf.o main.o -o main

clean:
	rm a.out main *.o
