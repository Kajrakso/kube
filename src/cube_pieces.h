#ifndef _CUBE_PIECES_H
#define _CUBE_PIECES_H

typedef enum {
    UBL, UBR, UFR, UFL,
    DFL, DFR, DBR, DBL,
} corner;

typedef enum {
    UB, UR, UF, UL,
    DF, DR, DB, DL,
    BL, BR, FR, FL,
} edge;

#endif /* _CUBE_PIECES_H */