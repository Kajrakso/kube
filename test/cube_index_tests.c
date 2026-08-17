#include "criterion/criterion.h"

#include "../src/index.h"
#include "../src/scrambler.h"

Test(cube_index, c_index_is_section) {
  for (uint32_t i = 0; i < 10000; i++) {
    cube_t c = c_index_to_cube(i);
    uint64_t j = cube_to_c_index(&c, UD);
    cr_assert_eq(i, j);
  }
}

Test(cube_index, H_lr_same_as_conj){
    for (int j = 0; j < 100; j++){
        cube_t c = cube_scrambler_get_handscrambled_cube();

        uint64_t i = cube_to_H_index(&c, LR);
        cube_t c2 = cube_operation_sym_conjugate(c, 13); // (y)^1 (y y z z z)^1
        uint64_t ii = cube_to_H_index(&c2, UD);

        cr_assert_eq(i, ii);
    }
}

Test(cube_index, H_fb_same_as_conj){
    for (int j = 0; j < 100; j++){
        cube_t c = cube_scrambler_get_handscrambled_cube();

        uint64_t i = cube_to_H_index(&c, FB);
        cube_t c2 = cube_operation_sym_conjugate(c, 10); // (y)^2 (y z)^2
        uint64_t ii = cube_to_H_index(&c2, UD);

        cr_assert_eq(i, ii);
    }
}


Test(cube_index, partial_e_index_preserved_E_slice){
    int edges[4] = {BL, BR, FR, FL}; 
    cube_t c = cube_create_new_cube();

    uint64_t i = cube_to_partial_e_index(&c, UD, edges, 4);

    cube_move_apply_move_string(&c, "U");
    cr_assert_eq(i, cube_to_partial_e_index(&c, UD, edges, 4));

    cube_move_apply_move_string(&c, "D");
    cr_assert_eq(i, cube_to_partial_e_index(&c, UD, edges, 4));

    cube_move_apply_move_string(&c, "U2");
    cr_assert_eq(i, cube_to_partial_e_index(&c, UD, edges, 4));

    cube_move_apply_move_string(&c, "R U R' U R U2 R'");
    cr_assert_eq(i, cube_to_partial_e_index(&c, UD, edges, 4));


    cube_t c2 = cube_create_new_cube();

    cube_move_apply_move_string(&c, "R U F B2 L D B U'");
    uint64_t i2 = cube_to_partial_e_index(&c2, UD, edges, 4);

    cube_move_apply_move_string(&c, "D");
    cr_assert_eq(i2, cube_to_partial_e_index(&c2, UD, edges, 4));

    cube_move_apply_move_string(&c, "D");
    cr_assert_eq(i2, cube_to_partial_e_index(&c2, UD, edges, 4));

    cube_move_apply_move_string(&c, "D");
    cr_assert_eq(i2, cube_to_partial_e_index(&c2, UD, edges, 4));

    cube_move_apply_move_string(&c, "L D2 L' F' D F");
    cr_assert_eq(i2, cube_to_partial_e_index(&c2, UD, edges, 4));
}


Test(cube_index, partial_e_index_preserved_D_layer){
    int edges[4] = {DF, DR, DB, DL}; 
    cube_t c = cube_create_new_cube();

    uint64_t i = cube_to_partial_e_index(&c, UD, edges, 4);

    cube_move_apply_move_string(&c, "U");
    cr_assert_eq(i, cube_to_partial_e_index(&c, UD, edges, 4));

    cube_move_apply_move_string(&c, "L' U L");
    cr_assert_eq(i, cube_to_partial_e_index(&c, UD, edges, 4));

    cube_move_apply_move_string(&c, "F' U2 F");
    cr_assert_eq(i, cube_to_partial_e_index(&c, UD, edges, 4));

    cube_move_apply_move_string(&c, "R U R' U R U2 R'");
    cr_assert_eq(i, cube_to_partial_e_index(&c, UD, edges, 4));


    cube_t c2 = cube_create_new_cube();

    cube_move_apply_move_string(&c, "R U F B2 L D B U'");
    uint64_t i2 = cube_to_partial_e_index(&c2, UD, edges, 4);

    cube_move_apply_move_string(&c, "U");
    cr_assert_eq(i2, cube_to_partial_e_index(&c2, UD, edges, 4));

    cube_move_apply_move_string(&c, "U");
    cr_assert_eq(i2, cube_to_partial_e_index(&c2, UD, edges, 4));

    cube_move_apply_move_string(&c, "U");
    cr_assert_eq(i2, cube_to_partial_e_index(&c2, UD, edges, 4));

    cube_move_apply_move_string(&c, "F U F' R U2 R' U");
    cr_assert_eq(i2, cube_to_partial_e_index(&c2, UD, edges, 4));
}


