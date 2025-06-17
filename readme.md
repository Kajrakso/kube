# kube

An optimal solver for the 3x3x3 Rubik's cube.

*Work in progress.*

### Usage

```
$ kube --help
Usage: kube [OPTION...] scramble
kube -- an optimal Rubik's cube solver

  -n NUM                     Try to find NUM solutions
  -v, --verbose              Produce verbose output
  -?, --help                 Give this help list
      --usage                Give a short usage message
  -V, --version              Print program version
```

### Build from source

Run

```sh
make
```

to build the project and 

```sh
make test
```
to run the tests (you need [Criterion](https://github.com/Snaipe/Criterion) for these).

To install on your system (only tested on linux) run

```sh
make install
```

and to uninstall run

```sh
make uninstall
```

## Inspiration

I've used [https://github.com/rokicki/cube20src](https://github.com/rokicki/cube20src),
[https://github.com/sebastianotronto/nissy-classic](https://github.com/sebastianotronto/nissy-classic)
and [https://github.com/sebastianotronto/h48](https://github.com/sebastianotronto/h48)
for inspiration.
Also, Jaap's site is absolutely worth a visit:
[https://www.jaapsch.net/puzzles/compcube.htm](https://www.jaapsch.net/puzzles/compcube.htm).

