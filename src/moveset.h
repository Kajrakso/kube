#ifndef MOVESET_H
#define MOVESET_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "core/move.h"

/* all 18 moves (U1..B3) */
#define MOVESET_HTM (((uint32_t)1 << NMOVES) - 1)

/* move names in enum-ove order; see core/move.h */
extern const char* move_notation[NMOVES];

/* "U'"/"R2"/... -> enum move index, or -1 if not a move name */
int move_from_name(const char* name);

/* true iff every move in sub is also in super */
bool moveset_contains(uint32_t super, uint32_t sub);

/* parse "HTM" | "FTM" | "QTM" | comma-separated face list.
 * Each token is a face (or any move of that face: U, U2, U' all select U),
 * and a face brings all three of its turns, so the result is always
 * inverse-closed.
 * Returns 0 on success (mask in *mask_out), -1 with a message in err. */
int moveset_parse(const char* str, uint32_t* mask_out, char* err, size_t errsz);

/* deterministic comma-separated sorted move names for a mask
 * (used for messages; the ptable key uses the raw hex, see dsl.c). */
int moveset_canonical(uint32_t mask, char* out, size_t outsz);

/* allowed moves for the FIRST move (prev == NULLMOVE) */
uint32_t moveset_initial(uint32_t mask);

/* moves that may follow `prev` in a reduced sequence over `mask`
 * (see "1.2.1"). For the full HTM set this equals move_mask[prev]
 * bit-for-bit, so built-in HTM behavior is unchanged. */
uint32_t moveset_follow(uint32_t mask, int prev);

/* true iff mask is closed under inversion (R in mask <=> R' in mask).
 * Always true for a parsed moveset (faces expand to full triplets); kept as a
 * documented invariant / safety net. */
bool moveset_is_inverse_closed(uint32_t mask);



// I WANT TO DELETE THIS ONE

/* 0 ("unspecified") means the full HTM set.  Use everywhere a moveset is read
 * from a possibly-defaulted struct, so an unset field degrades to HTM. */
static inline uint32_t moveset_eff(uint32_t mask) {
    return mask ? mask : MOVESET_HTM;
}

#endif /* MOVESET_H */
