#include "safety.hpp"
#include <cassert>
#include <limits>
using namespace edup::vrinput;
int main(){Gate g;assert(!g.accept(true,true,false));assert(!g.accept(true,true,true));assert(g.accept(true,true,false));assert(!g.accept(false,true,false));assert(!g.accept(true,true,false));assert(!g.accept(true,true,true));assert(g.accept(true,true,false));assert(!g.accept(true,false,false));assert(!g.accept(true,true,false));assert(axis(.07f)==0);assert(axis(-.08f)==0);assert(axis(1)==1);assert(axis(-2)==-1);assert(unit(-1)==0);assert(unit(2)==1);assert(axis(std::numeric_limits<float>::quiet_NaN())==0);assert(unit(std::numeric_limits<float>::infinity())==0);}
