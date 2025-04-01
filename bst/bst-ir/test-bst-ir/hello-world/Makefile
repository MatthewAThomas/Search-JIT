CC=clang
FLAGS= -std=c99 # -g

hello_world_jit: hello_world.ll
	lli hello_world.ll


hello_world_static: hello_world

hello_world: hello_world.ll
	clang hello_world.ll -o hello_world


clean:
	rm -rf hello_world