# Custom solved states

I thought it would be cool if the user could specify which states
they consider as "solved" instead of relying on built in `solving_steps`
such as `eo`, `dr`, `htr` or `fin`. The user specified their custom solved
state through a *Domain Specific Language* (DSL). 

## Motivation and desciption of the usage of DSL:

The DSL (Domain Specific Language) is used for user-defined solving steps.
The idea is that the user wants an optimal solution to some "weird" state,
that is not captured by the basic solving steps eo, dr, htr and fin.
Via the CLI argument -D (--define), the user provides first a name for
the custom solving step, and then describes it using
primitives (eolr, eoud, cp, ep, etc.), pieces or
piece groups (faces) (UBR, UF, FR, L, Dw, M, etc.) and
simple operators (and (&), or (|) and not (!)). The solving step is
then used by the solver in a pipeline (with further configuration possible).

Primitives: eofb, eolr, eoud, cofb, colr, coud, ep, cp
Pieces: UBL, UBR, ..., DBR, DBL, UB, UR, UF, UL, DF, ... FR, FL
Combinations of pieces: M, S, E, Dw, Uw, ..., Rw, Lw, U, Uc (all corners on U), Ue (all edges on U)
Usage: To define an "atom" (the building block of a custom solved state) use PRIMITIVE or PRIMITIVE:PIECE/COMBINATION OF PIECES.
A primitive written without :PIECES refers to all pieces (e.g. solved == all pieces solved).
Where combination of pieces is a predefined one, or pieces separated by commas. To define a custom solved state to

    --define 'NAME=ATOM'

Atoms are combined using the simple operators above in addition to parentheses. Example:

    --define 'NAME=ATOM1 & ATOM2 & (ATOM3 | !ATOM4)'

The order of operations are (), !, *, &, | where * is the set/group product:

    --define 'NAME=A1 * A2'

denotes the product {a1*a2 : a1 in A1's states, a2 in A2's states} (apply a1,
then a2). At least one operand of * must be a set atom ({..} or <..>); if
both are sets the product is materialized eagerly. Example: 'solved:U * <R,U>'
is the set of states that solve to solved:U by some element of <R,U>.

Set atoms: {R, U2, e} (exactly the listed elements, identity only when
spelled out; each element may be a move sequence like R U R') and <R, U>
(closure of the given generators, a subgroup).
A set atom evaluates to true when the cube state is a member of the set.

Example: To define a solving step for f2l, with the corners on the U-layer permuted (but might not be oriented)
and then solve a scramble optimally (first to eo on the FB axis, then to the user defined state), do either:

    kube --define 'f2l_c_orien=solved:Dw&coud:U' --step eofb:max=5 --step f2l_c_orien:max=15 "SCRAMBLE"
    kube -D 'f2l_c_orien=solved:Dw&coud:U' -s eofb:max=5 -s f2l_c_orien:max=15 "SCRAMBLE"

kube also supports the use of custom movesets. This is done as follows:

    kube --define 'NAME=EXPRESSION@MOVESET'

The MOVESET is a list of faces of which we support doing moves. Also, HTM and QTM are valid options. Examples:

    kube --define 'fin_QTM=solved@QTM'
    kube --define 'OLL_RUD_spam=solved:Dw & eofb & coud @ R,U,D'

### Pruning tables:

In order to speed up the solving process, kube will automatically try to prune the search somehow.
To do this, tables have to be generated. 
n version 1.0.0 this is done on demand, and then saved to disk with a filename containing a hash 
the pieces used to prune. This way, it will be reused
later. The user should also be able to pregenerate some predefined tables with

    kube -g "all"

or something of that sort, but this is not supported yet.

## TODO: Config files:

In the future it would make sense to be able to define solving steps in a config file instead of
via --define  (or -D) everytime. This config file could then contain the eo, dr, htr solving steps
that currently is defined in solver_steps.c.

## TODO: Reusing existing definitions.
 would be super cool if the user can combine already defined solving states to ease the definitions.
Example:

    kube -D 'f2l=solved:Dw' -D 'step_name:f2l & eofb & solved:UFR,UFL,UBL,UBR'

## TODO: Exploiting symmetries in definitions.
 the user wants a very general case (like xcross on any face and any pair), then maybe we could
add symmetry actions to the definitions?
Example:

    kube -D 'any_cross:solved=De | solved=Ue | solved=Le | solved=Re | solved=Fe | solved=Be'

could instead be something along the lines of:

    kube -D 'any_cross:solved=De+S4'

encoding that we act on the state "solved=De" with the symmetry group of the cube (which is S4).
TODO: How to encode the different symmetries of the cube?

## TODO: is the definition actually solvable?

Actually checking if a custom solved state is solvable is not trivial. For the moment there is no check for solvability.
