CC=clang++
FLAGS= -std=c++11 -g
COMPILER_FLAGS = `llvm-config --cflags`
LINKER_FLAGS = `llvm-config --cxxflags --ldflags --libs core executionengine mcjit interpreter analysis native bitwriter --system-libs`

all:benchmark

benchmark: benchmark.o bst_data.o bst_ir.o benchmark_data.o
	$(CC) $(FLAGS) -o benchmark benchmark.o bst_data.o bst_ir.o benchmark_data.o $(LINKER_FLAGS)

benchmark.o: benchmark.c
	$(CC) $(FLAGS) benchmark.c -c

bst_data.o: bst-data/bst_data.c bst-data/bst_data.h
	$(CC) $(FLAGS) bst-data/bst_data.c -c

bst_ir.o: bst-ir/bst_ir.c bst-ir/bst_ir.h
	$(CC) $(FLAGS) $(COMPILER_FLAGS) -c bst-ir/bst_ir.c

benchmark_data.o: benchmark-utils/data.c benchmark-utils/data.h
	$(CC) $(FLAGS) -c  benchmark-utils/data.c -o benchmark_data.o

clean:
	rm -rf benchmark *.o