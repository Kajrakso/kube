from fastapi import FastAPI, HTTPException
from fastapi.responses import FileResponse
import subprocess

app = FastAPI()

@app.get("/")
def home():
    return FileResponse("index.html")

@app.post("/solve")
def solve_cube(data: dict):
    scramble = data.get("scramble")
    steps = data.get("steps")

    if not scramble:
        raise HTTPException(status_code=400, detail="Missing scramble")


    if not steps or steps == []:
        raise HTTPException(status_code=400, detail="Missing steps")

    cmd = ["kube"]
    for s in steps:
        cmd += ["--step", s]

    try:
        result = subprocess.run(
            cmd,
            input=scramble,
            capture_output=True,
            text=True,
        )

        if result.returncode != 0:
            raise Exception(result.stderr)

        return {"solution": result.stdout.strip()}

    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))
