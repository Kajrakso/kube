#ifndef _MOVETABLES_H_
#define _MOVETABLES_H_

#include <stdbool.h>

/* Initializes the move tables.
You have to call this function before applying moves to the cube.*/
void cube_movetables_generate();

/* Saves the move tables to disk. */
bool cube_movetables_save(const char *filename);

/* Loads the move tables from a file. */
bool cube_movetables_load(const char *filename);

#endif /* _MOVETABLES_H_ */