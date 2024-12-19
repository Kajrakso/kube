CC = cc
CFLAGS = -std=c99 -pthread -pedantic -Wall -Wextra -Wno-unused-parameter -O3
DBGFLAGS = -std=c99 -pthread -pedantic -Wall -Wextra \
	-Wno-unused-parameter -Wno-unused-function \
	-fsanitize=address -fsanitize=undefined \
	-g3

OBJ = src/cube.o src/move_tables.o src/move.o src/utilities.o src/print_cube.o
TEST_OBJ = tests/cube_moves_tests.o tests/cube_repr_tests.o tests/util_tests.o

test: $(TEST_OBJ) libcube.a
		$(CC) $^ -lcriterion -o a.out && a.out --verbose=1

libcube.a: $(OBJ)
		ar rcs $@ $^

main: libcube.a
		$(CC) $(DBGFLAGS) main.c -L. -lcube -o $@

clean:
		rm -f *.a *.o src/*.a src/*.o tests/*.a tests/*.o *.out

.PHONY: test clean