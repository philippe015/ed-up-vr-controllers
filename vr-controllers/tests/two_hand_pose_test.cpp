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
 return 0;
}
