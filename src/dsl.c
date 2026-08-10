#include "dsl.h"


/* ---------------------------------------------------------------- */
/* lexer helpers                                                    */
/* ---------------------------------------------------------------- */

typedef struct {
    const char* s;
    size_t pos;
    size_t len;
    char err[256];
    FILE* trace;   /* when set, log every consumed token (debugging) */
} dsl_parser_t;

static int dsl_peek(const dsl_parser_t* p) {
    size_t i = p->pos;
    while (i < p->len && isspace((unsigned char)p->s[i])) i++;
    if (i >= p->len) return -1;
    return (unsigned char)p->s[i];
}

static size_t dsl_next_pos(const dsl_parser_t* p) {
    size_t i = p->pos;
    while (i < p->len && isspace((unsigned char)p->s[i])) i++;
    return i;
}

static int dsl_match(dsl_parser_t* p, char c) {
    size_t i = dsl_next_pos(p);
    if (i < p->len && p->s[i] == c) {
        if (p->trace) fprintf(p->trace, "col %zu: '%c'\n", i + 1, c);
        p->pos = i + 1; return 1;
    }
    return 0;
}

static void dsl_fail(dsl_parser_t* p, const char* msg) {
    if (p->err[0] == '\0')
        snprintf(p->err, sizeof(p->err), "%s (at column %zu)", msg, dsl_next_pos(p) + 1);
}

/* reads [A-Za-z0-9_]+ into out; returns length, or -1 if none. */
static int dsl_read_ident(dsl_parser_t* p, char* out, size_t outsz) {
    size_t i = dsl_next_pos(p), start = i;
    while (i < p->len && (isalnum((unsigned char)p->s[i]) || p->s[i] == '_')) i++;
    if (i == start) return -1;
    if (i - start >= outsz) { dsl_fail(p, "identifier too long"); return -1; }
    memcpy(out, p->s + start, i - start);
    out[i - start] = '\0';
    if (p->trace) fprintf(p->trace, "col %zu: ident '%s'\n", start + 1, out);
    p->pos = i;
    return (int)(i - start);
}

/* ---------------------------------------------------------------- */
/* pieces & groups                                                  */
/* ---------------------------------------------------------------- */

static const char* const edge_names[NEDGES] = {
  "UB", "UR", "UF", "UL", "DF", "DR", "DB", "DL", "BL", "BR", "FR", "FL",
};
static const char* const corner_names[NCORNERS] = {
  "UBL", "UBR", "UFR", "UFL", "DFL", "DFR", "DBR", "DBL",
};

static int edge_bit(const char* name) {
    for (int i = 0; i < NEDGES; i++)
        if (strcmp(name, edge_names[i]) == 0) return i;
    return -1;
}

static int corner_bit(const char* name) {
    for (int i = 0; i < NCORNERS; i++)
        if (strcmp(name, corner_names[i]) == 0) return i;
    return -1;
}

/* masks are verified by test/steps_dsl_tests.c against the piece names */
static const struct { const char* name; uint16_t edge_mask; uint8_t corner_mask; } dsl_groups[] = {
    { "U",  0x000F, 0x0F },
    { "Ue", 0x000F, 0x00 },
    { "Uc", 0x0000, 0x0F },
    { "D",  0x00F0, 0xF0 },
    { "De", 0x00F0, 0x00 },
    { "Dc", 0x0000, 0xF0 },
    { "E",  0x0F00, 0x00 },
    { "M",  0x0055, 0x00 },
    { "S",  0x00AA, 0x00 },
    { "Dw", 0x0FF0, 0xF0 },
    { "Uw", 0x0F0F, 0x0F },
    { "Rw", 0x0A77, 0x66 },
    { "Lw", 0x05DD, 0x99 },
    { "Fw", 0x06EE, 0x3C },
    { "Bw", 0x09EB, 0xC3 },
};

static int dsl_group_lookup(const char* name, uint16_t* em, uint8_t* cm) {
    for (size_t i = 0; i < sizeof(dsl_groups) / sizeof(dsl_groups[0]); i++) {
        if (strcmp(name, dsl_groups[i].name) == 0) {
            *em = dsl_groups[i].edge_mask;
            *cm = dsl_groups[i].corner_mask;
            return 1;
        }
    }
    return 0;
}

