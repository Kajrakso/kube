#include <string.h>

#include "criterion/criterion.h"

#include "../src/moveset.h"
#include "../src/core/move.h"
#include "../src/tables.h"

static uint32_t mask_of(const char* names[]) {
    uint32_t mask = 0;
    for (size_t i = 0; names[i]; i++) {
        int m = move_from_name(names[i]);
        cr_assert_geq(m, 0, "bad move name \"%s\"", names[i]);
        mask |= (uint32_t)1 << m;
    }
    return mask;
}

static uint32_t qtm_mask(void) {
    uint32_t m = 0;
    for (int mv = 0; mv < NMOVES; mv++)
        if (mv % 3 != 1) m |= (uint32_t)1 << mv;
    return m;
}

Test(moveset, move_from_name_roundtrip) {
    for (int m = 0; m < NMOVES; m++)
        cr_assert_eq(move_from_name(move_notation[m]), m);
    cr_assert_eq(move_from_name(""), -1);
    cr_assert_eq(move_from_name("X"), -1);
    cr_assert_eq(move_from_name("u"), -1);
    cr_assert_eq(move_from_name("R2'"), -1);
}

Test(moveset, parse_predefined_sets) {
    uint32_t mask;
    char err[64];
    cr_assert_eq(moveset_parse("HTM", &mask, err, sizeof err), 0);
    cr_assert_eq(mask, MOVESET_HTM);
    cr_assert_eq(moveset_parse("FTM", &mask, err, sizeof err), 0);
    cr_assert_eq(mask, MOVESET_HTM);
    cr_assert_eq(moveset_parse("QTM", &mask, err, sizeof err), 0);
    cr_assert_eq(mask, qtm_mask());
}

Test(moveset, parse_face_list_expands_to_triplets) {
    uint32_t mask;
    char err[64];
    cr_assert_eq(moveset_parse("U", &mask, err, sizeof err), 0);
    cr_assert_eq(mask, mask_of((const char*[]){"U", "U2", "U'", NULL}));

    cr_assert_eq(moveset_parse("D,U", &mask, err, sizeof err), 0);
    cr_assert_eq(mask, mask_of(
        (const char*[]){"D", "D2", "D'", "U", "U2", "U'", NULL}));

    cr_assert_eq(moveset_parse("U2", &mask, err, sizeof err), 0);
    cr_assert_eq(mask, mask_of((const char*[]){"U", "U2", "U'", NULL}));

    cr_assert_eq(moveset_parse("L',R2", &mask, err, sizeof err), 0);
    cr_assert_eq(mask, mask_of(
        (const char*[]){"L", "L2", "L'", "R", "R2", "R'", NULL}));

    /* outer leading whitespace is skipped; tokens must not contain spaces */
    cr_assert_eq(moveset_parse("  U,D", &mask, err, sizeof err), 0);
    cr_assert_eq(mask, mask_of(
        (const char*[]){"U", "U2", "U'", "D", "D2", "D'", NULL}));
}

Test(moveset, parse_errors) {
    uint32_t mask;
    char err[64];
    const char* bad[] = {"", ",", ",,", "X", "U,X", "u", "L' R2", "U ,D"};
    for (size_t i = 0; i < sizeof bad / sizeof bad[0]; i++) {
        err[0] = '\0';
        cr_assert_eq(moveset_parse(bad[i], &mask, err, sizeof err), -1,
                     "expected failure for \"%s\"", bad[i]);
        cr_assert_neq(err[0], '\0', "expected message for \"%s\"", bad[i]);
    }
}

Test(moveset, parsed_masks_are_inverse_closed) {
    uint32_t mask;
    char err[64];
    const char* specs[] = {"HTM", "FTM", "QTM", "U,D", "R", "F,B,L"};
    for (size_t i = 0; i < sizeof specs / sizeof specs[0]; i++) {
        cr_assert_eq(moveset_parse(specs[i], &mask, err, sizeof err), 0);
        cr_assert(moveset_is_inverse_closed(mask), "\"%s\"", specs[i]);
    }
    cr_assert_not(moveset_is_inverse_closed((uint32_t)1 << U1));
    cr_assert_not(moveset_is_inverse_closed(
        ((uint32_t)1 << U1) | ((uint32_t)1 << U2)));
    cr_assert(moveset_is_inverse_closed(0));
}

Test(moveset, contains_and_initial_and_eff) {
    uint32_t qtm = qtm_mask();
    cr_assert(moveset_contains(MOVESET_HTM, qtm));
    cr_assert(moveset_contains(qtm, qtm));
    cr_assert_not(moveset_contains(qtm, MOVESET_HTM));
    cr_assert(moveset_contains(MOVESET_HTM, 0));

    cr_assert_eq(moveset_initial(MOVESET_HTM), MOVESET_HTM);
    cr_assert_eq(moveset_initial(0), MOVESET_HTM);
    cr_assert_eq(moveset_initial(qtm), qtm);

    cr_assert_eq(moveset_eff(0), MOVESET_HTM);
    cr_assert_eq(moveset_eff((uint32_t)1 << U1), (uint32_t)1 << U1);
}

