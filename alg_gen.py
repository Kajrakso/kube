import sys
from pathlib import Path

# Add the /python directory to sys.path
sys.path.append(str(Path(__file__).parent / "python"))

import libcube

scramble = "R2 U2 R2 U2 R2 U2"
generators = set(["U", "D", "R", "L", "F", "B"])

libcube.solve(
    scramble=scramble,
    number_of_solutions=8,
    generators=generators
)

