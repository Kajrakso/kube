#include "moveset.h"
#include <stdio.h>

const char* move_notation[NMOVES] = {
  "U", "U2", "U'", "D", "D2", "D'", "L", "L2", "L'",
  "R", "R2", "R'", "F", "F2", "F'", "B", "B2", "B'",
};

int move_from_name(const char* name) {
    for (int m = 0; m < NMOVES; m++)
        if (strcmp(name, move_notation[m]) == 0) return m;
    return -1;
}

bool moveset_contains(uint32_t super, uint32_t sub) {
    return (super & sub) == sub;
}

uint32_t moveset_initial(uint32_t mask) {
    return moveset_eff(mask);
}

bool moveset_is_inverse_closed(uint32_t mask) {
    for (int m = 0; m < NMOVES; m++)
        if (mask & (1u << m))
            if (!(mask & (1u << get_inv_move(m)))) return false;
    return true;
}

/* The general replacement for the HTM move_mask[prev] intersect (see 1.2.1).
 * For the full HTM set the result is bit-identical to move_mask[prev], so
 * built-in steps and the existing opt1/DR generators are unchanged. */
uint32_t moveset_follow(uint32_t mask, int prev) {
    mask = moveset_eff(mask);
    if (prev == NULLMOVE) return mask;

    /* no cancellation: R R' is a no-op */
    uint32_t r = mask & ~(1u << get_inv_move(prev));

    int pface = prev / 3, pgrp = prev / 6, pturns = (int)(prev % 3) + 1;

    for (int m = 0; m < NMOVES; m++) {
        if (!(r & (1u << m))) continue;
        int face = m / 3, grp = m / 6, turns = (int)(m % 3) + 1;

        if (face == pface) {
            /* same face: the pair's composition is the face turn `t` */
            int t = (pturns + turns) % 4;
            if (t == 0 || move_in_move_mask(mask, 3 * face + t - 1)) {
                r &= ~(1u << m);               /* identity, or already a single move */
            } else if (t == 2) {
                /* half turn not in moveset: reachable as CW,CW and CCW,CCW;
                 * keep only the canonical first move (CW if present, else CCW)
                 * so R' R' is dropped as redundant with R R. */
                int cw = 3 * face, ccw = 3 * face + 2;
                int canon = move_in_move_mask(mask, cw) ? cw : ccw;
                if (prev != canon) r &= ~(1u << m);
            }
        } else if (grp == pgrp && face > pface) {
            /* commuting opposite faces (U/D, L/R, F/B): canonical order */
            r &= ~(1u << m);
        }
    }
    return r;
}

int moveset_canonical(uint32_t mask, char* out, size_t outsz) {
    size_t o = 0;
    for (int m = 0; m < NMOVES; m++) {
        if (!(mask & (1u << m))) continue;
        size_t len = strlen(move_notation[m]);
        if (o + len + (o ? 1 : 0) + 1 > outsz) return -1;
        if (o) out[o++] = ',';
        memcpy(out + o, move_notation[m], len);
        o += len;
    }
    out[o] = '\0';
    return (int)o;
}

int moveset_parse(const char* str, uint32_t* mask_out, char* err, size_t errsz) {
    if (strcmp(str, "HTM") == 0 || strcmp(str, "FTM") == 0) {
        *mask_out = MOVESET_HTM;
        return 0;
    }
    if (strcmp(str, "QTM") == 0) {
        uint32_t m = 0;
        for (int mv = 0; mv < NMOVES; mv++)
            if (mv % 3 != 1) m |= 1u << mv;   /* U1,U3,D1,D3,... : no doubles */
        *mask_out = m;
        return 0;
    }

    uint32_t mask = 0;
    const char* p = str;
    while (*p) {
        while (*p == ' ' || *p == ',') p++;
        if (!*p) break;

        const char* start = p;
        while (*p && *p != ',') p++;

        char name[8];
        size_t len = (size_t)(p - start);
        if (len == 0 || len >= sizeof name) {
            snprintf(err, errsz, "invalid face name"); return -1;
        }
        memcpy(name, start, len);
        name[len] = '\0';

        /* a token selects a face; the face always brings all three turns */
        int m = move_from_name(name);
        if (m < 0) { snprintf(err, errsz, "unknown face '%s'", name); return -1; }
        int face = m / 3;
        mask |= (uint32_t)7 << (3 * face);   /* face's three turns, e.g. 111b */
    }

    if (mask == 0) { snprintf(err, errsz, "empty moveset"); return -1; }
    *mask_out = mask;
    return 0;
}