Test(moveset, follow_htm_axis_pairs) {
    uint32_t no_u_no_d = mask_of((const char*[]){
        "L", "L2", "L'", "R", "R2", "R'",
        "F", "F2", "F'", "B", "B2", "B'", NULL});
    cr_assert_eq(moveset_follow(MOVESET_HTM, U1), no_u_no_d);
    cr_assert_eq(moveset_follow(MOVESET_HTM, U2), no_u_no_d);
    cr_assert_eq(moveset_follow(MOVESET_HTM, U3), no_u_no_d);

    uint32_t no_d = MOVESET_HTM
                  & ~mask_of((const char*[]){"D", "D2", "D'", NULL});
    cr_assert_eq(moveset_follow(MOVESET_HTM, D1), no_d);
    cr_assert_eq(moveset_follow(MOVESET_HTM, D2), no_d);
    cr_assert_eq(moveset_follow(MOVESET_HTM, D3), no_d);

    uint32_t no_l_no_r = MOVESET_HTM
                       & ~mask_of((const char*[]){
                           "L", "L2", "L'", "R", "R2", "R'", NULL});
    uint32_t no_r = MOVESET_HTM
                  & ~mask_of((const char*[]){"R", "R2", "R'", NULL});
    /* after a lower face of a commuting pair the higher one is dropped too */
    cr_assert_eq(moveset_follow(MOVESET_HTM, L1), no_l_no_r);
    cr_assert_eq(moveset_follow(MOVESET_HTM, L3), no_l_no_r);
    cr_assert_eq(moveset_follow(MOVESET_HTM, R1), no_r);

    uint32_t no_f_no_b = MOVESET_HTM
                       & ~mask_of((const char*[]){
                           "F", "F2", "F'", "B", "B2", "B'", NULL});
    uint32_t no_b = MOVESET_HTM
                  & ~mask_of((const char*[]){"B", "B2", "B'", NULL});
    cr_assert_eq(moveset_follow(MOVESET_HTM, F2), no_f_no_b);
    cr_assert_eq(moveset_follow(MOVESET_HTM, B3), no_b);

    cr_assert_eq(moveset_follow(MOVESET_HTM, NULLMOVE), MOVESET_HTM);
}

Test(moveset, follow_qtm_half_turn_composition) {
    uint32_t qtm = qtm_mask();
    uint32_t quarters_lr_fb = mask_of((const char*[]){
        "L", "L'", "R", "R'", "F", "F'", "B", "B'", NULL});

    cr_assert_eq(moveset_follow(qtm, U1),
                 quarters_lr_fb | ((uint32_t)1 << U1));

    uint32_t expected_u3 = quarters_lr_fb;
    cr_assert_eq(moveset_follow(qtm, U3), expected_u3);

    uint32_t expected_d1 = quarters_lr_fb
                         | ((uint32_t)1 << U1) | ((uint32_t)1 << U3)
                         | ((uint32_t)1 << D1);
    cr_assert_eq(moveset_follow(qtm, D1), expected_d1);
}

Test(moveset, follow_small_masks) {
    uint32_t u_triplet = mask_of((const char*[]){"U", "U2", "U'", NULL});
    cr_assert_eq(moveset_follow(u_triplet, U1), 0);
    cr_assert_eq(moveset_follow(u_triplet, U2), 0);
    cr_assert_eq(moveset_follow(u_triplet, U3), 0);

    uint32_t u1_only = (uint32_t)1 << U1;
    cr_assert_eq(moveset_follow(u1_only, U1), u1_only);

    uint32_t u13 = ((uint32_t)1 << U1) | ((uint32_t)1 << U3);
    cr_assert_eq(moveset_follow(u13, U1), (uint32_t)1 << U1);
    cr_assert_eq(moveset_follow(u13, U3), 0);

    uint32_t u3_only = (uint32_t)1 << U3;
    cr_assert_eq(moveset_follow(u3_only, U3), u3_only);
}

Test(moveset, follow_invariants_over_masks) {
    uint32_t masks[5];
    char err[64];
    masks[0] = MOVESET_HTM;
    masks[1] = qtm_mask();
    cr_assert_eq(moveset_parse("U,D", &masks[2], err, sizeof err), 0);
    cr_assert_eq(moveset_parse("R", &masks[3], err, sizeof err), 0);
    cr_assert_eq(moveset_parse("U,F,B", &masks[4], err, sizeof err), 0);

    for (size_t i = 0; i < 5; i++) {
        uint32_t eff = moveset_eff(masks[i]);
        for (int prev = 0; prev <= NMOVES; prev++) {
            uint32_t f = moveset_follow(masks[i], prev);
            cr_assert_eq(f & ~eff, (uint32_t)0,
                         "follow not subset of mask %u prev %d", masks[i], prev);
            if (prev != NULLMOVE)
                cr_assert_eq(f & (1u << get_inv_move(prev)), (uint32_t)0,
                             "inverse allowed: mask %u prev %d",
                             masks[i], prev);
        }
    }
}

Test(moveset, follow_htm_matches_legacy_move_mask) {
    for (int m = 0; m < NMOVES; m++)
        cr_assert_eq(moveset_follow(MOVESET_HTM, m), move_mask[m],
                     "prev %s", move_notation[m]);
}

Test(moveset, canonical_strings) {
    char buf[128];
    uint32_t qtm = qtm_mask();

    int len = moveset_canonical(qtm, buf, sizeof buf);
    cr_assert_eq(len, (int)strlen(buf));
    cr_assert_str_eq(buf, "U,U',D,D',L,L',R,R',F,F',B,B'");

    uint32_t u2 = (uint32_t)1 << U2;
    cr_assert_eq(moveset_canonical(u2, buf, sizeof buf), 2);
    cr_assert_str_eq(buf, "U2");

    cr_assert_eq(moveset_canonical(0, buf, sizeof buf), 0);
    cr_assert_str_eq(buf, "");

    cr_assert_eq(moveset_canonical(u2, buf, 2), -1);
    cr_assert_eq(moveset_canonical(u2, buf, 3), 2);
}
