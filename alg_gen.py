import sys
from pathlib import Path

# Add the /python directory to sys.path
sys.path.append(str(Path(__file__).parent / "python"))

import libcube

@libcube.solver_env
def alg_genner(scr):
  solutions = libcube.solve(
      scr,
      number_of_solutions=20
  )

  for _solution in solutions:
     print(_solution)


if __name__ == "__main__":
    alg_genner(scr = "R' U R U R' F' R U R' U' R' F R2 U' R' U2 R U' R' U2 R")
