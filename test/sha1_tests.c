#include "criterion/criterion.h"

#include "../src/utils/sha1.h"


/* ---------------------------------------------------------------- */
/* sha1                                                             */
/* ---------------------------------------------------------------- */

Test(sha1, known_vectors) {
    char out[41];
    sha1_hex("abc", out);
    cr_assert_str_eq(out, "a9993e364706816aba3e25717850c26c9cd0d89d");
    sha1_hex("", out);
    cr_assert_str_eq(out, "da39a3ee5e6b4b0d3255bfef95601890afd80709");
    sha1_hex("wow this is epic", out);
    cr_assert_str_eq(out, "528196240dbb5bc629deee79e3dd15624920f535");
}

Test(sha1, deterministic_and_distinct) {
    char a[41], b[41];
    sha1_hex("eofb:*", a);
    sha1_hex("eofb:*", b);
    cr_assert_str_eq(a, b);
    sha1_hex("eolr:*", b);
    cr_assert_str_neq(a, b);
}


