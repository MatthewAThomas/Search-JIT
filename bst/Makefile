CC=clang
FLAGS= -std=c99 # -g

all:benchmark

benchmark: benchmark.o bst_data.o
	$(CC) $(FLAGS) -o benchmark benchmark.o bst_data.o

benchmark.o: benchmark.c
	$(CC) $(FLAGS) benchmark.c -c

bst_data.o: bst-data/bst_data.c bst-data/bst_data.h
	$(CC) $(FLAGS) bst-data/bst_data.c -c

clean:
	rm -rf bst_data.o benchmark.o benchmark