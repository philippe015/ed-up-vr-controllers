#include "onfoot.hpp"
#include "radial_wire.hpp"
#include <cassert>
#include <limits>
using namespace edup::vrinput::foot;
using namespace edup::vrinput::hands;
int main(){
 Vec a{};Pose h{};
 assert(wristAnchor({-.15f,-.15f,-.5f},h,a));assert(std::abs(a.y+.075f)<.0001f);
 assert(!wristAnchor({0,0,.5f},h,a));
 assert(!wristAnchor({1,0,-.5f},h,a));
 assert(!wristAnchor({0,0,std::numeric_limits<float>::quiet_NaN()},h,a));
 assert(wristAnchor({.15f,-.15f,-.5f},h,a));
 auto seg=wristSegment({{0,0,-.65f},{.1f,0,-.65f}},a);assert(std::abs(seg.b.x-seg.a.x-.048f)<.0001f);assert(seg.a.z==a.z);
 for(bool left:{false,true}){
  Settings cfg;cfg.wristMenu=true;cfg.leftHanded=left;Mapper m(cfg);Sample s;s.enabled=s.focused=s.headTracked=true;
  auto& l=s.hand[left?1:0];auto& r=s.hand[left?0:1];l.ready=r.ready=l.gripTracked=r.gripTracked=true;
  l.gripPosition={left?.15:-.15,-.15,-.5};auto tick=[&]{return m.update(s,.01f);};
  tick();l.squeeze=.8f;tick();tick();r.y=.9f;
  for(int i=0;i<20;++i){auto o=tick();assert(o.radialVisible&&o.held.none());}
  r.primary=true;assert(tick().held[SelectPrimaryWeapon]);r.primary=false;r.y=0;tick();
  r.y=.9f;for(int i=0;i<20;++i)tick();l.gripTracked=false;r.primary=true;assert(tick().held.none());l.gripTracked=true;assert(tick().held.none());
  r.primary=false;r.y=0;tick();l.gripPosition={0,0,.5};r.y=.9f;for(int i=0;i<20;++i){auto o=tick();assert(!o.radialVisible&&o.held.none());}
  r.primary=true;assert(tick().held.none());
 }
}
