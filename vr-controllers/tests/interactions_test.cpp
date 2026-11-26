#include "interaction.hpp"
#include "eye_projection.hpp"
#include <cassert>
#include <limits>
using namespace edup::vrinput;
int main(){
 using namespace foot;
 Mapper m;Sample s;s.enabled=s.focused=true;for(auto& h:s.hand){h.ready=true;h.aim={0,0,0,1,true};}
 assert(!m.update(s,.01f).pointing);s.hand[1].squeeze=.8f;assert(m.update(s,.01f).pointing);
 s.hand[0].squeeze=.8f;assert(!m.update(s,.01f).pointing);s.hand[1].squeeze=0;assert(!m.update(s,.01f).pointing);
 s.focused=false;assert(!m.update(s,.01f).pointing);s.focused=true;s.hand[0].squeeze=0;assert(!m.update(s,.01f).pointing);
 s.hand[1].squeeze=.8f;assert(m.update(s,.01f).pointing);s.hand[1].ready=false;assert(!m.update(s,.01f).pointing);
 Keys k;k[Interact]=true;assert(cues(k).right&&!cues(k).left);k.reset();k[EnergyLink]=true;assert(cues(k).left&&!cues(k).right);k.reset();k[Primary]=true;assert(!cues(k).left&&!cues(k).right);
 HapticLimiter lim;assert(lim.permit(0,0));assert(!lim.permit(0,119));assert(lim.permit(1,119));assert(lim.permit(0,120));assert(!lim.permit(0,119));assert(!lim.permit(2,200));
 hands::Pose head{},aim{{.2f,-.2f,-.4f},{}};hands::Segment ray;assert(pointerRay(aim,head,ray));assert(std::abs(ray.a.z+.44f)<.0001f);assert(std::abs(ray.b.z+1.9f)<.0001f);
 hands::Line2 l,r;hands::Fov f{-.8f,.8f,-.8f,.8f};assert(hands::eyeLine(ray,head,{{-.032f,0,0},{}},f,l));assert(hands::eyeLine(ray,head,{{.032f,0,0},{}},f,r));assert(l.a.x>r.a.x);
 aim.p.x=10;assert(!pointerRay(aim,head,ray));aim.p.x=.2f;aim.q.w=std::numeric_limits<float>::quiet_NaN();assert(!pointerRay(aim,head,ray));
}
