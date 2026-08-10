#ifndef DSL_H
#define DSL_H

/* Motivation and desciption of the usage of DSL:
 *
 * The DSL (Domain Specific Language) is used for user-defined solving steps.
 * The idea is that the user wants an optimal solution to some "weird" state,
 * that is not captured by the basic solving steps eo, dr, htr and fin.
 * Via the CLI argument -D, the user provides first a name for
 * the custom solving step, and then describes it using
 * primitives (eolr, eoud, cp, ep, etc.), pieces or
 * piece groups (faces) (UBR, UF, FR, L, Dw, M, etc.) and
 * simple operators (and (&), or (|) and not (!)). The solving step is
 * then used by the solver in a pipeline (with further configuration possible).
 *
 * Primitives: eofb, eolr, eoud, cofb, colr, coud, ep, cp
 * Pieces: UBL, UBR, ..., DBR, DBL, UB, UR, UF, UL, DF, ... FR, FL
 * Combinations of pieces: M, S, E, Dw, Uw, ..., Rw, Lw, U, Uc (all corners on U, Ue (all edges on U), * (all pieces)
 * Usage: To define an "atom" (the building block of a custom solved state) use PRIMITIVE:PIECE/COMBINATION OF PIECES*.
 * Where combination of pieces is a predefined one, or pieces separated by commas. To define a custom solved state to
 *
 *     --define 'NAME=ATOM'
 *
 * Atoms are combined using the simple operators above in addition to parentheses. Example:
 *
 *     --define 'NAME=ATOM1 & ATOM2 & (ATOM3 | !ATOM4)'
 * The order of operations are (), !, &, |.
 *
 * Example: To define a solving step for f2l, with the corners on the U-layer permuted (but might not be oriented)
 * and then solve a scramble optimally (first to eo on the FB axis, then to the user defined state), do either:
 *
 *     kube --define 'f2l_c_orien=solved:Dw&coud:U' --step eofb:max=5 --step f2l_c_orien:max=15 "SCRAMBLE"
 *     kube -D 'f2l_c_orien=solved:Dw&coud:U' -s eofb:max=5 -s f2l_c_orien:max=15 "SCRAMBLE"
 *
 * Pruning tables:
 *
 * In order to speed up the solving process, kube will automatically prune the search somehow.
 * To do this, tables have to be generated up front. TODO: How is best to do this?
 * Currently I am thinking to prompt the user the first time a new table is about to be generated,
 * and then save it with a unique hash of the custom solving step. This way, it will be reused
 * later. The user should also be able to pregenerate some predefined tables with
 *
 *     kube -g "all"
 *
 * or something of that sort.
 *
 * Config files:
 *
 * TODO: In the future it would make sense to be able to define solving steps in a config file instead of
 * via --define  (or -D) everytime. This config file could then contain the eo, dr, htr solving steps
 * that currently is defined in solver_steps.c.
 *
 * TODO: It would be super cool if the user can combine already defined solving states to ease the definitions.
 * Example:
 *
 *     kube -D 'f2l=solved:Dw' -D 'step_name:f2l & eofb & solved:UFR,UFL,UBL,UBR'
 *
 * TODO: If the user wants a very general case (like xcross on any face and any pair), then maybe we could
 * add symmetry actions to the definitions?
 * Example:
 *
 *     kube -D 'any_cross:solved=De | solved=Ue | solved=Le | solved=Re | solved=Fe | solved=Be'
 *
 * could instead be something along the lines of:
 *
 *     kube -D 'any_cross:solved=De+S4'
 *
 * encoding that we act on the state "solved=De" with the symmetry group of the cube (which is S4).
 * TODO: How to encode the different symmetries of the cube?
 *
 * TODO: User defined moveset restictions? User defined *starting* moveset restrictions.
 *
 * TODO: actually checking if a custom solved state is solvable is not trivial. For the moment there is no check for solvability.
 *
* TODO: document how to define movesets
 */


