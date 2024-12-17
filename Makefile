CC=gcc
CFLAGS=-Wall -g

print_cube:
	$(CC) $(CFLAGS) src/*.c -o ./a.out && ./a.out

test:
	$(CC) $(CFLAGS) src/cube.c src/utilities.c src/moves.c tests/*.c -lcriterion -o ./a.out && ./a.out --verbose=1

clean:
	rm ./a.out
