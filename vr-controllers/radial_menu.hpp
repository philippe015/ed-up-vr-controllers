#pragma once
#include <cmath>
namespace edup::vrinput::foot {
// Clockwise from top. Leave a gap at sector boundaries to avoid ambiguous choices.
inline int radialSector(float x,float y){
 if(!std::isfinite(x)||!std::isfinite(y)||std::hypot(x,y)<.55f)return -1;
 constexpr float step=.7853981634f;
 const float a=std::atan2(x,y)/step;
 const int n=int(std::round(a));
 if(std::abs(a-n)>.40f)return -1;
 return (n+8)%8;
}
struct RadialResult{bool visible=false;int sector=-1,confirmed=-1;};
class RadialMenu{
 bool active_=false,blocked_=false,used_=false,wasConfirm_=false;
 int sector_=-1;float dwell_=0;
public:
 bool active()const{return active_;}
 void reset(){*this=RadialMenu{};}
 RadialResult update(float x,float y,bool confirm,bool clean,float dt){
  if(!std::isfinite(dt)||dt<0||dt>=.1f){reset();return {};}
  bool centered=std::abs(x)<.10f&&std::abs(y)<.10f;
  if(!active_){
   if(centered)return {};
   active_=true;blocked_=!clean||confirm;wasConfirm_=confirm;
  }
  if(!clean)blocked_=true;
  if(centered&&!confirm&&clean){reset();return {};}
  int next=radialSector(x,y);
  if(next!=sector_){sector_=next;dwell_=0;}
  else if(next>=0)dwell_+=dt;
  int selected=-1;
  if(confirm&&!wasConfirm_&&!used_&&!blocked_){
   if(sector_>=0&&dwell_>=.12f){selected=sector_;used_=true;}
   else blocked_=true;
  }
  wasConfirm_=confirm;
  return {!blocked_&&!used_,blocked_?-1:sector_,selected};
 }
};
}
