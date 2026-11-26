#include "virtual_hands.hpp"
#include <cassert>
#include <limits>
using namespace edup::vrinput::hands;
int main(){Pose head{},g{{-.2f,-.25f,-.5f},{0,0,0,1}};auto a=make(g,head,true,0,0);assert(a.valid);auto b=make(g,head,true,1,1);assert(b.valid);assert(b.lines[6].b.y<a.lines[6].b.y);Pose shift{{1,2,3},{0,0,0,1}};g.p=g.p+shift.p;auto c=make(g,shift,true,0,0);assert(c.valid);assert(std::abs(c.lines[0].a.x-a.lines[0].a.x)<1e-5);Screen l{},r{};assert(project({0,0,-.5f},-.032f,1,l));assert(project({0,0,-.5f},.032f,1,r));assert(l.x>r.x);assert(!project({0,0,.1f},0,1,l));g.q.w=0;assert(!make(g,shift,true,0,0).valid);g.q.w=1;g.p.x=std::numeric_limits<float>::quiet_NaN();assert(!make(g,shift,true,0,0).valid);Snapshot s;set(s,0,a);assert(s.valid[0]==1);set(s,0,Wire{});assert(s.valid[0]==0);}
