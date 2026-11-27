#pragma once
#include <cstdint>
namespace edup::vrinput::hands {
// Evidence of a complete overlay draw in both eyes, not a claim that a human saw it.
// Single owner thread; consumed once on the next input frame.
struct MenuReceipt {
 int64_t frame=0;uint64_t tick=0;unsigned eyes=0;int sector=-1;
 void mark(unsigned eye){if(eye<2)eyes|=1u<<eye;}
 int consume(int64_t next,uint64_t now){
  int result=eyes==3&&sector>=0&&sector<8&&frame>0&&next>frame&&tick>0&&now>=tick&&now-tick<100?sector:-1;
  *this={};return result;
 }
};
}
