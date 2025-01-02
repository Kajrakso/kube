#include "../include/libcube.h"
#include "../src/_cube.h"
#include "criterion/criterion.h"
#include "criterion/redirect.h"
#include <stdint.h>
#include <stdio.h>

// suits

TestSuite(fix_orientation);

// tests

Test(fix_orientation, fix_co_lr_ud_several_moves){
    cube_t cube1 = cube_create_new_cube();
    cube_t cube2 = cube_create_new_cube();

    cube_tables_generate();

    int moves[] = {U1, R1, F1, R2, D3, U2, L2};

    cube_move_apply_multiple_moves(&cube1, moves, 7);
    cube_move_apply_multiple_moves(&cube2, moves, 7);

    // set some incorrect orientations (but keep cofb correct)
    update_corner_orien(&cube1.corners[UBL], extract_corner_orien(cube1.corners[UBL], FB), 0, 0);
    update_corner_orien(&cube1.corners[UBR], extract_corner_orien(cube1.corners[UBR], FB), 0, 0);
    update_corner_orien(&cube1.corners[UFR], extract_corner_orien(cube1.corners[UFR], FB), 0, 0);
    update_corner_orien(&cube1.corners[UFL], extract_corner_orien(cube1.corners[UFL], FB), 0, 0);
    update_corner_orien(&cube1.corners[DFL], extract_corner_orien(cube1.corners[DFL], FB), 0, 0);
    update_corner_orien(&cube1.corners[DFR], extract_corner_orien(cube1.corners[DFR], FB), 0, 0);
    update_corner_orien(&cube1.corners[DBR], extract_corner_orien(cube1.corners[DBR], FB), 0, 0);
    update_corner_orien(&cube1.corners[DBL], extract_corner_orien(cube1.corners[DBL], FB), 0, 0);

    // fix the orientations
    fix_co_lr_ud(&cube1);

    cr_assert(cube_operation_is_equal(cube1, cube2));
}

Test(fix_orientation, fix_eo_lr_ud_several_moves){
    cube_t cube1 = cube_create_new_cube();
    cube_t cube2 = cube_create_new_cube();

    cube_tables_generate();

    int moves[] = {U1, R1, F1, R2, D3, U2, L2};

    cube_move_apply_multiple_moves(&cube1, moves, 7);
    cube_move_apply_multiple_moves(&cube2, moves, 7);

    // set some incorrect orientations (but keep cofb correct)
    update_edge_orien(&cube1.edges[UB], extract_edge_orien(cube1.edges[UB], FB), 0, 0);
    update_edge_orien(&cube1.edges[UR], extract_edge_orien(cube1.edges[UR], FB), 0, 0);
    update_edge_orien(&cube1.edges[UF], extract_edge_orien(cube1.edges[UF], FB), 0, 0);
    update_edge_orien(&cube1.edges[UL], extract_edge_orien(cube1.edges[UL], FB), 0, 0);
    update_edge_orien(&cube1.edges[DF], extract_edge_orien(cube1.edges[DF], FB), 0, 0);
    update_edge_orien(&cube1.edges[DR], extract_edge_orien(cube1.edges[DR], FB), 0, 0);
    update_edge_orien(&cube1.edges[DB], extract_edge_orien(cube1.edges[DB], FB), 0, 0);
    update_edge_orien(&cube1.edges[DL], extract_edge_orien(cube1.edges[DL], FB), 0, 0);
    update_edge_orien(&cube1.edges[BL], extract_edge_orien(cube1.edges[BL], FB), 0, 0);
    update_edge_orien(&cube1.edges[BR], extract_edge_orien(cube1.edges[BR], FB), 0, 0);
    update_edge_orien(&cube1.edges[FR], extract_edge_orien(cube1.edges[FR], FB), 0, 0);
    update_edge_orien(&cube1.edges[FL], extract_edge_orien(cube1.edges[FL], FB), 0, 0);

    // fix the orientations
    fix_eo_lr_ud(&cube1);

    cr_assert(cube_operation_is_equal(cube1, cube2));
}
