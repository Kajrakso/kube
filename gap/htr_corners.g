#!/usr/bin/gap

# Representing cp of the Rubik's cube as permutations on 8 points.
#
#                    +--------------+
#                    |              |
#                    |  1         2 |
#                    |              |
#                    |     top      |
#                    |              |
#                    |  4         3 |
#                    |              |
#     +--------------+--------------+--------------+--------------+
#     |              |              |              |              |
#     |              |              |              |              |
#     |              |              |              |              |
#     |     left     |    front     |    right     |     rear     |
#     |              |              |              |              |
#     |              |              |              |              |
#     |              |              |              |              |
#     +--------------+--------------+--------------+--------------+
#                    |              |
#                    | 5         6  |
#                    |              |
#                    |    bottom    |
#                    |              |
#                    | 8         7  |
#                    |              |
#                    +--------------+

# UBL <-> 1
# UBR <-> 2
# UFR <-> 3
# UFL <-> 4
# DFL <-> 5
# DFR <-> 6
# DBR <-> 7
# DBL <-> 8

U := (1, 2, 3, 4);
D := (5, 6, 7, 8);
R := (3, 2, 7, 6);
L := (4, 5, 8, 1);
F := (4, 3, 6, 5);
B := (2, 1, 8, 7);

U2 := U * U;
D2 := D * D;
R2 := R * R;
L2 := L * L;
F2 := F * F;
B2 := B * B;

UD := U * D;

# define the relevant groups
S8 := SymmetricGroup(8);
dr := Subgroup(S8, [U, D, R2, L2, F2, B2]);
htr_mod_E := Subgroup(dr, [U2, D2, R2, L2, F2, B2, UD]);

# find the dr subsets (which are double cosets)
dcs := DoubleCosets(dr, htr_mod_E, htr_mod_E);

# we make a dictionary where the key is cp,
# and the value is the double coset indexed by i.
cosetOf := NewDictionary(S8, true);
for i in [1..Length(dcs)] do
    for g in dcs[i] do
        AddDictionary(cosetOf, g, i);
    od;
od;


# log to two files, one with the cp <-> double coset dict,
# the other one with double coset <-> representative
# (so that I can see which dr subset each double coset is)
cp_to_dr_subset := OutputTextFile( "./cp_to_dr_subset", false );
dr_subsets_key := OutputTextFile( "./dr_subsets_key", false );
SetPrintFormattingStatus(cp_to_dr_subset, false);
SetPrintFormattingStatus(dr_subsets_key, false);

for i in Elements(dr) do
    PrintTo(cp_to_dr_subset, String(List([1..8], k -> k^i)), ": ", LookupDictionary(cosetOf, i), "\n");
od;

for dc in dcs do
    PrintTo(dr_subsets_key, LookupDictionary(cosetOf, Representative(dc)), ": ", Representative(dc), "\n");
od;
