import sys
from pathlib import Path

# Add the /python directory to sys.path
sys.path.append(str(Path(__file__).parent / "python"))

import libcube

@libcube.solver_env
def alg_genner(scr):
  solutions = libcube.solve(
      scr,
      number_of_solutions=10
  )

  for _solution in solutions:
     print(_solution)


if __name__ == "__main__":
    alg_genner(scr = "F2 L2 D2 F2 D L2 D' L2 U' F2 R2 F R' B2 D' U L' F L B")
