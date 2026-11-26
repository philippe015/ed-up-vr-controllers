#pragma once
#include "virtual_hands.hpp"
#include <initializer_list>
namespace edup::vrinput::hands {
inline Quat compose(Quat a,Quat b){
 return {a.w*b.x+a.x*b.w+a.y*b.z-a.z*b.y,a.w*b.y-a.x*b.z+a.y*b.w+a.z*b.x,
 a.w*b.z+a.x*b.y-a.y*b.x+a.z*b.w,a.w*b.w-a.x*b.x-a.y*b.y-a.z*b.z};
}
inline float lengthSquared(Vec v){return v.x*v.x+v.y*v.y+v.z*v.z;}
struct GrabHand{Pose pose{};float grip=0;bool tracked=false;};
struct LabFrame{Pose object{};bool visible=false;int owner=-1;};
class ObjectLab{
 Pose object_{},offset_{};
 bool initialized_=false,armed_=false;
 int owner_=-1;
 std::array<bool,2> down_{};
 static bool valid(Pose& p){return finite(p.p)&&normalize(p.q);}
public:
 void reset(){*this=ObjectLab{};}
 LabFrame update(Pose head,bool headTracked,std::array<GrabHand,2> hand){
  if(!headTracked||!valid(head)){reset();return {};}
  for(auto& h:hand)if(!h.tracked||!valid(h.pose)||!std::isfinite(h.grip)||h.grip<0||h.grip>1||lengthSquared(h.pose.p-head.p)>4){reset();return {};}
  if(!initialized_){object_={head.p+rotate(head.q,{0,-.25f,-.45f}),head.q};initialized_=true;}
  if(lengthSquared(object_.p-head.p)>4){reset();return {};}
  if(!armed_){armed_=hand[0].grip<.05f&&hand[1].grip<.05f;return {object_,true,-1};}
  std::array<bool,2> edge{};
  for(unsigned i=0;i<2;++i){bool next=hand[i].grip>=(down_[i]?.40f:.70f);edge[i]=next&&!down_[i];down_[i]=next;}
  if(owner_>=0){
   auto& h=hand[owner_];
   if(!down_[owner_]){owner_=-1;armed_=false;down_={};return {object_,true,-1};}
   object_={h.pose.p+rotate(h.pose.q,offset_.p),compose(h.pose.q,offset_.q)};
   if(!valid(object_)){reset();return {};}
  }else if(edge[0]!=edge[1]){
   unsigned i=edge[1]?1:0;auto& h=hand[i];
   // Near contact only; preserve rigid 6DoF transform without snapping.
   if(!down_[1-i]&&lengthSquared(h.pose.p-object_.p)<=.12f*.12f){
    owner_=int(i);offset_={rotate(inverse(h.pose.q),object_.p-h.pose.p),compose(inverse(h.pose.q),object_.q)};
   }
  }
  return {object_,true,owner_};
 }
};
inline constexpr unsigned objectEdges=12;
inline std::array<Segment,objectEdges> objectWire(Pose object,Pose head){
 std::array<Segment,objectEdges> out{};
 if(!finite(object.p)||!finite(head.p)||!normalize(object.q)||!normalize(head.q))return out;
 std::array<Vec,8> v{};
 for(unsigned i=0;i<8;++i){
  Vec p{(i&1)?.055f:-.055f,(i&2)?.055f:-.055f,(i&4)?.055f:-.055f};
  v[i]=rotate(inverse(head.q),object.p+rotate(object.q,p)-head.p);
 }
 unsigned n=0;for(unsigned i=0;i<8;++i)for(unsigned bit:{1u,2u,4u})if(!(i&bit))out[n++]={v[i],v[i|bit]};
 return out;
}
}
