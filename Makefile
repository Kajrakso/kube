CC=gcc
CFLAGS=-Wall -g

print_cube:
	$(CC) $(CFLAGS) src/*.c -o ./a.out && ./a.out

test:
	$(CC) $(CFLAGS) src/cube.c src/utilities.c src/move.c src/move_tables.c tests/*.c -lcriterion -o ./a.out && ./a.out --verbose=1

clean:
	rm ./a.out
