#include "onfoot.hpp"
#include <cassert>
#include <limits>
using namespace edup::vrinput::foot;
constexpr double rad=3.14159265358979323846/180;
Orientation yaw(double degrees){return {0,float(-std::sin(degrees*rad/2)),0,float(std::cos(degrees*rad/2)),true};}
Orientation pitch(double degrees){return {float(std::sin(degrees*rad/2)),0,0,float(std::cos(degrees*rad/2)),true};}
int main(){
 MotionAim m;auto o=m.update(true,yaw(0),10,false,.01f);assert(o.usable&&o.dx==0&&o.dy==0);
 o=m.update(true,yaw(10),10,false,.01f);assert(o.usable&&o.dx>=99&&o.dx<=101&&o.dy==0);
 o=m.update(true,yaw(10),10,false,.01f);assert(o.dx==0&&o.dy==0);
 m.update(false,yaw(90),10,false,.01f);o=m.update(true,yaw(90),10,false,.01f);assert(o.usable&&o.dx==0);
 o=m.update(true,yaw(140),10,false,.01f);assert(!o.usable&&o.dx==0);assert(!m.update(true,yaw(141),10,false,.01f).usable);
 m.reset();m.update(true,yaw(179),10,false,.01f);o=m.update(true,yaw(-179),10,false,.01f);assert(o.usable&&o.dx>=19&&o.dx<=21);
 m.reset();m.update(true,pitch(0),10,false,.01f);o=m.update(true,pitch(5),10,false,.01f);assert(o.dy<=-49&&o.dy>=-51);
 m.reset();m.update(true,pitch(0),10,true,.01f);o=m.update(true,pitch(5),10,true,.01f);assert(o.dy>=49&&o.dy<=51);
 m.reset();assert(!m.update(true,pitch(90),10,false,.01f).usable);
 m.reset();auto q=yaw(0);q.w=std::numeric_limits<float>::quiet_NaN();assert(!m.update(true,q,10,false,.01f).usable);
 m.reset();m.update(true,yaw(0),30,false,.01f);o=m.update(true,yaw(20),30,false,.01f);assert(o.dx==180);assert(m.update(true,yaw(20),30,false,.01f).dx==0);
 // Integration with existing mapper: no simultaneous right-stick rotation, left-hand movement preserved.
 Mapper map;Sample s;s.enabled=s.focused=true;auto& l=s.hand[0];auto& r=s.hand[1];l.ready=r.ready=true;r.aim=yaw(0);
 auto step=[&]{return map.update(s,.01f);};step();r.squeeze=.8f;r.x=1;l.y=.8f;auto a=step();assert(a.dx==0&&a.held[Forward]);r.aim=yaw(3);a=step();assert(a.dx>=29&&a.dx<=31&&a.held[Forward]);
 r.trigger=.9f;r.aim.tracked=false;a=step();assert(a.held.none()&&a.dx==0);r.aim=yaw(4);assert(step().held.none());r.squeeze=0;assert(step().held.none());r.trigger=0;r.x=l.y=0;step();r.trigger=.9f;assert(step().held[Primary]);
 // Tools override aiming, requiring neutral on layer changes.
 r.trigger=0;step();r.squeeze=.8f;step();l.squeeze=.8f;a=step();assert(a.dx==0&&a.held.none());r.aim=yaw(8);assert(step().dx==0);r.squeeze=0;step();l.primary=true;assert(step().held[EnergyLink]);
 l.primary=false;l.squeeze=0;step();step();r.x=.5f;assert(step().dx==4);
 // Disable motion mode for conventional stick rotation.
 Settings c;c.motionAim=false;Mapper legacy(c);s={};s.enabled=s.focused=true;s.hand[0].ready=s.hand[1].ready=true;legacy.update(s,.01f);s.hand[1].squeeze=.8f;s.hand[1].x=1;assert(legacy.update(s,.01f).dx==9);
}
