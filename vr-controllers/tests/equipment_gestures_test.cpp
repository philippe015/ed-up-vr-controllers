#include "onfoot.hpp"
#include <cassert>
#include <limits>
using namespace edup::vrinput::foot;
using namespace edup::vrinput::hands;
int main(){
 Pose h{};
 assert(equipmentZone({.30f,-.75f,0},h,false,1)==EquipmentZone::Hip);
 assert(equipmentZone({-.30f,-.75f,0},h,true,1)==EquipmentZone::Hip);
 assert(equipmentZone({.30f,-.20f,.20f},h,false,1)==EquipmentZone::Shoulder);
 assert(equipmentZone({-.25f,-.20f,-.35f},h,false,1)==EquipmentZone::Energy);
 assert(equipmentZone({-.25f,-.40f,-.35f},h,false,1)==EquipmentZone::Profile);
 assert(equipmentZone({-.25f,-.62f,-.35f},h,false,1)==EquipmentZone::Suit);
 assert(equipmentZone({.25f,-.40f,-.45f},h,false,1)==EquipmentZone::Reload);
 assert(equipmentZone({0,0,0},h,false,1)==EquipmentZone::None);
 assert(equipmentZone({.3f,-.75f,0},h,false,0)==EquipmentZone::None);
 assert(equipmentZone({std::numeric_limits<float>::quiet_NaN(),0,0},h,false,1)==EquipmentZone::None);
 h.q={0,.70710678f,0,.70710678f};
 assert(equipmentZone({0,-.75f,-.30f},h,false,1)==EquipmentZone::Hip);
 for(bool left:{false,true}){
  Settings cfg;cfg.equipmentGestures=true;cfg.leftHanded=left;cfg.twoHandAim=true;cfg.strictTools=true;
  Mapper m(cfg);Sample s;s.enabled=s.focused=s.headTracked=true;
  auto& a=s.hand[left?1:0];auto& b=s.hand[left?0:1];a.ready=b.ready=b.gripTracked=true;
  auto tick=[&]{return m.update(s,.01f);};
  tick();a.squeeze=.8f;tick();tick();
  auto attempt=[&](double x,double y,double z,Action expected){
   b.gripPosition={left?-x:x,y,z};b.squeeze=.8f;
   unsigned presses=0;bool prev=false;
   for(int i=0;i<70;++i){auto o=tick();assert(o.dx==0&&o.dy==0);assert(o.held.count()<=1);assert(!o.held[Primary]&&!o.held[Secondary]);bool down=o.held[expected];assert(o.held.none()||down);if(down&&!prev)++presses;prev=down;}
   assert(presses==1);b.squeeze=0;assert(tick().held.none());
  };
  attempt(.30,-.75,0,SelectSecondaryWeapon);
  attempt(.30,-.20,.20,SelectPrimaryWeapon);
  attempt(-.25,-.20,-.35,EnergyLink);
  attempt(-.25,-.40,-.35,ProfileAnalyser);
  attempt(-.25,-.62,-.35,SuitTool);
  attempt(.25,-.40,-.45,Reload);
  b.gripPosition={left?-.30:.30,-.75,0};b.squeeze=.8f;b.trigger=.8f;
  assert(tick().held.none());b.trigger=0;for(int i=0;i<50;++i)assert(tick().held.none());
  b.squeeze=0;tick();b.squeeze=.8f;s.headTracked=false;tick();s.headTracked=true;
  for(int i=0;i<50;++i)assert(tick().held.none());
  b.squeeze=0;tick();b.squeeze=.8f;
  for(int i=0;i<10;++i)assert(tick().held.none());
  b.gripPosition={left?-.30:.30,-.20,.20};
  for(int i=0;i<50;++i)assert(tick().held.none());
  b.squeeze=0;tick();attempt(.30,-.75,0,SelectSecondaryWeapon);
  b.squeeze=.8f;tick();s.focused=false;assert(tick().held.none());s.focused=true;assert(tick().held.none());
 }
}
