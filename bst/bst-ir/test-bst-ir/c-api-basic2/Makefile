CC=clang++
FLAGS= -std=c++11 #-g
COMPILER_FLAGS = `llvm-config --cflags`
LINKER_FLAGS = `llvm-config --cxxflags --ldflags --libs core executionengine mcjit interpreter analysis native bitwriter --system-libs`

sum_mul: sum_mul.o
	$(CC) $(FLAGS) sum_mul.o -o sum_mul $(LINKER_FLAGS)

sum_mul.o: sum_mul.c
	$(CC) $(FLAGS) $(COMPILER_FLAGS) -c sum_mul.c


clean:
	rm -rf sum_mul sum_mul.o sum_mul.bc sum_mul.ll