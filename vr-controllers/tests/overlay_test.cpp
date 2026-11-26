#include "eye_projection.hpp"
#include <cassert>
#include <limits>
using namespace edup::vrinput::hands;
int main(){
 Fov f{-.8f,.8f,-.8f,.8f};Pose head{},left{{-.032f,0,0},{}},right{{.032f,0,0},{}};Line2 a,b;
 Segment s{{0,0,-1},{.1f,0,-1}};assert(eyeLine(s,head,left,f,a));assert(eyeLine(s,head,right,f,b));assert(a.a.x>b.a.x);assert(a.a.y==.5f);
 assert(!eyeLine({{0,0,1},{1,0,1}},head,left,f,a));
 assert(eyeLine({{0,0,0},{0,0,-1}},head,left,f,a));
 assert(eyeLine({{-100,0,-1},{100,0,-1}},head,left,f,a));assert(std::abs(a.a.x)<.00002f&&std::abs(a.b.x-1)<.00002f);
 assert(!eyeLine({{100,0,-1},{101,0,-1}},head,left,f,a));
 assert(!eyeLine(s,head,left,{0,0,-.8f,.8f},a));
 assert(!eyeLine(s,head,left,{-.8f,std::numeric_limits<float>::infinity(),-.8f,.8f},a));
 Pose moved{{2,1,3},{0,.70710678f,0,.70710678f}};Pose movedEye{moved.p+rotate(moved.q,left.p),moved.q};
 assert(eyeLine(s,moved,movedEye,f,a));assert(eyeLine(s,head,left,f,b));assert(std::abs(a.a.x-b.a.x)<.00001f);
 Fov asym{-.5f,.9f,-.7f,.8f};assert(eyeLine(s,head,head,asym,a));assert(std::abs(a.a.x-.5f)>.05f);
 Snapshot ss;ss.tick=100;assert(fresh(ss,200));assert(!fresh(ss,201));assert(!fresh(ss,99));ss.version=1;assert(!fresh(ss,100));ss.version=4;ss.controlMode=6;assert(!fresh(ss,100));ss.controlMode=4;ss.leftHanded=2;assert(!fresh(ss,100));ss.leftHanded=1;assert(fresh(ss,100));
}
