#ifndef _MOVES_H_
#define _MOVES_H_

#include "cube.h"
#include <stdio.h>

typedef enum {
  U, D,
  L, R,
  F, B,
} facemove;

// idea: write the following tables to a file:
// move_table_corners[NMOVES][corner]         // we have 9 bits for corner: 18*2**9 = 9216
// move_table_edges[NMOVES][edge]           // we have 7 bits for edges: 18*2**7 = 2304
// so total table size is: 18*(2**9 + 2**7) * 16 = 184320 bits ~ 23 kB

/*
    actually: a corner is uniquely identified by co_fb + cp
    and an edge by eo_ud + ep, so we can do the following:

    uint16_t move_table_corners[NMOVES][corner & 0b11111]
    uint16_t move_table_edges[NMOVES][edge >> 2]
    
    so total size of the tables is: 18*(24 + 24) * 16 = 13824 ~ 1.7 kB
*/

// then a move is just a look-up:
// ex.:
// void move(int move):
//   c_trans = move_table_corners[move];
//   corners[0] = c_trans[corners[0]];
//   e_trans = move_table_edges[move];
//   edges[0] = c_trans[edges[0]]

// try first to just generate the table, then do moves.
// if this works fine:
// try to write it to a file and load on init.
// then do some benchmarking

void initialize_move_tables(uint16_t corner_trans[NMOVES][NCORNERCUBIES], uint16_t edge_trans[NMOVES][NEDGECUBIES]);
void gen_move_tables(uint16_t corner_trans[NMOVES][NCORNERCUBIES], uint16_t edge_trans[NMOVES][NEDGECUBIES]);

void make_move(cube_t* cube, facemove move, int turns, uint16_t corner_trans[NMOVES][NCORNERCUBIES], uint16_t edge_trans[NMOVES][NEDGECUBIES]);

#endif