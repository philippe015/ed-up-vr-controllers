#pragma once
#include "input_core.hpp"
#include "onfoot.hpp"
namespace edup::vrinput::foot {
// Call on the existing XR owner thread after poll(). Explicit mode selection;
// no guessed game state. The game window focus is also checked by WindowsBridge.
inline Sample fromCore(const InputCore& core,bool onFootEnabled,bool xrFocused){
 Sample s;s.enabled=onFootEnabled;s.focused=xrFocused;
 for(unsigned i=0;i<2;++i){const auto& h=core.hands()[i];auto& d=s.hand[i];d.ready=h.ready&&h.tracked;d.primary=h.primary;d.secondary=h.secondary;d.stickClick=h.stickClick;d.x=h.x;d.y=h.y;d.trigger=h.trigger;d.squeeze=h.squeeze;const auto& q=h.aim.orientation;d.aim={q.x,q.y,q.z,q.w,h.tracked};d.gripPosition={h.grip.position.x,h.grip.position.y,h.grip.position.z};d.gripTracked=h.tracked;}
 return s;
}
}
