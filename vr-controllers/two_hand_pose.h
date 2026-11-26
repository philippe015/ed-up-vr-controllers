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
