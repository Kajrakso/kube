#ifndef SYM_H
#define SYM_H

#include "cube.h"
#include "cube_operation.h"

#define SYM_I
enum symmetries {
    /* Naming: SYM_"UP-face""Front-face" */
    /* for the inversion ones I have not given them proper names yet. */
    SYM_uf = 0, SYM_ur, SYM_ub, SYM_ul,
    SYM_fr, SYM_rb, SYM_bl, SYM_lf, 
    SYM_ru, SYM_bu, SYM_lu, SYM_fu,
    SYM_lb, SYM_fl, SYM_rf, SYM_br,
    SYM_dl, SYM_df, SYM_dr, SYM_db,
    SYM_bd, SYM_ld, SYM_fd, SYM_rd,
    SYM_inversion0, SYM_inversion1, SYM_inversion2, SYM_inversion3,
    SYM_inversion4, SYM_inversion5, SYM_inversion6, SYM_inversion7,
    SYM_inversion8, SYM_inversion9, SYM_inversion10, SYM_inversion11,
    SYM_inversion12, SYM_inversion13, SYM_inversion14, SYM_inversion15,
    SYM_inversion16, SYM_inversion17, SYM_inversion18, SYM_inversion19,
    SYM_inversion20, SYM_inversion21, SYM_inversion22, SYM_inversion23
};

void do_y_rot(cube_t* cube);
void do_z_rot(cube_t* cube);
void do_inversion(cube_t* cube);

/* Doing an x rotation is just y z y'.
 It would be better to just precompute the multiplication table for the symmetries.
 */
void do_x_rot(cube_t* cube);

uint8_t get_inv_sym(uint8_t s);

/* Applies the symmetry sym to the cube.
This function should probably not be used.
Use `cube_operation_sym_conjugate` for conjugation
by a symmetry.*/
void cube_apply_symm(cube_t* cube, uint8_t sym);

#endif /* SYM_H */
