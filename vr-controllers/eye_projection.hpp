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
inline bool eyeLine(Segment line,Pose head,Pose eye,Fov f,Line2& out){
 if(!finite(head.p)||!finite(eye.p)||!normalize(head.q)||!normalize(eye.q)||!finite(line.a)||!finite(line.b))return false;
 // Input segments are in head-local coordinates; eye and head poses share the seated space.
 auto toEye=[&](Vec p){return rotate(inverse(eye.q),head.p+rotate(head.q,p)-eye.p);};
 Vec a=toEye(line.a),b=toEye(line.b);if(!finite(a)||!finite(b))return false;
 constexpr float nearZ=-.04f;
 if(a.z>nearZ&&b.z>nearZ)return false;
 if(a.z>nearZ){float t=(nearZ-a.z)/(b.z-a.z);a=a+(b-a)*t;a.z=nearZ;}
 if(b.z>nearZ){float t=(nearZ-b.z)/(a.z-b.z);b=b+(a-b)*t;b.z=nearZ;}
 for(float x:{f.left,f.right,f.down,f.up})if(!std::isfinite(x)||std::abs(x)>=1.56f)return false;
 const float l=std::tan(f.left),r=std::tan(f.right),d=std::tan(f.down),u=std::tan(f.up);
 if(r-l<.001f||u-d<.001f)return false;
 auto project=[&](Vec p){return Screen{(p.x/(-p.z)-l)/(r-l),(u-p.y/(-p.z))/(u-d)};};
 out={project(a),project(b)};return clipUnit(out);
}
inline Segment segment(const Snapshot& s,unsigned hand,unsigned i){const auto& p=s.points[hand][i];return {{p[0],p[1],p[2]},{p[3],p[4],p[5]}};}
inline bool fresh(const Snapshot& s,uint64_t now,uint64_t maxAge=100){return s.version==4 && s.controlMode<=5 && s.objectVisible<=1 && s.objectOwner>=-1 && s.objectOwner<=1 && s.leftHanded<=1&&s.tick&&now>=s.tick&&now-s.tick<=maxAge;}
}
