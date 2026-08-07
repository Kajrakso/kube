# kube

An optimal solver for the 3x3x3 Rubik's cube.

### Usage

See manpage (`doc/kube.1`) or run `kube --help`.

```
$ kube --help
Usage: kube [OPTION...] [SCRAMBLE]
kube -- an optimal Rubik's cube solver

  -f, --format=FORMAT        Specify scramble format
  -g, --gen                  Generate tables
  -i, --stdin                Read scrambles from standard input
  -M, --max-depth=MAX        limit the search depth
  -n, --num=NUM              Try to find NUM solutions. When multiple steps are
                             given, kube does a beam search to find NUM
                             solutions.
  -s, --step=STEP            Append a solving step (ordered). Can be repeated.
                             Examples:
                               -s eo -s dr -s fin
  -t, --threads=NUM          specify number of threads to use during search.
                             defaults to number of cpus on the system
  -v, --verbose              Produce verbose output
  -?, --help                 Give this help list
      --usage                Give a short usage message
  -V, --version              Print program version

Mandatory or optional arguments to long options are also mandatory or optional
for any corresponding short options.

Report bugs to <oskarfj@oskarfj.no>.
`


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

