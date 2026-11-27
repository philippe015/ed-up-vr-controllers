"""Synthetic packaging fixture. Never publish or install these files into a real game."""
import sys
import zipfile
import subprocess
from pathlib import Path
import stage_full

root=Path(sys.argv[1]).resolve()
root.mkdir(parents=True,exist_ok=False)
module=Path(__file__).resolve().parents[1]
archive=root/"synthetic.zip"
with zipfile.ZipFile(archive,"w") as z:
    for n in [*stage_full.REQUIRED,*stage_full.NOTICES,*stage_full.OPTIONAL]:
        z.writestr(n,("SYNTHETIC TEST ONLY: "+n+"\n").encode("ascii"))
provenance=root/"provenance.txt"
provenance.write_text("SYNTHETIC TEST ONLY - NOT A RELEASE\n",encoding="ascii")
subprocess.run([sys.executable,str(module/"installer"/"stage_full.py"),str(archive),str(root/"package"),str(module),str(provenance)],check=True)
