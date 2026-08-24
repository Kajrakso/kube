# kube

An optimal solver for the 3x3x3 Rubik's cube.

kube finds a shortest solution (in half-turn metric) to any given scramble
using IDA\* search. For multi-step solves, it uses a beam search to combine
user-defined or built-in solving phases.

## Features

- **Optimal single-step solving** via IDA\* with pruning tables
- **Multi-step beam search** pipelines (e.g. `eo → dr → htr → fin`)
- **Custom solved states** through a built-in DSL with set/group algebra
- **Custom movesets** (HTM, QTM, or arbitrary face subsets)
- **Web interface** with visual DSL expression builder, drag-and-drop pipeline, and real-time streaming output
- **Multi-threaded** search
- **Multiple input formats**: Singmaster notation and Speffz (blindfold memo)

## Quick start

```sh
make
kube -s fin "R U F"
```

## Usage

```
kube [OPTION...] [SCRAMBLE]
```

| Option | Description |
|---|---|
| `-D`, `--define` | Define a custom solving step (see [Custom solved states](#custom-solved-states)) |
| `-f`, `--format=FORMAT` | Scramble format: `singmaster` (default) or `speffz` |
| `-g`, `--gen` | Pre-generate pruning tables |
| `-i`, `--stdin` | Read scrambles from stdin (newline-separated) |
| `-s`, `--step=STEP` | Append a solving step (repeatable). Options: `max=DEPTH`, `num=COUNT` |
| `-t`, `--threads=NUM` | Number of threads (default: system CPU count) |
| `-v`, `--verbose` | Verbose output |
| `-V`, `--version` | Print version |

### Examples

Optimal solve with a single step:

```sh
kube -s fin "R U F // comment"
```

Multi-step pipeline with depth limits:

```sh
kube -s eo -s dr -s fin "R' U' F L D2 B' U2 R2 B' D2 L2 B D2 L2 B2 R F D' B' D L2 B U R' U' F"
```

Find all sub-15 solutions for a Ua-perm (up to 100):

```sh
echo "R' U R' U' R' U' R' U R U R2" | kube -s fin:max=15,num=100 -v
```

Speffz blindfold memo format:

```sh
echo "ab." | kube -s fin -f speffz
```

## Custom solved states

kube includes a DSL for defining arbitrary solved states via the `-D` flag.
Atoms are primitives (`eofb`, `eolr`, `coud`, `ep`, `cp`, ...) optionally
restricted to pieces, combined with `&` (and), `|` (or), `!` (not), and
parentheses.

```sh
# F2L with U-layer corners oriented
kube -D 'f2l=solved:Dw & coud:U' -s eofb:max=5 -s f2l:max=15 "SCRAMBLE"
```

### Set and group atoms

Explicit state sets `{R, U2, e}` and generator-closed subgroups `<R, U>` are
supported, along with a product operator `A * B`:

```sh
# States reachable from solved:U by some element of <R,U>
kube -D 'step=solved:U * <R,U>' -s step "SCRAMBLE"
```

### Custom movesets

Append `@MOVESET` to restrict which moves are available:

```sh
kube -D 'fin_QTM=solved@QTM' -s fin_QTM "SCRAMBLE"
kube -D 'spam=solved:Dw & eofb & coud @ R,U,D' -s spam "SCRAMBLE"
```

See [`doc/custom_solved_states.md`](doc/custom_solved_states.md) for the full
DSL reference.

## Web interface

A browser-based UI is included in `web/`. It provides a visual expression
builder for custom solved states, drag-and-drop pipeline configuration, and
real-time streaming output.

```sh
cd web
uv run uvicorn server:app --host 0.0.0.0 --port 8000
```

Then open `http://localhost:8000` in your browser.

## Build from source

Requirements:

- A C compiler supporting C2x (e.g. GCC 14+)
- GNU Make
- [Criterion](https://github.com/Snaipe/Criterion) (for tests only)

```sh
make            # build
make test       # run tests
make install    # install to /usr/local (Linux only)
make uninstall  # uninstall
```

## Inspiration

- [cube20src](https://github.com/rokicki/cube20src) by Tomas Rokicki
- [nissy-classic](https://github.com/sebastianotronto/nissy-classic) by Sebastiano Tronto
- [h48](https://github.com/sebastianotronto/h48) by Sebastiano Tronto
- [Jaap's Puzzle Page](https://www.jaapsch.net/puzzles/compcube.htm)

## License

[MIT](LICENSE)

