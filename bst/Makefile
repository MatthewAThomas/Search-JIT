CC=clang++
#COMPILER_FLAGS= -std=c++11 -g
COMPILER_FLAGS = -std=c++11 `llvm-config --cflags` #-g
LINKER_FLAGS = `llvm-config --cxxflags --ldflags --libs core executionengine mcjit interpreter analysis native bitwriter --system-libs`

all:benchmark

benchmark: benchmark.o bst_data.o bst_ir.o benchmark_data.o benchmark_queries.o
	$(CC) $(COMPILER_FLAGS) -o benchmark bst_ir.o benchmark.o bst_data.o benchmark_data.o benchmark_queries.o $(LINKER_FLAGS)

benchmark.o: benchmark.c
	$(CC) $(COMPILER_FLAGS) benchmark.c -c

bst_data.o: bst-data/bst_data.c bst-data/bst_data.h
	$(CC) $(COMPILER_FLAGS) bst-data/bst_data.c -c

bst_ir.o: bst-ir/bst_ir.c bst-ir/bst_ir.h
	$(CC) $(COMPILER_FLAGS) -c bst-ir/bst_ir.c

benchmark_data.o: benchmark-utils/data.c benchmark-utils/data.h
	$(CC) $(COMPILER_FLAGS) -c  benchmark-utils/data.c -o benchmark_data.o

benchmark_queries.o: benchmark-utils/queries.c benchmark-utils/queries.h
	$(CC) $(COMPILER_FLAGS) -c  benchmark-utils/queries.c -o benchmark_queries.o

clean:
	rm -rf benchmark *.o