#include <stdlib.h>

#include "core/cube.h"
#include "core/move.h"

/* Just does 26 random moves to the cube. Use
 * ``cube_scrambler_get_scrambled_cube`` for a random state. */
cube_t cube_scrambler_get_handscrambled_cube();

/* Returns a completely random cube state. */
cube_t cube_scrambler_get_scrambled_cube();

/* Parse Speffz notation. */
int cube_scrambler_parse_speffz(cube_t* c, char* s);

/* Returns a random DR */
cube_t cube_scrambler_get_scrambled_DR();
