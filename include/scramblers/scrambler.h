#ifndef _SCRAMBLER_H_
#define _SCRAMBLER_H_

#include <stdlib.h>

#include "../core/cube.h"
#include "../core/move.h"

/* Just does 26 random moves to the cube. Use
 * ``cube_scrambler_get_scrambled_cube`` for a random state. */
cube_t cube_scrambler_get_handscrambled_cube();

/* Returns a completely random cube state. */
cube_t cube_scrambler_get_scrambled_cube();

#endif /* _SCRAMBLER_H_ */
