import cython.cimports.libcube as libcube

move_set = set(["R", "L", "U", "D", "F", "B"])

move_map = {
  "U": 0,
  "D": 3,
  "L": 6,
  "R": 9,
  "F": 12,
  "B": 15,
}

move_notation = [
  "U", "U2", "U'",
  "D", "D2", "D'",
  "L", "L2", "L'",
  "R", "R2", "R'",
  "F", "F2", "F'",
  "B", "B2", "B'",
]


def solver_env(func):
  
    def wrapper(*args, **kwargs):
        libcube.cube_tables_generate()
        libcube.cube_tables_load()

        func(*args, **kwargs)

        libcube.cube_tables_free()

    return wrapper


def solutions_to_list(solutions, number_of_solutions):
    sols = []
    for i in range(number_of_solutions):
        sol = ""
        for j in range(20):
            if 0 <= solutions[i*20 + j] < 18:
                sol += move_notation[solutions[i*20 + j]] + " "
        sols += [sol.strip()]
    return sols


def solve(scramble: str, number_of_solutions: int = 1, generators: set[str] = move_set) -> list:
    assert number_of_solutions >= 1, "number_of_solutions has to be >= 1"
    assert number_of_solutions <= 1000, "number_of_solutions has to be <= 1000"
    assert scramble != "", "you have to provide a scramble"

    py_byte_scramble = scramble.encode('UTF-8')
    cdef char* c_scramble = py_byte_scramble

    c = libcube.cube_create_new_cube()
    
    # scramble the cube
    cdef libcube.cube_t * c_p = &c
    libcube.cube_move_apply_move_string(c_p, c_scramble)
    
    banned = list(map(lambda x: move_map[x], list(move_set.difference(generators))))

    cdef int[6] c_banned
    for i in range(len(banned)):
        c_banned[i] = banned[i]

    # prepare array for solutions
    cdef int[20*1000] solutions

    libcube.cube_solvers_solve_cube(c, solutions, number_of_solutions, c_banned, len(banned))
    
    return solutions_to_list(solutions, number_of_solutions)