Test(cube_index, partial_e_index_preserved_order_of_edges){
    uint64_t i1, i2, i3, i4;
    {
        int edges[4] = {UL, UF, UR, UB}; 
        cube_t c = cube_create_new_cube();
        cube_move_apply_move_string(&c, "F U F' R U2 R' U");

        i1 = cube_to_partial_e_index(&c, UD, edges, 4);
    }
    {
        int edges[4] = {UF, UL, UR, UB}; 
        cube_t c = cube_create_new_cube();
        cube_move_apply_move_string(&c, "F U F' R U2 R' U");

        i2 = cube_to_partial_e_index(&c, UD, edges, 4);
    }
    {
        int edges[4] = {UF, UL, UB, UR}; 
        cube_t c = cube_create_new_cube();
        cube_move_apply_move_string(&c, "F U F' R U2 R' U");

        i3 = cube_to_partial_e_index(&c, UD, edges, 4);
    }
    {
        int edges[4] = {UB, UL, UR, UF}; 
        cube_t c = cube_create_new_cube();
        cube_move_apply_move_string(&c, "F U F' R U2 R' U");

        i4 = cube_to_partial_e_index(&c, UD, edges, 4);
    }

    cr_assert_eq(i1, i2);
    cr_assert_eq(i2, i3);
    cr_assert_eq(i3, i4);
}


Test(cube_index, partial_c_index_preserved_U_layer){
    int corners[4] = {UBL, UBR, UFR, UFL}; 
    cube_t c = cube_create_new_cube();

    uint64_t i = cube_to_partial_c_index(&c, UD, corners, 4);

    cube_move_apply_move_string(&c, "D");
    cr_assert_eq(i, cube_to_partial_c_index(&c, UD, corners, 4));

    cube_move_apply_move_string(&c, "D2");
    cr_assert_eq(i, cube_to_partial_c_index(&c, UD, corners, 4));

    cube_move_apply_move_string(&c, "R L' F R' L");
    cr_assert_eq(i, cube_to_partial_c_index(&c, UD, corners, 4));


    cube_t c2 = cube_create_new_cube();

    cube_move_apply_move_string(&c, "R U F B2 L D B U'");
    uint64_t i2 = cube_to_partial_c_index(&c2, UD, corners, 4);

    cube_move_apply_move_string(&c, "F2 R' F R F");
    cr_assert_eq(i2, cube_to_partial_c_index(&c2, UD, corners, 4));
}


Test(cube_index, partial_c_index_preserved_D_layer){
    int corners[4] = {DFL, DFR, DBR, DBL}; 
    cube_t c = cube_create_new_cube();

    uint64_t i = cube_to_partial_c_index(&c, UD, corners, 4);

    cube_move_apply_move_string(&c, "U");
    cr_assert_eq(i, cube_to_partial_c_index(&c, UD, corners, 4));

    cube_move_apply_move_string(&c, "R U R' U R U2 R'");
    cr_assert_eq(i, cube_to_partial_c_index(&c, UD, corners, 4));


    cube_t c2 = cube_create_new_cube();

    cube_move_apply_move_string(&c, "R U F B2 L D B U'");
    uint64_t i2 = cube_to_partial_c_index(&c2, UD, corners, 4);

    cube_move_apply_move_string(&c, "U R' U' R B2 R' U R U'");
    cr_assert_eq(i2, cube_to_partial_c_index(&c2, UD, corners, 4));
}


Test(cube_index, partial_c_index_preserved_order_of_corners){
    uint64_t i1, i2, i3, i4;
    {
        int corners[4] = {UFL, UFR, DFR, DFL}; 
        cube_t c = cube_create_new_cube();
        cube_move_apply_move_string(&c, "F U F' R U2 R' U");

        i1 = cube_to_partial_c_index(&c, UD, corners, 4);
    }
    {
        int corners[4] = {UFR, UFL, DFR, DFL}; 
        cube_t c = cube_create_new_cube();
        cube_move_apply_move_string(&c, "F U F' R U2 R' U");

        i2 = cube_to_partial_c_index(&c, UD, corners, 4);
    }
    {
        int corners[4] = {UFL, DFR, UFR, DFL}; 
        cube_t c = cube_create_new_cube();
        cube_move_apply_move_string(&c, "F U F' R U2 R' U");

        i3 = cube_to_partial_c_index(&c, UD, corners, 4);
    }
    {
        int corners[4] = {UFL, DFL, DFR, UFR}; 
        cube_t c = cube_create_new_cube();
        cube_move_apply_move_string(&c, "F U F' R U2 R' U");

        i4 = cube_to_partial_c_index(&c, UD, corners, 4);
    }

    cr_assert_eq(i1, i2);
    cr_assert_eq(i2, i3);
    cr_assert_eq(i3, i4);
}
