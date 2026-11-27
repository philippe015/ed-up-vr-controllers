#pragma once
#include <array>
#include <bitset>
#include <cmath>
#include <algorithm>
#include <cstdint>
#include "safety.hpp"
#include "stability.hpp"
#include "motion_aim.hpp"
#include "two_hand_pose.h"
#include "equipment_gestures.hpp"
#include "radial_menu.hpp"
#include "wrist_panel.hpp"
namespace edup::vrinput::foot {
enum Action {Forward, Back, Left, Right, Jump, Interact, Crouch, Sprint, CycleTool, Reload, Primary, Secondary, EnergyLink, ProfileAnalyser, SuitTool, Holster, Flashlight, ToolMode, SelectPrimaryWeapon, SelectSecondaryWeapon, Count};
using Keys=std::bitset<Count>;
struct Hand {bool ready=false, primary=false, secondary=false, stickClick=false; float x=0,y=0,trigger=0,squeeze=0; Orientation aim{}; edup::experimental::Vec3 gripPosition{}; bool gripTracked=false;};
struct Sample {std::array<Hand,2> hand{}; bool enabled=false, focused=false; hands::Pose head{}; bool headTracked=false;};
enum class ControlMode : uint32_t { Neutral=0, Normal=1, Tools=2, Motion=3, TwoHand=4, Snap=5 };
struct Output {Keys held{}; int dx=0,dy=0; bool pointing=false; ControlMode mode=ControlMode::Neutral; bool radialVisible=false;int radialIndex=-1;};
struct Settings {float moveOn=.55f,moveOff=.35f,triggerOn=.65f,triggerOff=.40f; float lookPixelsPerSecond=900.f; bool invertY=false; bool toolLayer=true; bool motionAim=true; float aimPixelsPerDegree=10.f; bool twoHandAim=false; bool leftHanded=false; unsigned snapMode=0; unsigned snapPixelsPerDegree=10; unsigned extraLookDeadzonePercent=0; bool strictTools=false; bool weaponSelect=false; bool equipmentGestures=false; bool radialMenu=false; bool wristMenu=false; unsigned equipmentScalePercent=100;};
class Mapper {
 Settings cfg_; Keys held_; bool armed_=false, tools_=false, layerReady_=false, chordBlocked_=false; float remX_=0,remY_=0; MotionAim motion_; bool clutch_=false; edup::experimental::TwoHandFilter twoFilter_; MotionAim twoMotion_; bool twoActive_=false; double twoTime_=0; bool snapReady_=false; bool weaponBusy_=false,weaponConsumed_=false; EquipmentGesture equipment_; bool gestureLocked_=false,gestureInhibit_=false; EquipmentZone gestureZone_=EquipmentZone::None; Action gestureAction_=Count; float gesturePulse_=0; RadialMenu radial_; Action radialAction_=Count;float radialPulse_=0;
 bool hysteresis(Action a,float value,float on,float off) const {return value>=(held_[a]?off:on);}
 static bool valid(const Hand& h){return h.ready&&analogValid(h.x,-1.f,1.f)&&analogValid(h.y,-1.f,1.f)&&analogValid(h.trigger,0.f,1.f)&&analogValid(h.squeeze,0.f,1.f);}
 static bool neutral(const Hand& h){return std::abs(h.x)<.10f&&std::abs(h.y)<.10f&&h.trigger<.05f&&h.squeeze<.05f&&!h.primary&&!h.secondary&&!h.stickClick;}
public:
 explicit Mapper(Settings cfg={}):cfg_(cfg){
  auto safe=[](float x,float fallback){return std::isfinite(x)?x:fallback;};
  cfg_.moveOn=std::clamp(safe(cfg_.moveOn,.55f),.15f,1.f);cfg_.moveOff=std::clamp(safe(cfg_.moveOff,.35f),.10f,cfg_.moveOn);
  cfg_.triggerOn=std::clamp(safe(cfg_.triggerOn,.65f),.15f,1.f);cfg_.triggerOff=std::clamp(safe(cfg_.triggerOff,.40f),.05f,cfg_.triggerOn);
  cfg_.lookPixelsPerSecond=std::clamp(safe(cfg_.lookPixelsPerSecond,900.f),0.f,3000.f);
  cfg_.snapMode=(std::min)(cfg_.snapMode,3u);cfg_.snapPixelsPerDegree=std::clamp(cfg_.snapPixelsPerDegree,1u,30u);cfg_.extraLookDeadzonePercent=(std::min)(cfg_.extraLookDeadzonePercent,40u);
  cfg_.equipmentScalePercent=std::clamp(cfg_.equipmentScalePercent,70u,130u);
  cfg_.aimPixelsPerDegree=std::clamp(safe(cfg_.aimPixelsPerDegree,10.f),1.f,30.f);
 }
 Output reset(){held_.reset();armed_=false;tools_=layerReady_=chordBlocked_=false;remX_=remY_=0;motion_.reset();clutch_=false;twoFilter_.reset();twoMotion_.reset();twoActive_=false;twoTime_=0;snapReady_=false;weaponBusy_=weaponConsumed_=false;equipment_.reset();gestureLocked_=gestureInhibit_=false;gestureZone_=EquipmentZone::None;gestureAction_=Count;gesturePulse_=0;radial_.reset();radialAction_=Count;radialPulse_=0;return {};}
 Output update(const Sample& s,float dt){
  const auto& l=s.hand[cfg_.leftHanded?1:0];const auto& r=s.hand[cfg_.leftHanded?0:1];
  if(!s.enabled||!s.focused||!valid(l)||!valid(r)||!std::isfinite(dt)||dt<0||dt>=.1f)return reset();
  if(!armed_){armed_=neutral(l)&&neutral(r);layerReady_=armed_;return {};}
  // Experimental mode: dominant grip first, then support grip. Support grip alone selects tools.
  if(twoActive_ || (cfg_.twoHandAim && cfg_.motionAim && !tools_ && layerReady_ && clutch_ && r.squeeze>=.70f && l.squeeze>=.70f)){
   if(!twoActive_){
    twoActive_=true;twoTime_=0;twoFilter_.reset();twoMotion_.reset();
    twoFilter_.update(r.gripPosition,l.gripPosition,r.gripTracked,l.gripTracked,true,false,0);
   }
   if(l.squeeze<.40f || r.squeeze<.40f || dt<=0 || l.trigger>=.05f || r.trigger>=.05f || l.primary || l.secondary || r.primary || r.secondary || l.stickClick || r.stickClick)return reset();
   twoTime_+=dt;
   auto pose=twoFilter_.update(r.gripPosition,l.gripPosition,r.gripTracked,l.gripTracked,true,true,twoTime_);
   if(!pose.valid)return reset();
   const auto v=pose.direction;
   const double horizontal=std::hypot(v.x,v.z);
   if(horizontal<.17)return reset();
   const double yaw=-std::atan2(v.x,-v.z)/2, pitch=std::atan2(v.y,horizontal)/2;
   Orientation q{float(std::cos(yaw)*std::sin(pitch)),float(std::sin(yaw)*std::cos(pitch)),float(-std::sin(yaw)*std::sin(pitch)),float(std::cos(yaw)*std::cos(pitch)),true};
   auto aim=twoMotion_.update(true,q,cfg_.aimPixelsPerDegree,cfg_.invertY,dt);
   if(!aim.usable)return reset();
   held_.reset();remX_=remY_=0;snapReady_=false;
   Output out;out.dx=aim.dx;out.dy=aim.dy;out.mode=ControlMode::TwoHand;return out;
  }
  // Left grip is a physical modifier, not a game action. Hysteresis prevents chatter.
  const bool nextTools=cfg_.toolLayer && l.squeeze>=(tools_?.40f:.70f);
  if(nextTools!=tools_){tools_=nextTools;layerReady_=false;chordBlocked_=false;weaponBusy_=weaponConsumed_=false;equipment_.reset();gestureLocked_=gestureInhibit_=false;gestureZone_=EquipmentZone::None;gestureAction_=Count;gesturePulse_=0;radial_.reset();radialAction_=Count;radialPulse_=0;held_.reset();remX_=remY_=0;motion_.reset();clutch_=false;snapReady_=false;return {};}
  auto layerNeutral=[&]{Hand left=l;if(tools_)left.squeeze=0;return neutral(left)&&neutral(r);};
  if(!layerReady_){layerReady_=layerNeutral();return {};}
  if(tools_){
   // Never carry ordinary actions (including fire) into the tool layer.
   const std::array<bool,6> buttons={l.primary,l.secondary,r.primary,r.secondary,l.stickClick,r.stickClick};
   const std::array<Action,6> actions={EnergyLink,ProfileAnalyser,SuitTool,Holster,Flashlight,ToolMode};
   unsigned n=0,selected=0;for(unsigned i=0;i<buttons.size();++i)if(buttons[i]){++n;selected=i;}
   // Optional radial replaces stick weapon selection; button/gesture paths remain available.
   if((cfg_.radialMenu||cfg_.wristMenu)&&(radial_.active()||(!gestureLocked_&&r.squeeze<.05f&&(std::abs(r.x)>=.20f||std::abs(r.y)>=.20f)))){
    Output o;o.mode=ControlMode::Tools;
    hands::Vec anchor{};
    const bool wristOk=!cfg_.wristMenu||(s.headTracked&&l.gripTracked&&hands::wristAnchor({float(l.gripPosition.x),float(l.gripPosition.y),float(l.gripPosition.z)},s.head,anchor));
    const bool clean=wristOk&&s.headTracked&&l.trigger<.05f&&r.trigger<.05f&&r.squeeze<.05f&&
      std::abs(l.x)<.10f&&std::abs(l.y)<.10f&&!l.primary&&!l.secondary&&!l.stickClick&&!r.secondary&&!r.stickClick&&
      !gestureLocked_&&!chordBlocked_&&!weaponBusy_&&(held_.none()||(radialAction_!=Count&&held_[radialAction_]));
    auto menu=radial_.update(r.x,r.y,r.primary,clean,dt);
    if(!clean||!radial_.active()){radialAction_=Count;radialPulse_=0;}
    constexpr Action choices[]={SelectPrimaryWeapon,EnergyLink,ProfileAnalyser,SuitTool,SelectSecondaryWeapon,Holster,Flashlight,ToolMode};
    if(menu.confirmed>=0){
     radialAction_=choices[menu.confirmed];radialPulse_=.08f;
    }
    if(radialPulse_>0&&clean&&radialAction_!=Count&&r.primary&&menu.sector>=0&&choices[menu.sector]==radialAction_){o.held[radialAction_]=true;radialPulse_-=dt;}
    else {radialAction_=Count;radialPulse_=0;}
    o.radialVisible=menu.visible;o.radialIndex=menu.sector;held_=o.held;return o;
   }
   // Explicit support grip first, then dominant grip inside a virtual equipment zone.
   // No actions from other layers while this gesture is active.
   if(cfg_.equipmentGestures&&(gestureLocked_||r.squeeze>=.70f)){
    gestureLocked_=true;Output o;o.mode=ControlMode::Tools;
    const bool clean=!gestureInhibit_&&s.headTracked&&r.gripTracked&&n==0&&l.trigger<.05f&&r.trigger<.05f&&
      std::abs(l.x)<.10f&&std::abs(l.y)<.10f&&std::abs(r.x)<.10f&&std::abs(r.y)<.10f&&
      !chordBlocked_&&!weaponBusy_&&(held_.none()||(gestureAction_!=Count&&held_[gestureAction_]));
    const auto zone=clean?equipmentZone({float(r.gripPosition.x),float(r.gripPosition.y),float(r.gripPosition.z)},s.head,cfg_.leftHanded,cfg_.equipmentScalePercent/100.f):EquipmentZone::None;
    const auto selectedZone=equipment_.update(r.squeeze,zone,clean,dt);
    if(!clean||r.squeeze<.40f||(gestureZone_!=EquipmentZone::None&&zone!=gestureZone_)){gestureInhibit_=true;gestureAction_=Count;gesturePulse_=0;}
    if(selectedZone!=EquipmentZone::None){
     switch(selectedZone){
      case EquipmentZone::Hip:gestureAction_=SelectSecondaryWeapon;break;
      case EquipmentZone::Shoulder:gestureAction_=SelectPrimaryWeapon;break;
      case EquipmentZone::Energy:gestureAction_=EnergyLink;break;
      case EquipmentZone::Profile:gestureAction_=ProfileAnalyser;break;
      case EquipmentZone::Suit:gestureAction_=SuitTool;break;
      case EquipmentZone::Reload:gestureAction_=Reload;break;
      default:gestureAction_=Count;break;
     }
     gestureZone_=selectedZone;gesturePulse_=.08f;
    }
    if(gesturePulse_>0&&gestureAction_!=Count&&clean&&r.squeeze>=.40f&&!gestureInhibit_&&zone==gestureZone_){o.held[gestureAction_]=true;gesturePulse_-=dt;}
    if(r.squeeze<.05f&&n==0&&l.trigger<.05f&&r.trigger<.05f&&std::abs(l.x)<.10f&&std::abs(l.y)<.10f&&std::abs(r.x)<.10f&&std::abs(r.y)<.10f){
     equipment_.reset();gestureLocked_=gestureInhibit_=false;gestureZone_=EquipmentZone::None;gestureAction_=Count;gesturePulse_=0;
    }
    held_=o.held;return o;
   }
   // Opt-in equipment selection: support grip + dominant stick up/down.
   // One short key press per deliberate deflection; never fire or move in this layer.
   if(cfg_.weaponSelect && (weaponBusy_ || std::abs(r.y)>=.20f || std::abs(r.x)>=.20f)){
    weaponBusy_=true;Output o;o.mode=ControlMode::Tools;
    const bool conflict=n!=0||l.trigger>=.05f||r.trigger>=.05f||r.squeeze>=.05f||
      std::abs(l.x)>=.10f||std::abs(l.y)>=.10f||std::abs(r.x)>=.20f||chordBlocked_||
      (held_.any()&&!held_[SelectPrimaryWeapon]&&!held_[SelectSecondaryWeapon]);
    if(conflict)weaponConsumed_=true;
    if(!weaponConsumed_ && std::abs(r.y)>=.75f){
     weaponConsumed_=true;
     o.held[r.y>0?SelectPrimaryWeapon:SelectSecondaryWeapon]=true;
    }
    if(n==0&&l.trigger<.05f&&r.trigger<.05f&&r.squeeze<.05f&&
       std::abs(l.x)<.10f&&std::abs(l.y)<.10f&&std::abs(r.x)<.20f&&std::abs(r.y)<.20f){
     weaponBusy_=weaponConsumed_=chordBlocked_=false;
    }
    held_=o.held;return o;
   }
   // Optional strict selection: any conflicting physical input requires a clean release.
   const bool conflict=cfg_.strictTools && (std::abs(l.x)>=.10f||std::abs(l.y)>=.10f||std::abs(r.x)>=.10f||std::abs(r.y)>=.10f||r.squeeze>=.05f);
   if(conflict)chordBlocked_=true;
   if(n>1||l.trigger>=.05f||r.trigger>=.05f||(n==1&&held_.any()&&!held_[actions[selected]]))chordBlocked_=true;
   Output o;o.mode=ControlMode::Tools;
   if(n==0&&!conflict&&l.trigger<.05f&&r.trigger<.05f)chordBlocked_=false;
   if(n==1&&!chordBlocked_)o.held[actions[selected]]=true;
   held_=o.held;return o;
  }
  Output out;out.mode=cfg_.snapMode?ControlMode::Snap:ControlMode::Normal; auto set=[&](Action a,float v,float on,float off){out.held[a]=hysteresis(a,v,on,off);};
  set(Forward,l.y,cfg_.moveOn,cfg_.moveOff);set(Back,-l.y,cfg_.moveOn,cfg_.moveOff);
  set(Right,l.x,cfg_.moveOn,cfg_.moveOff);set(Left,-l.x,cfg_.moveOn,cfg_.moveOff);
  out.held[Jump]=r.secondary;out.held[Interact]=r.primary;out.held[Crouch]=l.primary;
  out.held[Sprint]=l.stickClick;out.held[CycleTool]=l.secondary;out.held[Reload]=r.stickClick;
  set(Primary,r.trigger,cfg_.triggerOn,cfg_.triggerOff);set(Secondary,l.trigger,cfg_.triggerOn,cfg_.triggerOff);
  const bool nextClutch=cfg_.motionAim&&r.squeeze>=(clutch_?.40f:.70f);
  if(nextClutch!=clutch_){clutch_=nextClutch;motion_.reset();remX_=remY_=0;snapReady_=false;}
  if(clutch_){
   auto aim=motion_.update(true,r.aim,cfg_.aimPixelsPerDegree,cfg_.invertY,dt);
   out.dx=aim.dx;out.dy=aim.dy;
   if(!aim.usable)return reset();
   out.pointing=true;out.mode=ControlMode::Motion;held_=out.held;return out;
  }
  // InputCore already applies its stick deadzone. Fractional pixels preserve slow motion.
  const float dead=cfg_.extraLookDeadzonePercent/100.f;
  auto axis=[dead](float v){return std::copysign((std::max)(0.f,(std::abs(v)-dead)/(1.f-dead)),v);};
  const float x=axis(r.x),y=axis(r.y);
  if(cfg_.snapMode){
   // One calibrated relative mouse pulse per neutral-to-edge gesture.
   // Never queue turns or repeat while held. Actual game angle depends on sensitivity.
   remX_=0;
   if(std::abs(r.x)<=.20f)snapReady_=true;
   if(std::abs(r.x)>=.75f && snapReady_){
    snapReady_=false;if(dt>0)out.dx=(r.x>0?1:-1)*int(15*cfg_.snapMode*cfg_.snapPixelsPerDegree);
   }
  }else remX_+=x*cfg_.lookPixelsPerSecond*dt;
  remY_+=y*cfg_.lookPixelsPerSecond*dt*(cfg_.invertY?1.f:-1.f);
  if(!cfg_.snapMode)out.dx=static_cast<int>(remX_);out.dy=static_cast<int>(remY_);if(!cfg_.snapMode)remX_-=out.dx;remY_-=out.dy;
  if(x==0)remX_=0;if(y==0)remY_=0;
  held_=out.held;return out;
 }
};
// Only transitions are emitted. A held action is not a stream of repeated key presses.
template<class Sink> bool dispatch(Keys& sent,const Keys& desired,Sink&& sink){
 // No OS transitions are necessary when all held states are unchanged.
 if(sent==desired)return true;
 bool ok=true;
 // Release first; never press if any release failed.
 for(unsigned i=0;i<Count;++i)if(sent[i]&&!desired[i]){if(sink(static_cast<Action>(i),false))sent.reset(i);else ok=false;}
 if(!ok)return false;
 for(unsigned i=0;i<Count;++i)if(!sent[i]&&desired[i]){if(sink(static_cast<Action>(i),true))sent.set(i);else return false;}
 return true;
}
}
