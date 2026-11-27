#pragma once
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <d3d11_1.h>
#include <wrl/client.h>
#include <openxr/openxr.h>
#include "eye_projection.hpp"
#include "radial_wire.hpp"
namespace edup::vrinput::hands {
// Only active during the synchronous stereo compose call on its owning thread.
inline thread_local const Snapshot* activeOverlay=nullptr;
class OverlayScope {
 const Snapshot* previous_;
public:
 explicit OverlayScope(const Snapshot* s):previous_(activeOverlay){activeOverlay=s;}
 ~OverlayScope(){activeOverlay=previous_;}
 OverlayScope(const OverlayScope&)=delete;OverlayScope& operator=(const OverlayScope&)=delete;
};
inline Pose xrPose(const XrPosef& p){return {{p.position.x,p.position.y,p.position.z},{p.orientation.x,p.orientation.y,p.orientation.z,p.orientation.w}};}
// Diagnostic-only overlay. No shader, depth buffer, blending or game model changes.
// ClearView paints only bounded nonempty rectangles; never call with zero rectangles.
inline void drawOverlay(ID3D11Device* device,ID3D11DeviceContext* ctx,ID3D11RenderTargetView* rtv,
 const XrPosef& head,const XrView& eye,unsigned width,unsigned height){
 if(!activeOverlay||!device||!ctx||!rtv||!width||!height||width>16384||height>16384)return;
 D3D11_FEATURE_DATA_D3D11_OPTIONS options{};
 if(FAILED(device->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS,&options,sizeof(options)))||!options.ClearView)return;
 Microsoft::WRL::ComPtr<ID3D11DeviceContext1> context1;if(FAILED(ctx->QueryInterface(IID_PPV_ARGS(&context1))))return;
 const Fov f{eye.fov.angleLeft,eye.fov.angleRight,eye.fov.angleDown,eye.fov.angleUp};
 const float colors[6][4]={{.03f,.65f,1.f,1.f},{1.f,.45f,.04f,1.f},{.12f,1.f,.35f,1.f},{.9f,.9f,.15f,1.f},{.20f,.65f,.70f,1.f},{1.f,.65f,.12f,1.f}};
 // Bound the GPU work: 19 segments * at most 49 small rectangles per hand.
 std::array<D3D11_RECT,count*49> rects{};
 for(unsigned h=0;h<12;++h){if(h<2?activeOverlay->valid[h]!=1:h==2?activeOverlay->laserValid!=1:h==3?activeOverlay->objectVisible!=1:activeOverlay->radialVisible!=1)continue;unsigned n=0;
  const auto wheel=h>=4?radialWire(h-4):RadialWire{};
  for(unsigned i=0;i<(h<2?count:h==2?1u:h==3?12u:wheel.size);++i){Line2 l;if(!eyeLine(h<2?segment(*activeOverlay,h,i):h==2?activeOverlay->laser:h==3?activeOverlay->objectLines[i]:wheel.lines[i],xrPose(head),xrPose(eye.pose),f,l))continue;
   float dx=(l.b.x-l.a.x)*width,dy=(l.b.y-l.a.y)*height;
   unsigned steps=std::clamp(unsigned(std::ceil((std::max)(std::abs(dx),std::abs(dy))/3.f)),1u,48u);
   for(unsigned k=0;k<=steps;++k){float t=float(k)/steps;LONG x=LONG(std::lround(l.a.x*(width-1)+t*(l.b.x-l.a.x)*(width-1)));LONG y=LONG(std::lround(l.a.y*(height-1)+t*(l.b.y-l.a.y)*(height-1)));
    rects[n++]={(std::max)(0L,x-1),(std::max)(0L,y-1),(std::min)(LONG(width),x+2),(std::min)(LONG(height),y+2)};
   }
  }
  if(n)context1->ClearView(rtv,colors[h<4?h:(int(h-4)==activeOverlay->radialIndex?5:4)],rects.data(),n);
 }
}
}
#endif
