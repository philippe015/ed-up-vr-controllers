#pragma once
#include "virtual_hands.hpp"
namespace edup::vrinput::hands {
// Head-local radial overlay. One wedge (4 edges) plus a seven-segment digit.
struct RadialWire {std::array<Segment,11> lines{};unsigned size=0;};
inline RadialWire buildRadialWire(unsigned i){
 RadialWire w;if(i>=8)return w;
 constexpr float step=.7853981634f;
 float a=i*step;
 auto p=[](float angle,float radius){return Vec{std::sin(angle)*radius,std::cos(angle)*radius,-.65f};};
 auto add=[&](Vec x,Vec y){w.lines[w.size++]={x,y};};
 Vec a1=p(a-.34f,.10f),a2=p(a+.34f,.10f),b1=p(a-.34f,.23f),b2=p(a+.34f,.23f);
 add(a1,a2);add(a2,b2);add(b2,b1);add(b1,a1);
 Vec c=p(a,.163f);
 constexpr float x=.013f,y=.023f;
 const Vec pts[]={c+Vec{-x,y,0},c+Vec{x,y,0},c+Vec{x,0,0},c+Vec{x,-y,0},c+Vec{-x,-y,0},c+Vec{-x,0,0}};
 const unsigned ends[][2]={{0,1},{1,2},{2,3},{3,4},{4,5},{5,0},{5,2}};
 constexpr unsigned masks[]={0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f};
 for(unsigned k=0;k<7;++k)if(masks[i]&(1u<<k))add(pts[ends[k][0]],pts[ends[k][1]]);
 return w;
}
inline const RadialWire& radialWire(unsigned i){
 static const auto cached=[](){std::array<RadialWire,8> w{};for(unsigned j=0;j<8;++j)w[j]=buildRadialWire(j);return w;}();
 static const RadialWire empty{};return i<cached.size()?cached[i]:empty;
}
}
