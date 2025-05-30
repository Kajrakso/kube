#ifndef _TABLES_H_
#define _TABLES_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Initializes the move and symmetry tables.
You have to call this function before applying moves to the cube.*/
void cube_tables_generate();

/* Call before using tables */
int cube_tables_load();

/* Call after use of tables */
void cube_tables_free();

// todo: WIP
void gen_ptable_L();

void gen_ptable_H();

int init_ptable_H(const char* path);
void free_ptable_H();

#endif /* _TABLES_H_ */