/* comma-separated piece/group list -> position masks (incl. the `*` wildcard) */
static int dsl_parse_pieces(dsl_parser_t* p, uint16_t* em, uint8_t* cm) {
    uint16_t e = 0;
    uint8_t c = 0;
    int any = 0;
    for (;;) {
        if (dsl_match(p, '*')) {
            e = 0x0FFF; c = 0xFF;
        } else {
            char id[16];
            if (dsl_read_ident(p, id, sizeof(id)) < 0) { dsl_fail(p, "expected a piece or group name"); return 0; }
            uint16_t ge; uint8_t gc;
            if (dsl_group_lookup(id, &ge, &gc)) { e |= ge; c |= gc; }
            else {
                int b = edge_bit(id);
                if (b >= 0) e |= (uint16_t)(1u << b);
                else if ((b = corner_bit(id)) >= 0) c |= (uint8_t)(1u << b);
                else { dsl_fail(p, "unknown piece or group"); return 0; }
            }
        }
        any = 1;
        if (!dsl_match(p, ',')) break;
    }
    if (!any) { dsl_fail(p, "empty piece list"); return 0; }
    *em = e;
    *cm = c;
    return 1;
}

/* ---------------------------------------------------------------- */
/* Constructing expressions                                         */
/* ---------------------------------------------------------------- */


static int dsl_prim_lookup(const char* id, dsl_atom_kind* kind, axes* axis) {
    for (size_t i = 0; i < sizeof(dsl_primitives) / sizeof(dsl_primitives[0]); i++) {
        if (strcmp(id, dsl_primitives[i].name) == 0) {
            *kind = dsl_primitives[i].kind;
            *axis = dsl_primitives[i].axis;
            return 1;
        }
    }
    return 0;
}

static dsl_expr_t* dsl_new_atom(dsl_parser_t* p, dsl_atom_kind atom_kind, axes ax,
                                uint16_t edge_mask, uint8_t corner_mask) {
    dsl_expr_t* e = calloc(1, sizeof *e);
    if (!e) { dsl_fail(p, "out of memory"); return NULL; }
    e->kind = EXPR_ATOM;
    e->atom_kind = atom_kind;
    e->axis = ax;
    e->edge_mask = edge_mask;
    e->corner_mask = corner_mask;
    return e;
}

static dsl_expr_t* dsl_new_binop(dsl_parser_t* p, dsl_expr_kind kind,
                                 dsl_expr_t* left, dsl_expr_t* right) {
    dsl_expr_t* e = calloc(1, sizeof *e);
    if (!e) { dsl_fail(p, "out of memory"); return NULL; }
    e->kind = kind;
    e->left = left;
    e->right = right;
    return e;
}


/* ---------------------------------------------------------------- */
/* Parsing                                                          */
/* ---------------------------------------------------------------- */

static dsl_expr_t* dsl_parse_expr(dsl_parser_t* p);  /* forward decl */

static dsl_expr_t* dsl_parse_primary(dsl_parser_t* p) {
    if (dsl_match(p, '(')) {
        dsl_expr_t* e = dsl_parse_expr(p);
        if (!e) return NULL;
        if (!dsl_match(p, ')')) { dsl_fail(p, "expected ')'"); dsl_free_expression(e); return NULL; }
        return e;
    }

    char id[16];
    if (dsl_read_ident(p, id, sizeof(id)) < 0) { dsl_fail(p, "expected a primitive"); return NULL; }

    dsl_atom_kind kind; axes ax;
    if (!dsl_prim_lookup(id, &kind, &ax)) { dsl_fail(p, "unknown primitive"); return NULL; }

    uint16_t em = 0x0FFF; uint8_t cm = 0xFF;   /* `prim` without `:pieces` == `prim:*` */
    if (dsl_match(p, ':'))
        if (!dsl_parse_pieces(p, &em, &cm)) return NULL;

    return dsl_new_atom(p, kind, ax, em, cm);
}

