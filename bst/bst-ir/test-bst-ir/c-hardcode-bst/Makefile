CC=clang++
FLAGS= -std=c++11 -g
COMPILER_FLAGS = `llvm-config --cflags`
LINKER_FLAGS = `llvm-config --cxxflags --ldflags --libs core executionengine mcjit interpreter analysis native bitwriter --system-libs`

basic_tree: basic_tree.o
	$(CC) $(FLAGS) basic_tree.o -o basic_tree $(LINKER_FLAGS)

basic_tree.o: basic_tree.c
	$(CC) $(FLAGS) $(COMPILER_FLAGS) -c basic_tree.c


clean:
	rm -rf basic_tree basic_tree.o basic_tree.bc basic_tree.ll