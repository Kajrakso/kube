# libcube

*Work in progress.*

---

## Goal

Learn to build something in C.

The objective is to make a simple and fast Rubik's cube library.
It should support moving the the cube, keeping track of oriented
and misoriented pieces as well as functionality for checking if the
cube is in a certain state (DR, HTR etc.).

Maybe I will create scramblers, solvers and so on
that builds on this in the future.

---

## Inspiration

I've used https://cube20.org/src/ and https://nissy.tronto.net/ for inspiration.


## Project structure:

- `src/`
    Source code duh.
- `tests/`
    I have used [Criterion](https://github.com/Snaipe/Criterion) for unit testing.

---

## Cube representation

todo

// Representing the Rubik's cube as permutations of corners and edges:
//
//                    +--------------+
//                    |              |
//                    |  1    1    2 |
//                    |              |
//                    |  4  top    2 |
//                    |              |
//                    |  4    3    3 |
//                    |              |
//     +--------------+--------------+--------------+--------------+
//     |              |              |              |              |
//     |              |              |              |              |
//     |              |              |              |              |
//     | 9   left  12 |    front     | 11 right  10 |     rear     |
//     |              |              |              |              |
//     |              |              |              |              |
//     |              |              |              |              |
//     +--------------+--------------+--------------+--------------+
//                    |              |
//                    |  5    5    6 |
//                    |              |
//                    |  8 bottom  6 |
//                    |              |
//                    |  8    7    7 |
//                    |              |
//                    +--------------+
// and orientation of corners and edges:
//
// cofb: 0, 1, 2. co is invariant under <U, D, R2 L2>.
//      0: oriented
//      1: clockwise
//      2: counter clockwise.
// eofb: 0, 1. eo is invariant under <U, D, R, L, F2, B2>.
//      0: oriented
//      1: misoriented.
// 
// similar for lr and ud.