static dsl_expr_t* dsl_parse_unary(dsl_parser_t* p) {
    if (dsl_match(p, '!')) {
        dsl_expr_t* child = dsl_parse_unary(p);
        if (!child) return NULL;
        return dsl_new_binop(p, EXPR_NOT, child, NULL);
    }
    return dsl_parse_primary(p);
}

static dsl_expr_t* dsl_parse_and(dsl_parser_t* p) {
    dsl_expr_t* left = dsl_parse_unary(p);
    if (!left) return NULL;
    while (dsl_match(p, '&')) {
        dsl_expr_t* right = dsl_parse_unary(p);
        if (!right) { dsl_free_expression(left); return NULL; }
        left = dsl_new_binop(p, EXPR_AND, left, right);
    }
    return left;
}

static dsl_expr_t* dsl_parse_or(dsl_parser_t* p) {
    dsl_expr_t* left = dsl_parse_and(p);
    if (!left) return NULL;
    while (dsl_match(p, '|')) {
        dsl_expr_t* right = dsl_parse_and(p);
        if (!right) { dsl_free_expression(left); return NULL; }
        left = dsl_new_binop(p, EXPR_OR, left, right);
    }
    return left;
}

static dsl_expr_t* dsl_parse_expr(dsl_parser_t* p) { return dsl_parse_or(p); }

dsl_expr_t* dsl_parse(const char* str, char* err, size_t errsz) {
    dsl_parser_t p = {.s = str, .pos = 0, .len = strlen(str), .err = ""};
    dsl_expr_t* e = dsl_parse_expr(&p);
    if (e == NULL) {
        snprintf(err, errsz, "%s", p.err[0] ? p.err : "parse error");
        return NULL;
    }
    if (dsl_peek(&p) != -1) {
        dsl_free_expression(e);
        snprintf(err, errsz, "unexpected character at column %zu", dsl_next_pos(&p) + 1);
        return NULL;
    }
    return e;
}

/* ---------------------------------------------------------------- */
/* Expression                                                       */
/* ---------------------------------------------------------------- */

static bool dsl_eval_permute(const dsl_expr_t* e, cube_t* cube) {
    for (int i = 0; i < NEDGES; i++)
        if (e->edge_mask & (1u << i))
            if (extract_edge_perm(cube->edges[i]) != i) return false;
    for (int i = 0; i < NCORNERS; i++)
        if (e->corner_mask & (1u << i))
            if (extract_corner_perm(cube->corners[i]) != i) return false;
    return true;
}

/* all three stored orientation components must be 0 (piece in solved orientation) */
static bool dsl_eval_fully_oriented(const dsl_expr_t* e, cube_t* cube) {
    for (int i = 0; i < NEDGES; i++)
        if (e->edge_mask & (1u << i)) {
            if (extract_edge_orien(cube->edges[i], FB) != 0 ||
                extract_edge_orien(cube->edges[i], LR) != 0 ||
                extract_edge_orien(cube->edges[i], UD) != 0) return false;
        }
    for (int i = 0; i < NCORNERS; i++)
        if (e->corner_mask & (1u << i)) {
            if (extract_corner_orien(cube->corners[i], FB) != 0 ||
                extract_corner_orien(cube->corners[i], LR) != 0 ||
                extract_corner_orien(cube->corners[i], UD) != 0) return false;
        }
    return true;
}


/* Evaluate an expression on a cube */
bool dsl_eval(const dsl_expr_t* e, cube_t* cube) {
    switch (e->kind) {
    case EXPR_AND: return dsl_eval(e->left, cube) && dsl_eval(e->right, cube);
    case EXPR_OR:  return dsl_eval(e->left, cube) || dsl_eval(e->right, cube);
    case EXPR_NOT: return !dsl_eval(e->left, cube);
    case EXPR_ATOM: break;
    }

    switch (e->atom_kind) {
    case ATOM_EO:
        for (int i = 0; i < NEDGES; i++)
            if (e->edge_mask & (1u << i))
                if (extract_edge_orien(cube->edges[i], e->axis) != 0) return false;
        return true;
    case ATOM_CO:
        for (int i = 0; i < NCORNERS; i++)
            if (e->corner_mask & (1u << i))
                if (extract_corner_orien(cube->corners[i], e->axis) != 0) return false;
        return true;
    case ATOM_EP:
        for (int i = 0; i < NEDGES; i++)
            if (e->edge_mask & (1u << i))
                if (extract_edge_perm(cube->edges[i]) != i) return false;
        return true;
    case ATOM_CP:
        for (int i = 0; i < NCORNERS; i++)
            if (e->corner_mask & (1u << i))
                if (extract_corner_perm(cube->corners[i]) != i) return false;
        return true;
    case ATOM_SOLVED:
        return dsl_eval_permute(e, cube) && dsl_eval_fully_oriented(e, cube);
    }
    return false;
}

