#pragma once
#include <cmath>
namespace edup::experimental {
struct Vec3 { double x{}, y{}, z{}; };
struct TwoHandResult { bool valid{}; Vec3 direction{}; };
// Pure geometry only: no input injection or automatic activation.
inline TwoHandResult twoHandDirection(Vec3 dominant, Vec3 support,
 bool dominantTracked, bool supportTracked, bool enabled) {
 if (!enabled || !dominantTracked || !supportTracked) return {};
 auto finite = [](Vec3 p) { return std::isfinite(p.x) && std::isfinite(p.y) && std::isfinite(p.z); };
 if (!finite(dominant) || !finite(support)) return {};
 Vec3 d{support.x-dominant.x,support.y-dominant.y,support.z-dominant.z};
 double n=std::hypot(d.x,d.y,d.z);
 if (!std::isfinite(n) || n<0.15 || n>0.85) return {};
 return {true,{d.x/n,d.y/n,d.z/n}};
}
}

namespace edup::experimental {
// Call once per fresh frame; seconds from a monotonic clock.
// Release activation after any invalid frame before rearming.
class TwoHandFilter {
 bool ready_=false, active_=false;
 double last_=0;
 Vec3 direction_{};
public:
 void reset() { ready_=false; active_=false; direction_={}; last_=0; }
 TwoHandResult update(Vec3 dominant, Vec3 support, bool trackedA,
 bool trackedB, bool enabled, bool activation, double now) {
  if (!enabled || !trackedA || !trackedB || !std::isfinite(now)) { reset(); return {}; }
  if (!activation) { reset(); ready_=true; return {}; }
  if (!ready_) return {};
  auto raw=twoHandDirection(dominant,support,trackedA,trackedB,enabled);
  if (!raw.valid) { reset(); return {}; }
  if (!active_) { active_=true; last_=now; direction_=raw.direction; return raw; }
  double dt=now-last_;
  if (!(dt>0 && dt<=0.1)) { reset(); return {}; }
  auto v=raw.direction;
  double dot=v.x*direction_.x+v.y*direction_.y+v.z*direction_.z;
  if (dot<0.5) { reset(); return {}; }
  double a=-std::expm1(-dt/0.06);
  Vec3 p{direction_.x+a*(v.x-direction_.x),direction_.y+a*(v.y-direction_.y),direction_.z+a*(v.z-direction_.z)};
  double n=std::hypot(p.x,p.y,p.z);
  if (!std::isfinite(n) || n<1e-9) { reset(); return {}; }
  direction_={p.x/n,p.y/n,p.z/n}; last_=now;
  return {true,direction_};
 }
};
}
