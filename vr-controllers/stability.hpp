#pragma once
#include <cmath>
#include <cstdint>
namespace edup::vrinput {
inline bool analogValid(float v,float lo,float hi) noexcept {
 return std::isfinite(v)&&v>=lo&&v<=hi;
}
inline bool poseComponentsValid(float x,float y,float z,float qx,float qy,float qz,float qw) noexcept {
 if(!std::isfinite(x)||!std::isfinite(y)||!std::isfinite(z)||!std::isfinite(qx)||!std::isfinite(qy)||!std::isfinite(qz)||!std::isfinite(qw))return false;
 const double n=double(qx)*qx+double(qy)*qy+double(qz)*qz+double(qw)*qw;
 return n>=.99&&n<=1.01;
}
// Treat clock reversal as stale. Threshold is a policy, not a hard real-time guarantee.
inline bool sampleExpired(uint64_t now,uint64_t last,uint64_t timeout=100) noexcept {
 return now<last||now-last>=timeout;
}
}
