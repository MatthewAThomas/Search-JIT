CC=clang++
FLAGS= -std=c++11 -g
COMPILER_FLAGS = `llvm-config --cflags`
LINKER_FLAGS = `llvm-config --cxxflags --ldflags --libs core executionengine mcjit interpreter analysis native bitwriter --system-libs`

test: bst_ir.o test.o
	$(CC) $(FLAGS) -o test bst_ir.o test.o $(LINKER_FLAGS)

bst_ir.o: ../../bst_ir.c ../../bst_ir.h
	$(CC) $(FLAGS) $(COMPILER_FLAGS) -c ../../bst_ir.c

test.o: test.c
	$(CC) $(FLAGS) $(COMPILER_FLAGS) -c test.c


clean:
	rm -rf test bst_ir.o bst_ir.bc bst_ir.ll test.o has_val.bc has_val.ll