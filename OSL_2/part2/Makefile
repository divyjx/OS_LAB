all : twice half square
	 ./twice ./square ./half ./twice ./half 10
twice : twice.c
	@clang twice.c -o twice
half : half.c
	@clang half.c -o half
square : square.c
	@clang square.c -o square
clean:
	rm twice half square