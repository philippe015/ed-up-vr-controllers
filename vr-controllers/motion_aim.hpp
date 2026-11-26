#pragma once
#include <algorithm>
#include <cmath>
namespace edup::vrinput::foot {
struct Orientation {float x=0,y=0,z=0,w=1; bool tracked=false;};
struct AimDelta {int dx=0,dy=0;bool usable=false;};
// A physical clutch: relative angular motion -> mouse pixels. No target tracking.
class MotionAim {
 static constexpr double pi=3.14159265358979323846;
 bool have_=false,blocked_=false;double yaw_=0,pitch_=0,rx_=0,ry_=0;
 static bool angles(const Orientation& q,double& yaw,double& pitch){
  if(!q.tracked)return false;
  double x=q.x,y=q.y,z=q.z,w=q.w;
  const double norm=x*x+y*y+z*z+w*w;
  if(!std::isfinite(norm)||norm<.81||norm>1.21)return false;
  const double n=std::sqrt(norm);x/=n;y/=n;z/=n;w/=n;
  // Rotate OpenXR's forward (-Z); yaw positive right, pitch positive up.
  const double fx=-2*(x*z+w*y),fy=2*(w*x-y*z),fz=-(1-2*(x*x+y*y));
  const double horizontal=std::hypot(fx,fz);
  if(horizontal<.17)return false; // near vertical yaw is ill-conditioned
  yaw=std::atan2(fx,-fz);pitch=std::atan2(fy,horizontal);return true;
 }
public:
 void reset(){have_=blocked_=false;rx_=ry_=0;}
 AimDelta update(bool clutch,const Orientation& q,float pixelsPerDegree,bool invertY,float dt){
  if(!clutch){reset();return {};}
  if(blocked_)return {};
  double y,p;
  if(!std::isfinite(dt)||dt<0||dt>.1f||!std::isfinite(pixelsPerDegree)||!angles(q,y,p)){blocked_=true;have_=false;rx_=ry_=0;return {};}
  if(!have_){yaw_=y;pitch_=p;have_=true;return {0,0,true};}
  const double dyaw=std::remainder(y-yaw_,2*pi)*180/pi,dpitch=(p-pitch_)*180/pi;
  yaw_=y;pitch_=p;
  // Large discontinuities (recenter, tracking jumps) require grip release.
  if(std::hypot(dyaw,dpitch)>25){blocked_=true;rx_=ry_=0;return {};}
  const double scale=std::clamp(double(pixelsPerDegree),1.,30.);
  rx_+=dyaw*scale;ry_+=dpitch*scale*(invertY?1:-1);
  const int dx=int(std::clamp(rx_,-180.,180.)),dy=int(std::clamp(ry_,-180.,180.));
  // Discard saturation excess; never queue a delayed camera movement.
  rx_=std::abs(rx_)>180?0:rx_-dx;ry_=std::abs(ry_)>180?0:ry_-dy;
  return {dx,dy,true};
 }
};
}
