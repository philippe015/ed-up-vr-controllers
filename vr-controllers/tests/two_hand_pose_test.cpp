#include "two_hand_pose.h"
#include <limits>
using namespace edup::experimental;
int main() {
 Vec3 a{}, b{0,0,-0.4};
 auto r=twoHandDirection(a,b,true,true,true);
 if (!r.valid || std::abs(r.direction.z+1)>1e-9) return 1;
 if (twoHandDirection(a,b,true,true,false).valid) return 2;
 if (twoHandDirection(a,b,false,true,true).valid) return 3;
 if (twoHandDirection(a,b,true,false,true).valid) return 4;
 if (twoHandDirection(a,a,true,true,true).valid) return 5;
 if (twoHandDirection(a,{0,0,1},true,true,true).valid) return 6;
 if (twoHandDirection(a,{std::numeric_limits<double>::quiet_NaN(),0,0},true,true,true).valid) return 7;
 if (twoHandDirection(a,{std::numeric_limits<double>::infinity(),0,0},true,true,true).valid) return 8;
 auto left=twoHandDirection({-0.2,0,0},{-0.2,0,-0.4},true,true,true);
 if (!left.valid || std::abs(left.direction.z+1)>1e-9) return 9;
 TwoHandFilter filter;
 if (filter.update(a,b,true,true,true,true,0).valid) return 10;
 filter.update(a,b,true,true,true,false,0);
 if (!filter.update(a,b,true,true,true,true,0.01).valid) return 11;
 auto smooth=filter.update(a,{0.1,0,-0.4},true,true,true,true,0.02);
 if (!smooth.valid || smooth.direction.x<=0 || smooth.direction.x>=0.1) return 12;
 if (std::abs(std::hypot(smooth.direction.x,smooth.direction.y,smooth.direction.z)-1)>1e-9) return 13;
 if (filter.update(a,b,true,false,true,true,0.03).valid) return 14;
 if (filter.update(a,b,true,true,true,true,0.04).valid) return 15;
 filter.update(a,b,true,true,true,false,0.05);
 if (!filter.update(a,b,true,true,true,true,0.06).valid) return 16;
 if (filter.update(a,b,true,true,true,true,0.2).valid) return 17;
 filter.update(a,b,true,true,true,false,0.21);
 if (!filter.update(a,b,true,true,true,true,0.22).valid) return 18;
 if (filter.update(a,{0,0,0.4},true,true,true,true,0.23).valid) return 19;
 return 0;
}
