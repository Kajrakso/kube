cdef extern from "libcube.h":
    ctypedef struct cube_t:
        pass
    
    cube_t cube_create_new_cube()
    
    void cube_tables_generate() # todo! where to gen tables?

    void cube_move_apply_move_string(cube_t* cube, const char* moves)
    bint cube_solvers_solve_cube(cube_t cube, int number_of_solutions, int* banned_moves, int num_banned_moves)

    void cube_print_cube(cube_t* cube)