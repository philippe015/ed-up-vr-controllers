#include "object_lab.hpp"
#include <cassert>
#include <limits>
using namespace edup::vrinput::hands;
bool near(Vec a,Vec b){return lengthSquared(a-b)<1e-8f;}
int main(){
 ObjectLab lab;Pose head{};std::array<GrabHand,2> h{};
 h[0]={{{-.3f,-.25f,-.45f},{}},0,true};h[1]={{{.3f,-.25f,-.45f},{}},0,true};
 uint64_t now=0;auto tick=[&]{now+=10;return lab.update(head,true,h,now);};
 auto f=tick();assert(f.visible&&f.owner==-1);Vec start=f.object.p;
 h[1].grip=.8f;assert(tick().owner==-1); // remote grab rejected
 h[1].pose.p=start;assert(tick().owner==-1); // moving while held does not grab
 h[1].grip=0;tick();h[1].grip=.8f;assert(tick().owner==1);
 h[1].pose.p={.2f,-.1f,-.6f};h[1].pose.q={0,.70710678f,0,.70710678f};
 f=tick();assert(f.owner==1&&near(f.object.p,h[1].pose.p));assert(std::abs(f.object.q.y-.70710678f)<1e-5f);
 auto wire=objectWire(f.object,head);for(auto e:wire)assert(std::abs(lengthSquared(e.a-e.b)-.0121f)<1e-5f);
 h[1].grip=0;f=tick();assert(f.owner==-1);Vec released=f.object.p;
 h[1].pose.p={.5f,0,-.5f};f=tick();assert(near(f.object.p,released));
 h[0].pose.p=released;h[0].grip=.8f;assert(tick().owner==0);
 h[0].tracked=false;assert(!tick().visible);h[0].tracked=true;assert(tick().owner==-1);assert(tick().owner==-1);
 h[0].grip=0;tick();h[0].pose.p=tick().object.p;h[1].pose.p=h[0].pose.p;h[0].grip=h[1].grip=.8f;assert(tick().owner==-1);
 h[0].grip=h[1].grip=0;tick();
 // A stalled render thread cannot resume an existing grab.
 h[0].pose.p=tick().object.p;h[0].grip=.8f;assert(tick().owner==0);
 now+=100;assert(tick().owner==-1);assert(tick().owner==-1);
 h[0].grip=0;tick();
 h[0].pose.p.x=std::numeric_limits<float>::quiet_NaN();assert(!tick().visible);
}
