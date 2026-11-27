"""Build an offline VR-only payload from the upstream validated native archive.
No upstream updater, unsigned update manifests, flat profile or optional EDHM.
"""
import argparse, hashlib, json, shutil, zipfile
from pathlib import Path
REQUIRED = {
    "d3d11.dll":"d3d11.dll",
    "openvr/openvr_api.dll":"Openvr/win64/openvr_api.dll",
    "openvr/openxr_loader.dll":"Openvr/win64/openxr_loader.dll",
    "openvr/OPENXR-LOADER-LICENSE.txt":"Openvr/win64/OPENXR-LOADER-LICENSE.txt",
    "edup.ini":"edup.ini",
    "edup_profile.ini":"edup_profile.ini",
    "DITTOSR-LICENSE.txt":"DITTOSR-LICENSE.txt",
    "DITTOSR-GSR-LICENSE.txt":"DITTOSR-GSR-LICENSE.txt",
    "libxess.dll":"libxess.dll",
    "INTEL-XESS-LICENSE.txt":"INTEL-XESS-LICENSE.txt",
    "INTEL-XESS-THIRD-PARTY.txt":"INTEL-XESS-THIRD-PARTY.txt",
    "amd_fidelityfx_loader_dx12.dll":"amd_fidelityfx_loader_dx12.dll",
    "amd_fidelityfx_upscaler_dx12.dll":"amd_fidelityfx_upscaler_dx12.dll",
    "AMD-FIDELITYFX-LICENSE.txt":"AMD-FIDELITYFX-LICENSE.txt",
}
NOTICES = ["LICENSE.txt","DXBC-HASH-LICENSE.txt","MINIZ-LICENSE.txt","MIN-ED-LAUNCHER-LICENSE.txt"]
OPTIONAL = ["nvngx_dlss.dll","NVIDIA-DLSS-LICENSE.txt","FIDELITYFX-SDK-DX11-LICENSE.txt"]
def sha(p):
    return hashlib.sha256(p.read_bytes()).hexdigest()
def main():
    p=argparse.ArgumentParser()
    p.add_argument("archive",type=Path); p.add_argument("output",type=Path)
    p.add_argument("module",type=Path); p.add_argument("provenance",type=Path)
    a=p.parse_args()
    a.output.mkdir(parents=True,exist_ok=False)
    payload=a.output/"payload"; payload.mkdir()
    items=dict(REQUIRED)
    with zipfile.ZipFile(a.archive) as z:
        names=z.namelist()
        if len(names)!=len(set(x.casefold() for x in names)):
            raise ValueError("duplicate archive member")
        for n in OPTIONAL:
            if n in names: items[n]=n
        if ("nvngx_dlss.dll" in items)!=("NVIDIA-DLSS-LICENSE.txt" in items):
            raise ValueError("DLSS notice mismatch")
        for n in NOTICES:
            items[n]="EDUP-VR-Notices/"+n
        for source,dest in items.items():
            member=z.getinfo(source)
            if member.file_size<=0 or member.file_size>512*1024*1024:
                raise ValueError("unexpected member size "+source)
            path=payload/dest;path.parent.mkdir(parents=True,exist_ok=True)
            with z.open(member) as src,path.open("xb") as out: shutil.copyfileobj(src,out)
    shutil.copyfile(a.module/"edup-vr-tools.ini",payload/"edup-vr-tools.ini")
    shutil.copyfile(a.module/"LICENSE",payload/"EDUP-VR-Notices/VR-LICENSE.txt")
    shutil.copyfile(a.module/"README.md",a.output/"VR-README.md")
    shutil.copyfile(a.provenance,a.output/"build-provenance.txt")
    files=[{"path":p.relative_to(payload).as_posix(),"sha256":sha(p)} for p in sorted(payload.rglob("*")) if p.is_file()]
    (a.output/"manifest.json").write_text(json.dumps({"schema":1,"files":files},indent=2),encoding="ascii")
    for name in ["install-patch.ps1","Installer.cmd","Restaurer.cmd","FULL-INSTALL-GUIDE.md"]:
        shutil.copyfile(a.module/"installer"/name,a.output/name)
if __name__=="__main__": main()
