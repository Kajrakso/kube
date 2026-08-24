from fastapi import FastAPI, HTTPException
from fastapi.responses import FileResponse, StreamingResponse
import asyncio
import json
import shlex
import logging

logging.basicConfig(level=logging.DEBUG)
log = logging.getLogger("kube")

app = FastAPI()


def build_cmd(data: dict):
    scramble = data.get("scramble")
    steps = data.get("steps")
    definitions = data.get("definitions", [])
    step_config = data.get("stepConfig", {})

    if not scramble:
        raise HTTPException(status_code=400, detail="Missing scramble")
    if not steps:
        raise HTTPException(status_code=400, detail="Missing steps")

    cmd = ["kube"]
    if data.get("verbose"):
        cmd.append("-v")

    for d in definitions:
        name = d.get("name", "")
        expr = d.get("expr", "")
        moveset = d.get("moveset", "")
        if not name or not expr:
            continue
        defn = f"{name}={expr}"
        if moveset:
            defn += f"@{moveset}"
        cmd += ["-D", defn]

    for s in steps:
        spec = s
        conf = step_config.get(s, {})
        opts = []
        if conf.get("max"):
            opts.append(f"max={conf['max']}")
        if conf.get("num") and conf["num"] != 1:
            opts.append(f"num={conf['num']}")
        if opts:
            spec += ":" + ",".join(opts)
        cmd += ["-s", spec]

    cmd.append(scramble)
    return cmd


@app.get("/")
def home():
    return FileResponse("index.html")


@app.post("/solve/stream")
async def solve_stream(data: dict):
    try:
        cmd = build_cmd(data)
    except HTTPException:
        raise

    log.info("Streaming: %s", " ".join(shlex.quote(c) for c in cmd))

    proc = await asyncio.create_subprocess_exec(
        *cmd,
        stdout=asyncio.subprocess.PIPE,
        stderr=asyncio.subprocess.PIPE,
    )

    async def generate():
        q = asyncio.Queue()

        async def read_stream(stream, event_type):
            while True:
                chunk = await stream.read(1024)
                if not chunk:
                    break
                await q.put({"type": event_type, "text": chunk.decode("utf-8", errors="replace")})
            await q.put({"type": f"{event_type}_done"})

        asyncio.create_task(read_stream(proc.stderr, "stderr"))
        asyncio.create_task(read_stream(proc.stdout, "stdout"))

        stdout_text = ""
        stderr_done = False
        stdout_done = False

        while not stdout_done:
            msg = await q.get()
            if msg["type"] == "stderr":
                yield f"data: {json.dumps(msg)}\n\n"
            elif msg["type"] == "stdout":
                stdout_text += msg["text"]
            elif msg["type"] == "stderr_done":
                stderr_done = True
            elif msg["type"] == "stdout_done":
                stdout_done = True

        await proc.wait()

        yield f"data: {json.dumps({'type': 'done', 'solution': stdout_text.strip(), 'returncode': proc.returncode})}\n\n"

    return StreamingResponse(
        generate(),
        media_type="text/event-stream",
        headers={"Cache-Control": "no-cache", "X-Accel-Buffering": "no"},
    )
