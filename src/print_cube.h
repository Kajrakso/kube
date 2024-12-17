// THIS FILE IS OLD!

#ifndef _PRINT_CUBE_H_
#define _PRINT_CUBE_H_


#include <stdio.h>
#include "cube.h"
#include "moves.h"

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"

#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0') 


#define CO_BYTES_TO_BINARY_PATTERN "[%i%i%i%i, %i%i%i%i]"
#define EO_BYTES_TO_BINARY_PATTERN "[%c%c%c%c, %c%c%c%c, %c%c%c%c]"

#define CO_BYTES_TO_BINARY(byte)  \
  ((int)((byte>>14) & 0x03)), \
  ((int)((byte>>12) & 0x03)), \
  ((int)((byte>>10) & 0x03)), \
  ((int)((byte>>8) & 0x03)), \
  ((int)((byte>>6) & 0x03)), \
  ((int)((byte>>4) & 0x03)), \
  ((int)((byte>>2) & 0x03)), \
  ((int)((byte) & 0x03))

#define EO_BYTES_TO_BINARY(byte)  \
  ((byte>>8) & 0x08 ? '1' : '0'), \
  ((byte>>8) & 0x04 ? '1' : '0'), \
  ((byte>>8) & 0x02 ? '1' : '0'), \
  ((byte>>8) & 0x01 ? '1' : '0'), \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0') 

void print_cube(cube_t* cube);

#endif
