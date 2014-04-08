as -g $1 -o $1.o
ld $1.o -e _main -lc -macosx_version_min 10.8.5
rm $1.o

