#include "criterion/criterion.h"
#include "criterion/redirect.h"

#include <stdint.h>
#include <time.h>

#include "../src/core/cube.h"
#include "../src/core/cube_state.h"
#include "../src/scrambler.h"
#include "../src/tables.h"

// suites

TestSuite(scramble);

// tests

Test(scramble, handscramble_results_in_legal_cube) {
    srand((unsigned int) time(NULL));
    cube_tables_generate();

    cube_t cube = cube_scrambler_get_handscrambled_cube();

    cr_assert(cube_valid_eo(&cube));
    cr_assert(cube_valid_co(&cube));
    cr_assert(cube_valid_permutation(&cube));
}

Test(scramble, scramble_results_in_legal_cube) {
    srand((unsigned int) time(NULL));
    cube_tables_generate();

    cube_t cube = cube_scrambler_get_scrambled_cube();

    cr_assert(cube_valid_eo(&cube));
    cr_assert(cube_valid_co(&cube));
    cr_assert(cube_valid_permutation(&cube));
}

Test(scramble, parse_speffz_corners){
    cube_tables_generate();

    cube_t c1 = cube_create_new_cube();
    cube_t c2 = cube_create_new_cube();

    char* speffz = "da.";
    char* scr = "R' D' R U2 R' D R U R' D' R U R' D R";

    cube_scrambler_parse_speffz(&c1, speffz);
    cube_move_apply_move_string(&c2, scr);

    cr_assert(cube_operation_is_equal(c1, c2));
}


Test(scramble, parse_speffz_edges){
    cube_tables_generate();

    cube_t c1 = cube_create_new_cube();
    cube_t c2 = cube_create_new_cube();

    char* speffz = ".wbxbauadvdltljrj";
    char* scr = "R2 L2 U2 D2 F2 B2";

    cube_scrambler_parse_speffz(&c1, speffz);
    cube_move_apply_move_string(&c2, scr);

    cr_assert(cube_operation_is_equal(c1, c2));
}


Test(scramble, parse_speffz_full){
    cube_tables_generate();

    cube_t c1 = cube_create_new_cube();
    cube_t c2 = cube_create_new_cube();

    char* scr = "R L U D";
    char* speffz = "bsndkfowguea.jvuldarxwtba";

    cube_scrambler_parse_speffz(&c1, speffz);
    cube_move_apply_move_string(&c2, scr);

    cr_assert(cube_operation_is_equal(c1, c2));
}


Test(scramble, parse_speffz_invalid_state){
    cube_tables_generate();

    cube_t c;
    char* speffz;
    int res;

    c = cube_create_new_cube();
    speffz = "a.";
    res = cube_scrambler_parse_speffz(&c, speffz);
    cr_assert_eq(res, 1);

    c = cube_create_new_cube();
    speffz = ".a";
    res = cube_scrambler_parse_speffz(&c, speffz);
    cr_assert_eq(res, 1);

    c = cube_create_new_cube();
    speffz = "ab.a";
    res = cube_scrambler_parse_speffz(&c, speffz);
    cr_assert_eq(res, 1);

    c = cube_create_new_cube();
    speffz = "a.ab";
    res = cube_scrambler_parse_speffz(&c, speffz);
    cr_assert_eq(res, 1);

    c = cube_create_new_cube();
    speffz = "x.";
    res = cube_scrambler_parse_speffz(&c, speffz);
    cr_assert_eq(res, 1);

    c = cube_create_new_cube();
    speffz = ".x";
    res = cube_scrambler_parse_speffz(&c, speffz);
    cr_assert_eq(res, 1);
}