bool dsl_is_maybe_fin(dsl_expr_t* e) {
    return e->kind == EXPR_ATOM && e->atom_kind == ATOM_SOLVED &&
           e->edge_mask == 0x0FFF && e->corner_mask == 0xFF;
}

/* recursive free-er */
void dsl_free_expression(dsl_expr_t* e) {
    if (!e) return;
    dsl_free_expression(e->left);
    dsl_free_expression(e->right);
    free(e);
}

/* ---------------------------------------------------------------- */
/* debugging / printing helpers                                     */
/* ---------------------------------------------------------------- */

/* ---------------------------- names ----------------------------- */ 

static const char* dsl_expr_kind_name(dsl_expr_kind k) {
    switch (k) {
    case EXPR_AND:  return "EXPR_AND";
    case EXPR_OR:   return "EXPR_OR";
    case EXPR_NOT:  return "EXPR_NOT";
    case EXPR_ATOM: return "ATOM";
    }
    return "?";
}

static const char* dsl_axis_name(axes a) {
    return a == FB ? "FB" : a == LR ? "LR" : "UD";
}

static const char* dsl_prim_name(dsl_atom_kind kind, axes ax) {
    switch (kind) {
    case ATOM_EO:     return ax == FB ? "eofb" : ax == LR ? "eolr" : "eoud";
    case ATOM_CO:     return ax == FB ? "cofb" : ax == LR ? "colr" : "coud";
    case ATOM_EP:     return "ep";
    case ATOM_CP:     return "cp";
    case ATOM_SOLVED: return "solved";
    }
    return "?";
}

/* ---------------------------- render -------------------------------*/

/* renders a mask as a sorted, comma-separated piece list */
static void dsl_render_pieces(uint16_t em, uint8_t cm, char* buf, size_t sz) {
    size_t pos = 0;
    int first = 1;
    for (int i = 0; i < NEDGES; i++) {
        if (em & (1u << i)) {
            int n = snprintf(buf + pos, sz - pos, "%s%s", first ? "" : ",", edge_names[i]);
            pos += (size_t)n;
            first = 0;
        }
    }
    for (int i = 0; i < NCORNERS; i++) {
        if (cm & (1u << i)) {
            int n = snprintf(buf + pos, sz - pos, "%s%s", first ? "" : ",", corner_names[i]);
            pos += (size_t)n;
            first = 0;
        }
    }
}

static char* dsl_render(const dsl_expr_t* e) {
    char buf[256];
    char* s = NULL;

    switch (e->kind) {
    case EXPR_AND:
    case EXPR_OR: {
        char* l = dsl_render(e->left);
        char* r = dsl_render(e->right);
        s = malloc(strlen(l) + strlen(r) + 8);
        sprintf(s, "(%s %c %s)", l, e->kind == EXPR_AND ? '&' : '|', r);
        free(l); free(r);
        return s;
    }
    case EXPR_NOT: {
        char* c = dsl_render(e->left);
        s = malloc(strlen(c) + 4);
        sprintf(s, "!(%s)", c);
        free(c);
        return s;
    }
    case EXPR_ATOM:
        if (e->edge_mask == 0x0FFF && e->corner_mask == 0xFF)
            sprintf(buf, "%s:*", dsl_prim_name(e->atom_kind, e->axis));
        else {
            char pieces[192];
            dsl_render_pieces(e->edge_mask, e->corner_mask, pieces, sizeof pieces);
            sprintf(buf, "%s:%s", dsl_prim_name(e->atom_kind, e->axis), pieces);
        }
        return strdup(buf);
    }
    return NULL;
}

