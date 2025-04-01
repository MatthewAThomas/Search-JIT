CC=clang++
FLAGS= -std=c++11 -g
COMPILER_FLAGS = `llvm-config --cflags`
LINKER_FLAGS = `llvm-config --cxxflags --ldflags --libs core executionengine mcjit interpreter analysis native bitwriter --system-libs`

sum: sum.o
	$(CC) $(FLAGS) sum.o -o sum $(LINKER_FLAGS)

sum.o: sum.c
	$(CC) $(FLAGS) $(COMPILER_FLAGS) -c sum.c


clean:
	rm -rf sum sum.o sum.bc sum.ll