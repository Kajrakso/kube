# Cube representation

We represent the Rubik's cube as permutations and orientations of corners pieces and edge pieces.
The following diagram illustrates the numbering of the pieces:

```
                   +--------------+
                   |              |
                   |  0    0    1 |
                   |              |
                   |  3    U    1 |
                   |              |
                   |  3    2    2 |
                   |              |
    +--------------+--------------+--------------+--------------+
    |              |              |              |              |
    |              |              |              |              |
    |              |              |              |              |
    | 8     L   11 |      F       | 10    R    9 |       B      |
    |              |              |              |              |
    |              |              |              |              |
    |              |              |              |              |
    +--------------+--------------+--------------+--------------+
                   |              |
                   |  4    4    5 |
                   |              |
                   |  7    D    5 |
                   |              |
                   |  7    6    6 |
                   |              |
                   +--------------+
```

## Permutation of corners and edges:

- corners: 0 to 7
- edges: 0 to 11

## Orientation of corners and edges:

- corners:
    - 0: oriented, 1: clockwise, 2: counter clockwise.
    
    - three axis:
        - `cofb`: invariant under `<U2, D2, R2, L2,  F,  B>`.
        - `colr`: invariant under `<U2, D2,  R,  L, F2, B2>`.
        - `coud`: invariant under `< U,  D, R2, L2, F2, B2>`.


- edges:
    - 0: oriented, 1: misoriented.
    - three axis:
        - `eofb`: invariant under `< U,  D,  R,  L, F2, B2>`.
        - `eolr`: invariant under `< U,  D, R2, L2,  F,  B>`.
        - `eoud`: invariant under `<U2, D2,  R,  L,  F,  B>`.
