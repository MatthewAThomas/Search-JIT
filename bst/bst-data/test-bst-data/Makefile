CC=clang
FLAGS= -std=c99 # -g

all:test

test: test.o bst_data.o
	$(CC) $(FLAGS) -o test test.o bst_data.o

test.o: test.c ../bst_data.h
	$(CC) $(FLAGS) test.c -c

bst_data.o: ../bst_data.c ../bst_data.h
	$(CC) $(FLAGS) ../bst_data.c -c

clean:
	rm -rf bst_data.o test.o test