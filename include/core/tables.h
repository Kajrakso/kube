#ifndef _TABLES_H_
#define _TABLES_H_

#include <stdbool.h>

/* Initializes the move and symmetry tables.
You have to call this function before applying moves to the cube.*/
void cube_tables_generate();

/* Saves a table to disk. */
bool cube_tables_save(const char *filename, uint16_t* table);

/* Loads a table from a file. */
bool cube_tables_load(const char *filename, uint16_t* table);

#endif /* _TABLES_H_ */