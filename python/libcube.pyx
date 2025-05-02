import cython.cimports.libcube as libcube
# import cython

move_set = set(["R", "L", "U", "D", "F", "B"])

move_map = {
  "U": 0,
  "D": 3,
  "L": 6,
  "R": 9,
  "F": 12,
  "B": 15,
}


def solve(scramble: str, number_of_solutions: int, generators: set[str]) -> None:
    assert number_of_solutions >= 1, "number_of_solutions has to be >= 1"
    assert scramble != "", "you have to provide a scramble"

    py_byte_scramble = scramble.encode('UTF-8')
    cdef char* c_scramble = py_byte_scramble

    libcube.cube_tables_generate()

    c = libcube.cube_create_new_cube()
    
    # scramble the cube
    cdef libcube.cube_t * c_p = &c
    libcube.cube_move_apply_move_string(c_p, c_scramble)
    
    banned = list(map(lambda x: move_map[x], list(move_set.difference(generators))))

    cdef int[6] c_banned
    for i in range(len(banned)):
        c_banned[i] = banned[i]

    # solve
    libcube.cube_solvers_solve_cube(c, number_of_solutions, c_banned, len(banned))