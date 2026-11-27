#include "onfoot.hpp"
#include "radial_wire.hpp"
#include <cassert>
#include <limits>
using namespace edup::vrinput::foot;
int main(){
 for(unsigned i=0;i<8;++i){float a=i*.7853981634f;assert(radialSector(.9f*std::sin(a),.9f*std::cos(a))==int(i));auto wire=edup::vrinput::hands::radialWire(i);assert(wire.size>=6&&wire.size<=11);}
 assert(radialSector(0,0)==-1);assert(radialSector(std::numeric_limits<float>::quiet_NaN(),1)==-1);
 for(bool left:{false,true}){
  Settings cfg;cfg.radialMenu=true;cfg.weaponSelect=true;cfg.leftHanded=left;Mapper m(cfg);
  Sample s;s.enabled=s.focused=s.headTracked=true;
  auto& l=s.hand[left?1:0];auto& r=s.hand[left?0:1];l.ready=r.ready=true;
  auto tick=[&]{return m.update(s,.01f);};
  tick();l.squeeze=.8f;tick();tick();
  constexpr Action actions[]={SelectPrimaryWeapon,EnergyLink,ProfileAnalyser,SuitTool,SelectSecondaryWeapon,Holster,Flashlight,ToolMode};
  for(unsigned i=0;i<8;++i){
   r.x=.9f*std::sin(i*.7853981634f);r.y=.9f*std::cos(i*.7853981634f);
   for(int j=0;j<20;++j){auto o=tick();assert(o.radialVisible&&o.radialIndex==int(i)&&o.held.none()&&!o.dx&&!o.dy);}
   s.renderedRadialSector=int(i);r.primary=true;assert(tick().held[actions[i]]);s.renderedRadialSector=-1;
   for(int j=0;j<20;++j){auto o=tick();assert(o.held.none()||(o.held.count()==1&&o.held[actions[i]]));}
   assert(tick().held.none());r.primary=false;r.x=r.y=0;assert(tick().held.none());
  }
  // No render acknowledgement or wrong sector must block even after dwell.
  for(int receipt:{-1,1}){
   r.y=.9f;for(int i=0;i<20;++i)tick();s.renderedRadialSector=receipt;
   r.primary=true;assert(tick().held.none());s.renderedRadialSector=0;
   for(int i=0;i<20;++i)assert(tick().held.none());
   r.primary=false;r.x=r.y=0;s.renderedRadialSector=-1;tick();
  }
  // Conflicting grip and stick cannot fall back to legacy selection or buttons.
  r.y=.9f;r.squeeze=.8f;for(int i=0;i<20;++i)assert(tick().held.none());
  r.primary=true;assert(tick().held.none());r.primary=false;r.squeeze=0;r.y=0;tick();
  r.y=.9f;tick();r.primary=true;assert(tick().held.none());for(int i=0;i<20;++i)assert(tick().held.none());r.primary=false;r.y=0;tick();
  r.y=.9f;for(int i=0;i<20;++i)tick();s.headTracked=false;r.primary=true;assert(tick().held.none());s.headTracked=true;assert(tick().held.none());
  r.primary=false;r.y=0;tick();r.y=.9f;for(int i=0;i<20;++i)tick();s.focused=false;r.primary=true;assert(tick().held.none());s.focused=true;assert(tick().held.none());
 }
}
