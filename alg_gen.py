import sys
from pathlib import Path

# Add the /python directory to sys.path
sys.path.append(str(Path(__file__).parent / "python"))

import libcube


@libcube.solver_env
def alg_genner():
    solutions = []

    solutions = libcube.solve(
        "R' U R U R' F' R U R' U' R' F R2 U' R' U2 R U' R' U2 R",
        number_of_solutions=50,
        generators=set({"R", "U", "F"}),
        verbose=True,
    )

    # print
    for _solution in solutions:
        print(_solution)


if __name__ == "__main__":
    alg_genner()
