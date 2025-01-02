#ifndef _TABLES_H_
#define _TABLES_H_

#include <stdbool.h>

/* Initializes the move and symmetry tables.
You have to call this function before applying moves to the cube.*/
void cube_tables_generate();

/* Saves the move and symmetry tables to disk. */
bool cube_tables_save(const char *filename);

/* Loads the move and symmetry tables from a file. */
bool cube_tables_load(const char *filename);

#endif /* _TABLES_H_ */