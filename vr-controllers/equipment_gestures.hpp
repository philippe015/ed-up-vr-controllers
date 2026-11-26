#pragma once
#include "virtual_hands.hpp"
namespace edup::vrinput::foot {
// Head-relative *virtual* zones, not body tracking. Y stays aligned to gravity.
enum class EquipmentZone {None,Hip,Shoulder,Energy,Profile,Suit,Reload};
inline EquipmentZone equipmentZone(hands::Vec hand,hands::Pose head,bool leftHanded,float scale){
 using namespace hands;
 if(!finite(hand)||!finite(head.p)||!normalize(head.q)||!std::isfinite(scale)||scale<.7f||scale>1.3f)return EquipmentZone::None;
 Vec d=hand-head.p,forward=rotate(head.q,{0,0,-1});
 float horizontal=std::hypot(forward.x,forward.z);
 if(!finite(d)||horizontal<.35f||d.x*d.x+d.y*d.y+d.z*d.z>2.25f)return EquipmentZone::None;
 forward=forward*(1.f/horizontal);forward.y=0;
 Vec right{-forward.z,0,forward.x};
 float x=(d.x*right.x+d.z*right.z)*(leftHanded?-1.f:1.f)/scale;
 float y=d.y/scale,z=-(d.x*forward.x+d.z*forward.z)/scale;
 // Nonoverlapping regions with intentional gaps. Positive z is behind the head.
 if(x>.18f&&x<.55f&&y>-.95f&&y<-.60f&&z>-.25f&&z<.18f)return EquipmentZone::Hip;
 if(x>.16f&&x<.55f&&y>-.35f&&y<.02f&&z>.10f&&z<.38f)return EquipmentZone::Shoulder;
 if(x<-.12f&&x>-.45f&&z<-.20f&&z>-.55f){
  if(y>-.28f&&y<-.10f)return EquipmentZone::Energy;
  if(y>-.49f&&y<-.33f)return EquipmentZone::Profile;
  if(y>-.72f&&y<-.54f)return EquipmentZone::Suit;
 }
 if(x>.08f&&x<.42f&&y>-.48f&&y<-.26f&&z<-.30f&&z>-.65f)return EquipmentZone::Reload;
 return EquipmentZone::None;
}
// One deliberate grip gesture. Stable zone dwell, then release before any repeat.
// Conflicts or tracking loss cancel until a clean release.
class EquipmentGesture {
 bool active_=false,blocked_=false,consumed_=false;
 EquipmentZone zone_=EquipmentZone::None;
 float elapsed_=0;
public:
 void reset(){*this=EquipmentGesture{};}
 bool active()const{return active_;}
 EquipmentZone update(float grip,EquipmentZone zone,bool clean,float dt){
  if(!std::isfinite(grip)||!std::isfinite(dt)||dt<0||dt>=.1f){reset();return EquipmentZone::None;}
  if(!active_&&grip<.70f)return EquipmentZone::None;
  if(!active_){active_=true;zone_=zone;elapsed_=0;blocked_=!clean||zone==EquipmentZone::None;}
  if(!clean||zone!=zone_)blocked_=true;
  if(grip<.40f){reset();return EquipmentZone::None;}
  if(blocked_||consumed_)return EquipmentZone::None;
  elapsed_+=dt;
  if(elapsed_>=.20f){consumed_=true;return zone_;}
  return EquipmentZone::None;
 }
};
}
