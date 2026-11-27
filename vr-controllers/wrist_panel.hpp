#pragma once
#include "virtual_hands.hpp"
namespace edup::vrinput::hands {
// Controller-relative, head-facing display; not a measured wrist skeleton.
// Reject positions outside a conservative central viewing region.
inline bool wristAnchor(Vec grip,Pose head,Vec& anchor){
 if(!finite(grip)||!finite(head.p)||!normalize(head.q))return false;
 auto local=rotate(inverse(head.q),grip-head.p);
 if(!finite(local)||local.z>-.28f||local.z<-.85f||std::abs(local.x)>.35f||local.y<-.40f||local.y>.20f)return false;
 anchor=local+Vec{0,.075f,0};
 if(std::abs(anchor.x)>(-anchor.z)*.65f||std::abs(anchor.y)>(-anchor.z)*.55f)return false;
 return true;
}
inline Segment wristSegment(Segment s,Vec anchor){
 auto transform=[&](Vec p){return anchor+Vec{p.x*.48f,p.y*.48f,0};};
 return {transform(s.a),transform(s.b)};
}
}
