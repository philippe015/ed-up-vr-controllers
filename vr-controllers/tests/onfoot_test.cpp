#include "onfoot.hpp"
#include <cassert>
#include <vector>
#include <limits>
using namespace edup::vrinput::foot;
int main(){
 Mapper m;Sample s;s.enabled=s.focused=true;s.hand[0].ready=s.hand[1].ready=true;
 s.hand[0].y=1;assert(m.update(s,.01f).held.none());s.hand[0].y=0;assert(m.update(s,.01f).held.none());
 s.hand[0].y=.7f;s.hand[0].x=-.7f;s.hand[1].x=.5f;s.hand[1].trigger=.8f;s.hand[1].primary=true;
 auto o=m.update(s,.02f);assert(o.held[Forward]&&o.held[Left]&&o.held[Primary]&&o.held[Interact]&&o.dx==9);
 s.hand[0].y=.45f;assert(m.update(s,.01f).held[Forward]);s.hand[0].y=.2f;assert(!m.update(s,.01f).held[Forward]);
 s.hand[1].trigger=.5f;assert(m.update(s,.01f).held[Primary]);s.hand[1].trigger=.2f;assert(!m.update(s,.01f).held[Primary]);
 s.hand[1].ready=false;assert(m.update(s,.01f).held.none());s.hand[1].ready=true;assert(m.update(s,.01f).held.none());
 s.hand[0]=Hand{};s.hand[1]=Hand{};s.hand[0].ready=s.hand[1].ready=true;m.update(s,.01f);s.hand[0].stickClick=true;s.hand[1].stickClick=true;s.hand[1].secondary=true;
 o=m.update(s,.01f);assert(o.held[Sprint]&&o.held[Reload]&&o.held[Jump]);
 s.focused=false;assert(m.update(s,.01f).held.none());s.focused=true;assert(m.update(s,.01f).held.none());
 s.hand[0].stickClick=s.hand[1].stickClick=s.hand[1].secondary=false;m.update(s,.01f);s.hand[0].primary=true;s.hand[0].secondary=true;o=m.update(s,.01f);assert(o.held[Crouch]&&o.held[CycleTool]);assert(m.update(s,.11f).held.none());
 s.hand[0].x=std::numeric_limits<float>::quiet_NaN();assert(m.update(s,.01f).held.none());
 Keys sent,desired;desired.set(Forward);std::vector<int> events;auto sink=[&](Action a,bool down){events.push_back(int(a)*2+down);return true;};
 assert(dispatch(sent,desired,sink)&&events.size()==1);assert(dispatch(sent,desired,sink)&&events.size()==1);desired.reset();desired.set(Back);assert(dispatch(sent,desired,sink)&&events.size()==3&&events[1]==Forward*2&&events[2]==Back*2+1);
 desired.reset();desired.set(Jump);assert(!dispatch(sent,desired,[](Action,bool){return false;}));assert(sent[Back]&&!sent[Jump]);assert(dispatch(sent,Keys{},sink)&&sent.none());
}
