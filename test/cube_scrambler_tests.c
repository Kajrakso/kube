#include "criterion/criterion.h"

#include <stdint.h>

#include "../src/core/cube.h"
#include "../src/solutions.h"
#include "../src/core/cube_state.h"
#include "../src/scrambler.h"
#include "../src/cli.h"

Test(scramble, handscramble_results_in_legal_cube) {
    cube_t cube = cube_scrambler_get_handscrambled_cube();

    cr_assert(cube_valid_eo(&cube));
    cr_assert(cube_valid_co(&cube));
    cr_assert(cube_valid_permutation(&cube));
}

Test(scramble, scramble_results_in_legal_cube) {
    cube_t cube = cube_scrambler_get_scrambled_cube();

    cr_assert(cube_valid_eo(&cube));
    cr_assert(cube_valid_co(&cube));
    cr_assert(cube_valid_permutation(&cube));
}

Test(scramble, parse_speffz_corners){
    cube_t c1 = cube_create_new_cube();
    cube_t c2 = cube_create_new_cube();

    char* speffz = "da.";
    char* scr = "R' D' R U2 R' D R U R' D' R U R' D R";

    cube_scrambler_parse_speffz(&c1, speffz);
    cube_move_apply_move_string(&c2, scr);

    cr_assert(cube_operation_is_equal(c1, c2));
}


Test(scramble, parse_speffz_edges){
    cube_t c1 = cube_create_new_cube();
    cube_t c2 = cube_create_new_cube();

    char* speffz = ".wbxbauadvdltljrj";
    char* scr = "R2 L2 U2 D2 F2 B2";

    cube_scrambler_parse_speffz(&c1, speffz);
    cube_move_apply_move_string(&c2, scr);

    cr_assert(cube_operation_is_equal(c1, c2));
}


Test(scramble, parse_speffz_full){
    cube_t c1 = cube_create_new_cube();
    cube_t c2 = cube_create_new_cube();

    char* scr = "R L U D";
    char* speffz = "bsndkfowguea.jvuldarxwtba";

    cube_scrambler_parse_speffz(&c1, speffz);
    cube_move_apply_move_string(&c2, scr);

    cr_assert(cube_operation_is_equal(c1, c2));
}


Test(scramble, parse_speffz_invalid_state){
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

Test(scramble, parse_scramble_niss_parsing_simple){
    cube_t c1 = cube_create_new_cube();
    cube_t c2 = cube_create_new_cube();

    char* scr_with_inverse = "R (U)";
    char* scr_without_inverse = "U' R";
    cube_move_apply_move_string(&c1, scr_with_inverse);
    cube_move_apply_move_string(&c2, scr_without_inverse);

    cr_assert(cube_operation_is_equal(c1, c2));
}

Test(scramble, parse_scramble_niss_parsing_several){
    cube_t c1 = cube_create_new_cube();
    cube_t c2 = cube_create_new_cube();

    char* scr_with_inverse = "R (U) F B2 (R)";
    char* scr_without_inverse = "R' U' R F B2";
    cube_move_apply_move_string(&c1, scr_with_inverse);
    cube_move_apply_move_string(&c2, scr_without_inverse);

    cr_assert(cube_operation_is_equal(c1, c2));
}

Test(scramble, parse_scramble_niss_parsing_onlyinverse){
    cube_t c1 = cube_create_new_cube();
    cube_t c2 = cube_create_new_cube();

    char* scr_with_inverse = "(R F2 B2 U L')";
    char* scr_without_inverse = "L U' F2 B2 R'";
    cube_move_apply_move_string(&c1, scr_with_inverse);
    cube_move_apply_move_string(&c2, scr_without_inverse);
    
    cr_assert(cube_operation_is_equal(c1, c2));
}


Test(scramble, parse_scramble_niss_parsing_long){
    cube_t c1 = cube_create_new_cube();
    cube_t c2 = cube_create_new_cube();

    char* scr_with_inverse = "(D2 F') L2 U2 B2 F' L2 D' R2 U L2 U' F R' (R F2 B2 U L') B2 L' B D2 F' R F (U L2 D') R (U)";
    char* scr_without_inverse = "U' D L2 U' L U' B2 F2 R' F D2 L2 U2 B2 F' L2 D' R2 U L2 U' F R' B2 L' B D2 F' R F R";
    cube_move_apply_move_string(&c1, scr_with_inverse);
    cube_move_apply_move_string(&c2, scr_without_inverse);

    cr_assert(cube_operation_is_equal(c1, c2));
}


Test(scramble, parse_scramble_with_comments){
    cube_t c1 = cube_create_new_cube();
    cube_t c2 = cube_create_new_cube();

    char* scr_with_inverse = "R U F L // this is a comment";
    char* scr_without_inverse = "R U F L";
    cube_move_apply_move_string(&c1, scr_with_inverse);
    cube_move_apply_move_string(&c2, scr_without_inverse);

    cr_assert(cube_operation_is_equal(c1, c2));
}



Test(scramble, parse_scramble_with_comments_and_newlines){
    cube_t c1 = cube_create_new_cube();
    cube_t c2 = cube_create_new_cube();

    char* scr_with_inverse = "R U F L // this is a comment on the first line \nU R L F // Then this one \nL D R F // final comment";
    char* scr_without_inverse = "R U F L U R L F L D R F";
    cube_move_apply_move_string(&c1, scr_with_inverse);
    cube_move_apply_move_string(&c2, scr_without_inverse);

    cr_assert(cube_operation_is_equal(c1, c2));
}

Test(scramble, parse_scramble_with_comment_with_moves){
    cube_t c1 = cube_create_new_cube();
    cube_t c2 = cube_create_new_cube();

    char* scr_with_inverse = "R U F L // U2 L this is a comment on the first line but it also has moves U D L2 F";
    char* scr_without_inverse = "R U F L";
    cube_move_apply_move_string(&c1, scr_with_inverse);
    cube_move_apply_move_string(&c2, scr_without_inverse);

    cr_assert(cube_operation_is_equal(c1, c2));
}



Test(scramble, parse_string_of_valid_moves_should_succeed){
    cube_t c1 = cube_create_new_cube();
    char* scrs[10] = {
        "R L F B U D",
        "R1 L1 F1 B1 U1 D1",
        "RLFBUD",
        "R1L1F1B1U1D1",
        "R2 L2 F2 B2 U2 D2",
        "R2L2F2B2U2D2",
        "R' L' F' B' U' D'",
        "R3 L3 F3 B3 U3 D3",
        "R'L'F'B'U'D'",
        "R3L3F3B3U3D3",
    };
    
    for (int i = 0; i < 10; i++){
        Solution s;
        solution_init(&s);
        cr_assert(parse_move_string(&s, scrs[i]));
        solution_free(&s);
    }
}

