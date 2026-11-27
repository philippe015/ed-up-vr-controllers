#include "onfoot.hpp"
#include "stability.hpp"
#include <cassert>
#include <limits>
#include <random>
using namespace edup::vrinput;
using namespace edup::vrinput::foot;
int main(){
 const float nan=std::numeric_limits<float>::quiet_NaN(), inf=std::numeric_limits<float>::infinity();
 assert(!analogValid(nan,0,1)&&!analogValid(inf,0,1)&&!analogValid(-.1f,0,1)&&!analogValid(1.1f,0,1));
 assert(poseComponentsValid(0,0,0,0,0,0,1));
 assert(!poseComponentsValid(nan,0,0,0,0,0,1)&&!poseComponentsValid(0,0,0,0,0,0,0)&&!poseComponentsValid(0,0,0,0,0,0,2));
 assert(!sampleExpired(199,100)&&sampleExpired(200,100)&&sampleExpired(99,100));
 Mapper m; Sample s; s.enabled=s.focused=true;s.hand[0].ready=s.hand[1].ready=true;
 m.update(s,.01f);s.hand[0].y=1;assert(m.update(s,.01f).held[Forward]);
 s.hand[1].trigger=-1;assert(m.update(s,.01f).held.none());s.hand[1].trigger=0;assert(m.update(s,.01f).held.none());
 s.hand[0].y=0;m.update(s,.01f);s.hand[0].y=1;assert(m.update(s,.01f).held[Forward]);
 std::mt19937 rng(42);
 for(int i=0;i<20000;++i){s.hand[0].x=float(int(rng()%201)-100)/100;s.hand[0].y=float(int(rng()%201)-100)/100;s.hand[1].trigger=float(rng()%101)/100;s.focused=(rng()%5)!=0;s.hand[1].ready=(rng()%7)!=0;auto o=m.update(s,.01f);if(!s.focused||!s.hand[1].ready)assert(o.held.none()&&o.dx==0&&o.dy==0);assert(!(o.held[Left]&&o.held[Right]));assert(!(o.held[Forward]&&o.held[Back]));}
 // Exercise every new mode with deterministic mixed inputs and injected faults.
 for(unsigned profile=0;profile<2;++profile)for(unsigned snap=0;snap<4;++snap){
  Settings cfg;cfg.leftHanded=profile!=0;cfg.snapMode=snap;cfg.twoHandAim=true;cfg.strictTools=true;cfg.weaponSelect=true;cfg.equipmentGestures=true;
  Mapper mixed(cfg);Sample v;
  for(unsigned i=0;i<10000;++i){
   v={};v.enabled=v.focused=v.headTracked=true;
   for(auto& h:v.hand){h.ready=h.gripTracked=true;h.aim.tracked=true;}
   auto& support=v.hand[profile?1:0];auto& dominant=v.hand[profile?0:1];
   support.gripPosition={0,0,-.4};
   if(i%10!=0){
    for(auto& h:v.hand){h.x=float(int(rng()%201)-100)/100;h.y=float(int(rng()%201)-100)/100;h.squeeze=float(rng()%101)/100;h.trigger=float(rng()%101)/100;h.primary=rng()%9==0;h.secondary=rng()%9==0;h.stickClick=rng()%9==0;}
    if(i%10==1){dominant.squeeze=.8f;support.squeeze=0;dominant.trigger=support.trigger=0;}
   }
   float dt=.01f;
   switch(i%17){case 1:v.focused=false;break;case 2:dominant.ready=false;break;case 3:dt=.1f;break;case 4:dt=nan;break;case 5:support.x=inf;break;default:break;}
   const auto o=mixed.update(v,dt);
   if(i%17>=1&&i%17<=5)assert(o.held.none()&&o.dx==0&&o.dy==0&&o.mode==ControlMode::Neutral);
   assert(!(o.held[Forward]&&o.held[Back])&&!(o.held[Left]&&o.held[Right]));
   assert(std::abs(o.dx)<=1350&&std::abs(o.dy)<=300);
   if(o.mode==ControlMode::TwoHand)assert(o.held.none()&&!o.pointing);
   if(o.mode==ControlMode::Tools){assert(o.dx==0&&o.dy==0&&o.held.count()<=1);for(unsigned a=0;a<EnergyLink;++a)if(a!=Reload)assert(!o.held[a]);}
  }
 }
 // No delayed snap from a zero-duration sample.
 Settings cfg;cfg.snapMode=1;Mapper snap(cfg);Sample t;t.enabled=t.focused=true;t.hand[0].ready=t.hand[1].ready=true;
 snap.update(t,.01f);snap.update(t,.01f);t.hand[1].x=.9f;
 assert(snap.update(t,0).dx==0);assert(snap.update(t,.01f).dx==0);
 t.hand[1].x=0;snap.update(t,.01f);t.hand[1].x=.9f;assert(snap.update(t,.01f).dx==150);
}
