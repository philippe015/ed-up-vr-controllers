#!/usr/bin/env python3
"""Experimental integration against inspected upstream main. Abort if anchors changed."""
import argparse
from pathlib import Path
p=argparse.ArgumentParser();p.add_argument('repo',type=Path);p.add_argument('--check',action='store_true');args=p.parse_args()
root=args.repo
host=root/'src/openxr/native_runtime_host.h';build=root/'build.bat';stereo=root/'src/openxr/d3d11_stereo.cpp'
# Preserve exact newline convention on disk.
def read(path):
    raw=path.read_bytes();return raw.decode('utf-8').replace('\r\n','\n'), b'\r\n' in raw
h,hn=read(host);b,bn=read(build);d,dn=read(stereo)
def one(text,old,new):
    if text.count(old)!=1: raise SystemExit('Ancre absente ou ambiguë : '+old[:120]+' — aucun fichier modifié.')
    return text.replace(old,new,1)
def block(code):return '\n#ifdef EDUP_ENABLE_ONFOOT\n'+code+'\n#endif\n'
h=one(h,'#include <mutex>\n','#include <mutex>\n'+block('#include "../../vr-controllers/host_onfoot.hpp"'))
h=one(h,'  NativeDevice graphics; SessionBinding binding; D3D11Stereo stereo; SessionState state;','  NativeDevice graphics; SessionBinding binding; D3D11Stereo stereo; SessionState state;'+block('  edup::vrinput::foot::HostOnFoot vrOnFoot;\n  bool vrTouchPlus=false;'))
h=one(h,'    bool d3d=false,timeConversion=false;','    bool d3d=false,timeConversion=false;'+block('    vrTouchPlus=false;'))
h=one(h,'    for(const auto& e:extensions) {\n      d3d|=', '    for(const auto& e:extensions) {'+block('      vrTouchPlus|=edup::vrinput::foot::HostOnFoot::requested()&&std::strcmp(e.extensionName,"XR_META_touch_controller_plus")==0;')+'      d3d|=')
h=one(h,'    const char* enabled[6]={','    const char* enabled[7]={')
h=one(h,'    ci.enabledExtensionCount=enabledCount;ci.enabledExtensionNames=enabled;',block('    if(vrTouchPlus)enabled[enabledCount++]="XR_META_touch_controller_plus";')+'    ci.enabledExtensionCount=enabledCount;ci.enabledExtensionNames=enabled;')
h=one(h,'    session=binding.session();local=binding.naturalSpace();xrLocal=binding.localSpace();view=binding.viewSpace();','    session=binding.session();local=binding.naturalSpace();xrLocal=binding.localSpace();view=binding.viewSpace();'+block('    vrOnFoot.attach(instance,api.get,session,vrTouchPlus);'))
h=one(h,'    poses.focus(compositorGeneration,true,state.running()&&!state.terminal()&&lifecycle==Lifecycle::Focused);','    poses.focus(compositorGeneration,true,state.running()&&!state.terminal()&&lifecycle==Lifecycle::Focused);'+block('    if(r!=XR_SUCCESS||!state.running()||state.terminal()||lifecycle!=Lifecycle::Focused)vrOnFoot.suspend();'))
h=one(h,'    render.time=frame.predictedDisplayTime;game.pose=invalidHeadPose(true);',block('    {const SeatContext c=decodeSeatContext(seatContextWord());\n     vrOnFoot.frame(c.published&&c.gameplay&&c.onFootKnown&&c.onFoot&&!c.cameraOpen,\n       state.lifecycle()==Lifecycle::Focused,xrLocal,frame.predictedDisplayTime,view);}')+'    render.time=frame.predictedDisplayTime;game.pose=invalidHeadPose(true);')
h=one(h,'    if(applied) {\n      referenceChanges+=applied;', '    if(applied) {'+block('      vrOnFoot.suspend();')+'      referenceChanges+=applied;')
h=one(h,'    auto fail=[&](XrResult error){timingInvalidate();lastCompositorResult=error;poses.invalidate(generation);menu.invalidate();invalidateEyeTreatments();', '    auto fail=[&](XrResult error){'+block('      vrOnFoot.suspend();')+'      timingInvalidate();lastCompositorResult=error;poses.invalidate(generation);menu.invalidate();invalidateEyeTreatments();')
h=one(h,'  bool close() {\n    clearOverlapHandoff();','  bool close() {\n    clearOverlapHandoff();'+block('    vrOnFoot.suspend();'))
h=one(h,'    const auto bindingResult=binding.shutdown();',block('    vrOnFoot.releaseSession();')+'    const auto bindingResult=binding.shutdown();')
h=one(h,'    if(instance && api.destroyInstance) {',block('    vrOnFoot.shutdown();')+'    if(instance && api.destroyInstance) {')
old='    /Fe"%BUILD%\\edvr_openxr_runtime.dll" "src\\openxr\\native_module.cpp" ^'
new='    /DEDUP_ENABLE_ONFOOT=1 ^\n'+old+'\n    "vr-controllers\\input_core.cpp" "vr-controllers\\windows_onfoot.cpp" ^'
b=one(b,old,new)
h=one(h,'    const auto r=frameWithheld?stereo.renderCaptured(',block('    const SeatContext vrOverlayContext=decodeSeatContext(seatContextWord());\n    edup::vrinput::hands::MenuReceipt vrMenuDraw;\n    const auto* vrMenuSnapshot=frameWithheld?nullptr:vrOnFoot.overlay(boundary.frame().predictedDisplayTime,\n       vrOverlayContext.published&&vrOverlayContext.gameplay&&vrOverlayContext.onFootKnown&&vrOverlayContext.onFoot&&!vrOverlayContext.cameraOpen,\n       state.lifecycle()==Lifecycle::Focused);\n    edup::vrinput::hands::OverlayScope vrOverlayScope(vrMenuSnapshot,&vrMenuDraw);')+'    const auto r=frameWithheld?stereo.renderCaptured(')
h=one(h,'    sharePress(leftShare,r==XR_SUCCESS,frameWithheld);',block('    vrOnFoot.recordMenuDraw(vrMenuSnapshot,vrMenuDraw,boundary.frame().predictedDisplayTime,r==XR_SUCCESS&&!frameWithheld);')+'    sharePress(leftShare,r==XR_SUCCESS,frameWithheld);')
d=one(d,'#include "plugin_frame_args.h"\n','#include "plugin_frame_args.h"\n'+block('#include "../../vr-controllers/vr_overlay.hpp"'))
d=one(d,'      if(fade>0.f) {\n        // The same triangle again under the fade blend:',block('      if(fade==0.f&&!ringOn&&!leftZoomed&&!blackLeft&&!shareOn&&!mirrorOn&&headPose&&poseValid(*headPose))\n        edup::vrinput::hands::drawOverlay(device_.Get(),drawContext,rtv,*headPose,views[i],eyes_[i].width,eyes_[i].height,i);')+'      if(fade>0.f) {\n        // The same triangle again under the fade blend:')
if not args.check:
    for path,content,crlf in ((host,h,hn),(build,b,bn),(stereo,d,dn)):
        path.write_bytes((content.replace('\n','\r\n') if crlf else content).encode('utf-8'))
print('Ancres vérifiées ; '+('aucune modification (--check)' if args.check else 'intégration expérimentale appliquée'))
