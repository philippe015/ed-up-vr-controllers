#pragma once
#include "onfoot.hpp"
#include "virtual_hands.hpp"
namespace edup::vrinput::foot {
// Feedback acknowledges Windows input submission, never a game-side result.
struct InteractionReport {Keys pressed{};bool pointing=false;ControlMode mode=ControlMode::Neutral;};
struct Cue {bool left=false,right=false;};
inline Cue cues(const Keys& pressed){
 return {pressed[EnergyLink]||pressed[ProfileAnalyser]||pressed[Flashlight],
 pressed[Interact]||pressed[SuitTool]||pressed[Holster]||pressed[ToolMode]||pressed[SelectPrimaryWeapon]||pressed[SelectSecondaryWeapon]};
}
class HapticLimiter {
 std::array<uint64_t,2> last_{};std::array<bool,2> seen_{};
public:
 bool permit(unsigned hand,uint64_t now){if(hand>=2)return false;
  if(seen_[hand]&&(now<last_[hand]||now-last_[hand]<120))return false;
  seen_[hand]=true;last_[hand]=now;return true;
 }
};
// Visual aid only: no raycast, hit testing, target locking, or pointer clicks.
inline bool pointerRay(hands::Pose aim,hands::Pose head,hands::Segment& out){
 using namespace hands;
 if(!finite(aim.p)||!finite(head.p)||!normalize(aim.q)||!normalize(head.q))return false;
 Vec delta=aim.p-head.p;if(!finite(delta)||delta.x*delta.x+delta.y*delta.y+delta.z*delta.z>9.f)return false;
 auto local=[&](Vec p){return rotate(inverse(head.q),p-head.p);};
 auto dir=rotate(aim.q,{0,0,-1});out={local(aim.p+dir*.04f),local(aim.p+dir*1.50f)};
 return finite(out.a)&&finite(out.b);
}
}