#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>

#include "core/cube.h"

#define DSL_NUMBER_OF_PRIMITIVES 9


/* ---------------------------------------------------------------- */
/* atoms                                                            */
/* ---------------------------------------------------------------- */

typedef enum {
    ATOM_EO,
    ATOM_CO,
    ATOM_EP,
    ATOM_CP,
    ATOM_SOLVED,
} dsl_atom_kind;

/* ---------------------------------------------------------------- */
/* primitives                                                       */
/* ---------------------------------------------------------------- */

/* A primitive: a predefined atom. */
typedef struct {
    const char*         name;
    dsl_atom_kind       kind;
    axes                axis; /* orientation axis for ATOM_EO/CO; unused otherwise */
} dsl_prim_t;

static const dsl_prim_t dsl_primitives[DSL_NUMBER_OF_PRIMITIVES] = {
    /* name      kind           axis */
    { "eofb",   ATOM_EO,        FB },
    { "eolr",   ATOM_EO,        LR },
    { "eoud",   ATOM_EO,        UD },
    { "cofb",   ATOM_CO,        FB },
    { "colr",   ATOM_CO,        LR },
    { "coud",   ATOM_CO,        UD },
    { "ep",     ATOM_EP,        UD },
    { "cp",     ATOM_CP,        UD },
    { "solved", ATOM_SOLVED,    UD },
};

/* ---------------------------------------------------------------- */
/* expressions                                                      */
/* ---------------------------------------------------------------- */

typedef enum {
    EXPR_AND,
    EXPR_OR,
    EXPR_NOT,
    EXPR_ATOM,
} dsl_expr_kind;


/* An expression: a binop with two expressions or an Atom with Edge mask and Corner mask */
typedef struct dsl_expr {
    dsl_expr_kind       kind;
    struct dsl_expr*    left;
    struct dsl_expr*    right;

    /* DSL_ATOM only */
    dsl_atom_kind       atom_kind;
    axes                axis;        /* only EO, CO ATOM */
    uint16_t            edge_mask;   /* piece mask; 0x0FFF = all edges */
    uint8_t             corner_mask; /* piece mask; 0xFF = all corners */
} dsl_expr_t;

/* Evaluates the expression for a cube state. */
bool dsl_eval(const dsl_expr_t* e, cube_t* cube);

/* Free an expression */
void dsl_free_expression(dsl_expr_t*);

/* Tries to detect if e is exactly "solve everything" (== fin). */
bool dsl_is_maybe_fin(dsl_expr_t*);

/* ---------------------------------------------------------------- */
/*  Parsing and string operations                                   */
/* ---------------------------------------------------------------- */

/* Parses str into an dsl_expr_t. On error returns NULL and writes a message to err. */
dsl_expr_t* dsl_parse(const char* str, char* err, size_t errsz);

/* Deterministic canonical string (pieces sorted, all-pieces as `*`). Caller frees. */
char* dsl_canonical(const dsl_expr_t* e);

/* ---------------------------------------------------------------- */
/* debugging / printing helpers                                     */
/* ---------------------------------------------------------------- */

/* Renders the AST as an indented tree dump (one node per line), e.g.
 *
 *     EXPR_OR
 *        ├─ ATOM eoud  axis=UD  edges=0x0FFF corners=0xFF
 *        └─ EXPR_AND
 *           └─ ATOM eofb  axis=FB  edges=0x0FFF corners=0xFF
 *
 * The caller must free() the returned string. */
char* dsl_debug_str(const dsl_expr_t* e);

/* Prints the tree dump (see dsl_debug_str) to out. */
void dsl_debug_print(const dsl_expr_t* e, FILE* out);

/* One-shot parse debugging: prints a token-by-token trace of str to out,
 * then, on success, the canonical string and the tree dump; on failure,
 * the parse error message. */
void dsl_debug_parse(const char* str, FILE* out);

#endif /* DSL_H */
