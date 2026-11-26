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
}
