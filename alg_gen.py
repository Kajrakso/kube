import sys
from pathlib import Path

# Add the /python directory to sys.path
sys.path.append(str(Path(__file__).parent / "python"))

import libcube


@libcube.solver_env
def alg_genner():

    libcube.solve("R")


if __name__ == "__main__":
    alg_genner()
