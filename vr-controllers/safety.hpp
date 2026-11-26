#pragma once
#include <algorithm>
#include <cmath>
namespace edup::vrinput {
inline float unit(float v) noexcept {return std::isfinite(v)?std::clamp(v,0.f,1.f):0.f;}
inline float axis(float v, float deadzone=.08f) noexcept {
 if(!std::isfinite(v)||!std::isfinite(deadzone)) return 0;
 v=std::clamp(v,-1.f,1.f); deadzone=std::clamp(deadzone,0.f,.95f);
 return std::abs(v)<=deadzone?0.f:std::copysign((std::abs(v)-deadzone)/(1.f-deadzone),v);
}
struct Gate {
 bool armed=false;
 bool accept(bool focused, bool tracked, bool neutral) noexcept {
  if(!focused||!tracked){armed=false;return false;}
  if(!armed){armed=neutral;return false;}
  return true;
 }
};
}
