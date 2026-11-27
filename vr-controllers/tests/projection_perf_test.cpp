#include "eye_projection.hpp"
#include "radial_wire.hpp"
#include <cassert>
#include <chrono>
#include <iostream>
#include <random>
#include <limits>
using namespace edup::vrinput::hands;
static bool referenceLine(Segment line,Pose head,Pose eye,Fov f,Line2& out){
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

struct Frame {Pose head,eye;Fov f;};
int main(){
 std::mt19937 rng(731);
 auto rnd=[&](){return float(int(rng()%2001)-1000)/1000.f;};
 std::array<Frame,64> frames{};
 std::array<Segment,128> lines{};
 for(auto& f:frames){
  float a=rnd()*.8f;Quat q{0,std::sin(a),0,std::cos(a)};
  f.head={{rnd()*2,rnd(),rnd()*2},q};f.eye={f.head.p+rotate(q,{.032f,0,0}),q};
  f.f={-.7f+rnd()*.1f,.8f+rnd()*.1f,-.7f,.8f};
 }
 for(auto& s:lines)s={{rnd(),rnd(),rnd()-.7f},{rnd(),rnd(),rnd()-.7f}};
 for(auto f:frames){EyeProjector p(f.head,f.eye,f.f);assert(p.valid());for(auto s:lines){Line2 a,b;bool x=referenceLine(s,f.head,f.eye,f.f,a),y=p.line(s,b);assert(x==y);if(x){assert(std::abs(a.a.x-b.a.x)<1e-5f&&std::abs(a.a.y-b.a.y)<1e-5f&&std::abs(a.b.x-b.b.x)<1e-5f&&std::abs(a.b.y-b.b.y)<1e-5f);}}}
 for(unsigned i=0;i<8;++i){const auto& a=radialWire(i);auto b=buildRadialWire(i);assert(&a==&radialWire(i)&&a.size==b.size);for(unsigned j=0;j<a.size;++j){assert(a.lines[j].a.x==b.lines[j].a.x&&a.lines[j].a.y==b.lines[j].a.y&&a.lines[j].a.z==b.lines[j].a.z&&a.lines[j].b.x==b.lines[j].b.x&&a.lines[j].b.y==b.lines[j].b.y&&a.lines[j].b.z==b.lines[j].b.z);}}
 assert(!EyeProjector({}, {}, {0,0,-.7f,.8f}).valid());assert(radialWire(8).size==0);
 // Informational CPU benchmark only, not a headset FPS measurement or timing gate.
 auto bench=[&](bool prepared){
  volatile double checksum=0;auto t=std::chrono::steady_clock::now();
  for(int run=0;run<120;++run)for(auto f:frames){EyeProjector p(f.head,f.eye,f.f);for(auto s:lines){Line2 o;bool ok=prepared?p.line(s,o):referenceLine(s,f.head,f.eye,f.f,o);if(ok)checksum+=o.a.x+o.b.y;}}
  double ms=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-t).count();
  std::cout<<(prepared?"prepared":"reference")<<" CPU ms="<<ms<<" checksum="<<checksum<<"\n";return double(checksum);
 };
 auto baseline=bench(false),optimized=bench(true);assert(std::abs(baseline-optimized)<1e-3);
}
