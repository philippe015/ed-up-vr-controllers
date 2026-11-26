#include "onfoot.hpp"
#include <cassert>
using namespace edup::vrinput::foot;
int main(){
 Mapper m;Sample s;s.enabled=s.focused=true;auto& l=s.hand[0];auto& r=s.hand[1];l.ready=r.ready=true;
 auto step=[&]{return m.update(s,.01f);};step();
 // An already-held button cannot be reinterpreted on grip entry.
 l.primary=true;assert(step().held[Crouch]);l.squeeze=.8f;assert(step().held.none());assert(step().held.none());
 l.primary=false;assert(step().held.none());l.primary=true;assert(step().held[EnergyLink]);assert(!step().held[Crouch]);
 l.primary=false;l.secondary=true;assert(step().held.none());l.secondary=false;step();l.secondary=true;assert(step().held[ProfileAnalyser]);l.secondary=false;step();
 r.primary=true;assert(step().held[SuitTool]);r.primary=false;step();r.secondary=true;assert(step().held[Holster]);r.secondary=false;step();
 l.stickClick=true;assert(step().held[Flashlight]);l.stickClick=false;step();r.stickClick=true;assert(step().held[ToolMode]);r.stickClick=false;step();
 // Ambiguous simultaneous selections suppressed until release.
 l.primary=r.primary=true;assert(step().held.none());r.primary=false;assert(step().held.none());l.primary=false;step();
 l.primary=true;assert(step().held[EnergyLink]);l.primary=false;step();
 // Triggers are blocked while selecting tools, and cannot fire after exit until released.
 r.trigger=.9f;assert(step().held.none());l.squeeze=0;assert(step().held.none());assert(step().held.none());r.trigger=0;assert(step().held.none());r.trigger=.9f;assert(step().held[Primary]);
 r.trigger=0;step();l.squeeze=.8f;step();step();r.primary=true;assert(step().held[SuitTool]);
 s.focused=false;assert(step().held.none());s.focused=true;assert(step().held.none());l.squeeze=0;r.primary=false;step();r.primary=true;assert(step().held[Interact]);
 // Physical grip threshold is hysteretic, and locomotion is blocked in the layer.
 r.primary=false;step();l.squeeze=.8f;step();step();l.squeeze=.5f;l.y=1;assert(step().held.none());l.y=0;l.primary=true;assert(step().held[EnergyLink]);
 l.primary=false;l.squeeze=.3f;assert(step().held.none());assert(step().held.none());l.squeeze=0;step();l.y=1;assert(step().held[Forward]);
 // Strict selection rejects simultaneous stick movement and requires a clean release.
 Settings cfg;cfg.strictTools=true;Mapper strict(cfg);Sample q;q.enabled=q.focused=true;
 auto& a=q.hand[0];auto& b=q.hand[1];a.ready=b.ready=true;
 auto tick=[&]{return strict.update(q,.01f);};
 tick();a.squeeze=.8f;tick();tick();
 a.primary=true;b.x=.5f;assert(tick().held.none());
 b.x=0;assert(tick().held.none());a.primary=false;tick();
 a.primary=true;assert(tick().held[EnergyLink]);
 b.squeeze=.8f;assert(tick().held.none());b.squeeze=0;assert(tick().held.none());
 a.primary=false;tick();a.secondary=true;assert(tick().held[ProfileAnalyser]);
}
