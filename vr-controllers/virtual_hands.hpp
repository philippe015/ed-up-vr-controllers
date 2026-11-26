#pragma once
#include <array>
#include <cmath>
#include <cstdint>
namespace edup::vrinput::hands {
struct Vec {float x=0,y=0,z=0;};
struct Quat {float x=0,y=0,z=0,w=1;};
struct Pose {Vec p; Quat q;};
inline Vec operator+(Vec a,Vec b){return {a.x+b.x,a.y+b.y,a.z+b.z};}
inline Vec operator-(Vec a,Vec b){return {a.x-b.x,a.y-b.y,a.z-b.z};}
inline Vec operator*(Vec a,float s){return {a.x*s,a.y*s,a.z*s};}
inline Vec cross(Vec a,Vec b){return {a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x};}
inline bool finite(Vec p){return std::isfinite(p.x)&&std::isfinite(p.y)&&std::isfinite(p.z);}
inline bool normalize(Quat& q){float n=q.x*q.x+q.y*q.y+q.z*q.z+q.w*q.w;if(!std::isfinite(n)||n<.8f||n>1.2f)return false;float s=1/std::sqrt(n);q={q.x*s,q.y*s,q.z*s,q.w*s};return true;}
inline Quat inverse(Quat q){return {-q.x,-q.y,-q.z,q.w};}
inline Vec rotate(Quat q,Vec p){Vec u{q.x,q.y,q.z};Vec t=cross(u,p)*2.f;return p+t*q.w+cross(u,t);}
struct Segment{Vec a,b;};
constexpr unsigned count=19;
struct Wire {std::array<Segment,count> lines{};bool valid=false;};
// Stylised palm, fingers and thumb attached to controller grip. Not a measured skeleton.
inline Wire make(Pose grip,Pose head,bool left,float trigger,float squeeze){
 Wire w;if(!finite(grip.p)||!finite(head.p)||!normalize(grip.q)||!normalize(head.q)||!std::isfinite(trigger)||!std::isfinite(squeeze))return w;
 Vec d=grip.p-head.p;if(!finite(d)||d.x*d.x+d.y*d.y+d.z*d.z>9.f)return w;
 auto local=[&](Vec p){return rotate(inverse(head.q),grip.p+rotate(grip.q,p)-head.p);};
 unsigned n=0;auto add=[&](Vec a,Vec b){w.lines[n++]={local(a),local(b)};};
 Vec a{-.04f,0,.035f},b{.04f,0,.035f},c{.04f,0,-.035f},e{-.04f,0,-.035f};add(a,b);add(b,c);add(c,e);add(e,a);
 auto clamp=[](float v){return v<0?0.f:v>1?1.f:v;};
 for(unsigned i=0;i<4;++i){float x=-.03f+.02f*i;float curl=clamp(i==0?trigger:squeeze);Vec p{x,0,-.035f},q{x,-.022f*curl,-.065f},r{x,-.048f*curl,-.087f+.026f*curl};add(p,q);add(q,r);add(r,{x,-.055f*curl,-.101f+.045f*curl});}
 float sign=left?1.f:-1.f;Vec t{sign*.04f,0,.015f},u{sign*.063f,-.012f,-.005f},v{sign*.07f,-.025f,-.025f};add(t,u);add(u,v);add(v,{sign*.067f,-.035f,-.035f});w.valid=true;return w;
}
struct Screen{float x,y;};
// Diagnostic perspective with parallel eyes; the actual runtime eye poses/FOV must replace this in the headset renderer.
inline bool project(Vec p,float eyeX,float tanHalfFov,Screen& s){if(!finite(p)||!std::isfinite(eyeX)||!std::isfinite(tanHalfFov)||tanHalfFov<=0||p.z>=-.04f)return false;float z=-p.z;s={(p.x-eyeX)/(z*tanHalfFov),p.y/(z*tanHalfFov)};return std::isfinite(s.x)&&std::isfinite(s.y);}
struct Snapshot {uint32_t version=4;uint32_t valid[2]{};uint64_t tick=0;float points[2][count][6]{};uint32_t laserValid=0;Segment laser{};uint32_t controlMode=0, leftHanded=0;uint32_t objectVisible=0;int32_t objectOwner=-1;std::array<Segment,12> objectLines{};};
inline void set(Snapshot& s,unsigned h,const Wire& w){if(h>=2)return;s.valid[h]=w.valid?1u:0u;for(unsigned i=0;i<count;++i){auto a=w.lines[i].a,b=w.lines[i].b;auto& p=s.points[h][i];p[0]=a.x;p[1]=a.y;p[2]=a.z;p[3]=b.x;p[4]=b.y;p[5]=b.z;}}
}
