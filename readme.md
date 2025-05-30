# libcube

*Work in progress.*

## Goal

Learn to build something in C and to make a simple and fast Rubik's cube library.

## Features

- cube states (eo, co, dr, etc.)
- cube moves and symmetries
- scrambler
- optimal solver (WIP)
- python wrapper (WIP)
- state generator (TODO)
- other optimal solvers (other metrics, solve to specific subgroups)  (TODO)

### Usage

Run

```sh
make
```

to build the library and

```sh
make test
```
to run the tests. See `main.c` for example usage of libcube.

To build the python module run

```sh
make pylibcube
```

See `alg_gen.py` for example usage.

## Project structure:

- `include/`
    Headers.
- `python/`
    Python wrapper.
- `doc/`
    Documentation.
- `src/`
    Source code duh.
- `tests/`
    I have used [Criterion](https://github.com/Snaipe/Criterion) for unit testing.

## Inspiration

I've used [https://github.com/rokicki/cube20src](https://github.com/rokicki/cube20src),
[https://github.com/sebastianotronto/nissy-classic](https://github.com/sebastianotronto/nissy-classic)
and [https://github.com/sebastianotronto/h48](https://github.com/sebastianotronto/h48)
for inspiration.
Also, Jaap's site is absolutely worth a visit:
[https://www.jaapsch.net/puzzles/compcube.htm](https://www.jaapsch.net/puzzles/compcube.htm).
