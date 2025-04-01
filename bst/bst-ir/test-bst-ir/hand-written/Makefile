CC=clang
FLAGS= -std=c99 # -g

basic_tree_static: basic_tree.ll
	clang basic_tree.ll -o basic_tree

basic_tree_jit: basic_tree.ll
	lli basic_tree.ll


two_level_tree_static: two_level_tree.ll
	clang two_level_tree.ll -o two_level_tree

two_level_tree_jit: two_level_tree.ll
	lli two_level_tree.ll


clean:
	rm -rf basic_tree two_level_tree