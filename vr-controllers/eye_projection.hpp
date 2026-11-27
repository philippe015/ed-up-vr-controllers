#pragma once
#include "virtual_hands.hpp"
#include <algorithm>
namespace edup::vrinput::hands {
struct Fov {float left,right,down,up;}; // OpenXR radians
struct Line2 {Screen a,b;}; // normalized [0,1], Y down
inline bool clipTest(float p,float q,float& lo,float& hi){
 if(p==0)return q>=0;float r=q/p;if(p<0)lo=(std::max)(lo,r);else hi=(std::min)(hi,r);return lo<=hi;
}
inline bool clipUnit(Line2& s){
 if(!std::isfinite(s.a.x)||!std::isfinite(s.a.y)||!std::isfinite(s.b.x)||!std::isfinite(s.b.y))return false;
 float dx=s.b.x-s.a.x,dy=s.b.y-s.a.y,lo=0,hi=1;
 if(!std::isfinite(dx)||!std::isfinite(dy)||!clipTest(-dx,s.a.x,lo,hi)||!clipTest(dx,1-s.a.x,lo,hi)||!clipTest(-dy,s.a.y,lo,hi)||!clipTest(dy,1-s.a.y,lo,hi))return false;
 auto a=s.a;s.a={std::clamp(a.x+lo*dx,0.f,1.f),std::clamp(a.y+lo*dy,0.f,1.f)};s.b={std::clamp(a.x+hi*dx,0.f,1.f),std::clamp(a.y+hi*dy,0.f,1.f)};return true;
}
// Prepare invariant eye/head transforms and FOV once per eye, not per segment.
class EyeProjector {
 Pose head_{},eye_{};Quat eyeInv_{};float l_=0,r_=0,d_=0,u_=0;bool valid_=false;
public:
 EyeProjector(Pose head,Pose eye,Fov f):head_(head),eye_(eye){
  if(!finite(head_.p)||!finite(eye_.p)||!normalize(head_.q)||!normalize(eye_.q))return;
  for(float x:{f.left,f.right,f.down,f.up})if(!std::isfinite(x)||std::abs(x)>=1.56f)return;
  l_=std::tan(f.left);r_=std::tan(f.right);d_=std::tan(f.down);u_=std::tan(f.up);
  if(r_-l_<.001f||u_-d_<.001f)return;
  eyeInv_=inverse(eye_.q);valid_=true;
 }
 bool valid() const{return valid_;}
 bool line(Segment line,Line2& out,bool* whole=nullptr)const{
  if(whole)*whole=false;
  if(!valid_||!finite(line.a)||!finite(line.b))return false;
  auto toEye=[&](Vec p){return rotate(eyeInv_,head_.p+rotate(head_.q,p)-eye_.p);};
  Vec a=toEye(line.a),b=toEye(line.b);if(!finite(a)||!finite(b))return false;
  constexpr float nearZ=-.04f;
  if(a.z>nearZ&&b.z>nearZ)return false;
  if(a.z>nearZ){float t=(nearZ-a.z)/(b.z-a.z);a=a+(b-a)*t;a.z=nearZ;}
  if(b.z>nearZ){float t=(nearZ-b.z)/(a.z-b.z);b=b+(a-b)*t;b.z=nearZ;}
  auto project=[&](Vec p){return Screen{(p.x/(-p.z)-l_)/(r_-l_),(u_-p.y/(-p.z))/(u_-d_)};};
  out={project(a),project(b)};
  if(whole){auto inside=[](Screen p){return std::isfinite(p.x)&&std::isfinite(p.y)&&p.x>=.01f&&p.x<=.99f&&p.y>=.01f&&p.y<=.99f;};*whole=a.z<nearZ&&b.z<nearZ&&inside(out.a)&&inside(out.b);}
  return clipUnit(out);
 }
};
inline bool eyeLine(Segment line,Pose head,Pose eye,Fov f,Line2& out){
 return EyeProjector(head,eye,f).line(line,out);
}
inline Segment segment(const Snapshot& s,unsigned hand,unsigned i){const auto& p=s.points[hand][i];return {{p[0],p[1],p[2]},{p[3],p[4],p[5]}};}
inline bool fresh(const Snapshot& s,uint64_t now,uint64_t maxAge=100){return s.version==6 && s.controlMode<=5 && s.wristVisible<=1 && (!s.wristVisible||(s.radialVisible&&finite(s.wristCenter))) && s.radialVisible<=1 && s.radialIndex>=-1 && s.radialIndex<=7 && s.objectVisible<=1 && s.objectOwner>=-1 && s.objectOwner<=1 && s.leftHanded<=1&&s.tick&&now>=s.tick&&now-s.tick<=maxAge;}
}
