#pragma once
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include "onfoot_adapter.hpp"
#include "windows_onfoot.hpp"
#include "hands_channel.hpp"
#include "vr_overlay.hpp"
#include <memory>
#include <string>
#include <stdexcept>
namespace edup::vrinput::foot {
// Optional integration with ED-UP's existing owner thread. No new XR session.
class HostOnFoot {
 hands::Channel handsChannel_;bool handsEnabled_=false,overlayEnabled_=false,laserEnabled_=false,hapticsEnabled_=false;
 HapticLimiter hapticLimiter_;float hapticStrength_=.25f;
 hands::Snapshot overlay_{};XrTime overlayTime_=0;
 void clearHands(){overlay_={};overlayTime_=0;if(handsEnabled_)handsChannel_.write(hands::Snapshot{});}
 static hands::Pose convert(const XrPosef& p){return {{p.position.x,p.position.y,p.position.z},{p.orientation.x,p.orientation.y,p.orientation.z,p.orientation.w}};}
 void publishHands(XrSpace view,XrSpace base,XrTime t,bool pointing){
  if(!handsEnabled_&&!overlayEnabled_)return;hands::Snapshot out;XrPosef head{};
  if(core_.locateHead(view,base,t,head))for(unsigned i=0;i<2;++i){const auto& h=core_.hands()[i];if(h.tracked&&h.ready)hands::set(out,i,hands::make(convert(h.grip),convert(head),i==0,h.trigger,h.squeeze));}
  if(laserEnabled_&&pointing&&out.valid[0]&&out.valid[1]&&pointerRay(convert(core_.hands()[1].aim),convert(head),out.laser))out.laserValid=1;
  out.tick=GetTickCount64();
  if(overlayEnabled_){overlay_=out;overlayTime_=t;}
  if(handsEnabled_)handsChannel_.write(out);
 }
 InputCore core_;std::unique_ptr<WindowsBridge> bridge_;bool initialized_=false,attached_=false,mode_=false,f10WasDown_=true;
 static bool foregroundGame(){DWORD pid=0;auto w=GetForegroundWindow();if(!w)return false;GetWindowThreadProcessId(w,&pid);return pid==GetCurrentProcessId();}
public:
 static bool requested(){wchar_t v[4]{};return GetEnvironmentVariableW(L"EDUP_VR_ONFOOT",v,4)==1&&v[0]==L'1';}
 void attach(XrInstance instance,PFN_xrGetInstanceProcAddr get,XrSession session,bool plus){
  if(!requested()||initialized_)return;
  if(core_.initialize(instance,get,plus)!=XR_SUCCESS)return;
  initialized_=true;
  if(core_.attach(session)!=XR_SUCCESS)return;
  attached_=true;
  try{
   wchar_t exe[32768]{};const DWORD n=GetModuleFileNameW(nullptr,exe,32768);
   if(!n||n>=32768)throw std::runtime_error("game path unavailable");
   std::wstring path(exe,n);const auto sep=path.find_last_of(L"\\/");
   if(sep==std::wstring::npos)throw std::runtime_error("game directory unavailable");
   path.resize(sep+1);path+=L"edup-vr-tools.ini";
   const auto visual=loadVisualSettings(path.c_str());
   bridge_=std::make_unique<WindowsBridge>(GetCurrentProcessId(),loadToolBindings(path.c_str()),loadAimSettings(path.c_str()));
   if(visual.hands)handsEnabled_=handsChannel_.open(GetCurrentProcessId(),true);
   overlayEnabled_=visual.overlay;laserEnabled_=visual.laser;hapticsEnabled_=visual.haptics;hapticStrength_=visual.hapticStrength;
  }catch(...){bridge_.reset();OutputDebugStringW(L"ED-UP VR OnFoot: invalid configuration or bridge unavailable; disabled.\n");}
 }
 void suspend(){clearHands();mode_=false;f10WasDown_=true;core_.neutralize();if(bridge_)bridge_->stop();}
 void frame(bool eligible,bool focused,XrSpace base,XrTime t,XrSpace view){
  if(!bridge_||!attached_)return;
  if(!eligible||!focused||!foregroundGame()){suspend();return;}
  const bool down=(GetAsyncKeyState(VK_F10)&0x8000)!=0;
  if(down&&!f10WasDown_){mode_=!mode_;core_.neutralize();bridge_->submit(Sample{});}
  f10WasDown_=down;
  if(!mode_){clearHands();return;}
  if(core_.poll(true,base,t)!=XR_SUCCESS){suspend();return;}
  const auto report=bridge_->submit(fromCore(core_,true,true));
  if(hapticsEnabled_&&hapticStrength_>0&&foregroundGame()){const auto cue=cues(report.pressed);const auto now=GetTickCount64();
   if(cue.left&&hapticLimiter_.permit(0,now))core_.pulse(0,hapticStrength_);
   if(cue.right&&hapticLimiter_.permit(1,now))core_.pulse(1,hapticStrength_);
  }
  publishHands(view,base,t,report.pointing);
 }
 const hands::Snapshot* overlay(XrTime t,bool eligible,bool focused) const {
  return overlayEnabled_&&mode_&&eligible&&focused&&foregroundGame()&&overlayTime_==t&&hands::fresh(overlay_,GetTickCount64())?&overlay_:nullptr;
 }
 void releaseSession(){suspend();handsChannel_.close();handsEnabled_=false;overlayEnabled_=false;laserEnabled_=false;hapticsEnabled_=false;bridge_.reset();if(initialized_)core_.releaseSession();attached_=false;}
 void shutdown(){if(initialized_)core_.shutdown();initialized_=false;attached_=false;}
};
}
#endif