char* dsl_canonical(const dsl_expr_t* e) { return dsl_render(e); }

typedef struct {
    char* s;
    size_t len;
    size_t cap;
} dsl_strbuf_t;

static void dsl_strbuf_reserve(dsl_strbuf_t* b, size_t extra) {
    if (b->len + extra + 1 <= b->cap) return;
    size_t ncap = b->cap ? b->cap : 64;
    while (ncap < b->len + extra + 1) ncap *= 2;
    char* ns = realloc(b->s, ncap);
    if (!ns) return;
    b->s = ns;
    b->cap = ncap;
}

static void dsl_strbuf_add(dsl_strbuf_t* b, const char* s) {
    size_t n = strlen(s);
    dsl_strbuf_reserve(b, n);
    if (!b->s) return;
    memcpy(b->s + b->len, s, n);
    b->len += n;
    b->s[b->len] = '\0';
}

static void dsl_strbuf_addf(dsl_strbuf_t* b, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    va_list ap2;
    va_copy(ap2, ap);
    int need = vsnprintf(NULL, 0, fmt, ap);
    va_end(ap);
    if (need < 0) { va_end(ap2); return; }
    dsl_strbuf_reserve(b, (size_t)need);
    if (!b->s) { va_end(ap2); return; }
    vsnprintf(b->s + b->len, b->cap - b->len, fmt, ap2);
    va_end(ap2);
    b->len += (size_t)need;
}


static void dsl_debug_node(dsl_strbuf_t* b, const dsl_expr_t* e) {
    if (e->kind == EXPR_ATOM)
        dsl_strbuf_addf(b, "ATOM %s  axis=%s  edges=0x%04X corners=0x%02X",
                        dsl_prim_name(e->atom_kind, e->axis),
                        dsl_axis_name(e->axis), e->edge_mask, e->corner_mask);
    else
        dsl_strbuf_add(b, dsl_expr_kind_name(e->kind));
}

static void dsl_debug_dump(dsl_strbuf_t* b, const dsl_expr_t* e,
                           const char* prefix, int is_last) {
    dsl_strbuf_add(b, prefix);
    if (prefix[0] != '\0')
        dsl_strbuf_add(b, is_last ? "└─ " : "├─ ");
    dsl_debug_node(b, e);
    dsl_strbuf_add(b, "\n");
    if (e->kind == EXPR_ATOM) return;

    const dsl_expr_t* kids[2];
    int n = 0;
    if (e->left) kids[n++] = e->left;
    if (e->right) kids[n++] = e->right;
    char child_prefix[512];
    snprintf(child_prefix, sizeof child_prefix, "%s%s",
             prefix, is_last ? "   " : "│  ");
    for (int i = 0; i < n; i++)
        dsl_debug_dump(b, kids[i], child_prefix, i == n - 1);
}

char* dsl_debug_str(const dsl_expr_t* e) {
    if (!e) return strdup("(null)");
    dsl_strbuf_t b = {0};
    dsl_debug_dump(&b, e, "", 1);
    if (!b.s) return strdup("");
    return b.s;
}

void dsl_debug_print(const dsl_expr_t* e, FILE* out) {
    if (!out) return;
    char* s = dsl_debug_str(e);
    fputs(s, out);
    free(s);
}

void dsl_debug_parse(const char* str, FILE* out) {
    if (!out) return;
    fprintf(out, "> %s\n", str);
    dsl_parser_t p = {.s = str, .pos = 0, .len = strlen(str), .err = "", .trace = out};
    dsl_expr_t* e = dsl_parse_expr(&p);
    if (e == NULL) {
        fprintf(out, "parse error: %s\n", p.err[0] ? p.err : "parse error");
        return;
    }
    if (dsl_peek(&p) != -1) {
        dsl_free_expression(e);
        fprintf(out, "parse error: unexpected character at column %zu\n",
                dsl_next_pos(&p) + 1);
        return;
    }
    char* c = dsl_render(e);
    fprintf(out, "parsed OK\nRender: %s\n\ntree:\n", c);
    free(c);
    char* t = dsl_debug_str(e);
    fputs(t, out);
    free(t);
    dsl_free_expression(e);
}

