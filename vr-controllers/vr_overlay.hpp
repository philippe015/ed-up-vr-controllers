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
#include "wrist_panel.hpp"
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
 const auto& snapshot=*activeOverlay;
 if(snapshot.valid[0]!=1&&snapshot.valid[1]!=1&&snapshot.laserValid!=1&&snapshot.objectVisible!=1&&snapshot.radialVisible!=1)return;
 const EyeProjector projection(xrPose(head),xrPose(eye.pose),{eye.fov.angleLeft,eye.fov.angleRight,eye.fov.angleDown,eye.fov.angleUp});
 if(!projection.valid())return;
 D3D11_FEATURE_DATA_D3D11_OPTIONS options{};
 if(FAILED(device->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS,&options,sizeof(options)))||!options.ClearView)return;
 Microsoft::WRL::ComPtr<ID3D11DeviceContext1> context1;if(FAILED(ctx->QueryInterface(IID_PPV_ARGS(&context1))))return;
 const float colors[6][4]={{.03f,.65f,1.f,1.f},{1.f,.45f,.04f,1.f},{.12f,1.f,.35f,1.f},{.9f,.9f,.15f,1.f},{.20f,.65f,.70f,1.f},{1.f,.65f,.12f,1.f}};
 // Fixed buffers; submit at most six color batches per eye. Keep the exact
 // original sampling density and clipping. No heap allocation or GPU readback.
 std::array<D3D11_RECT,8*11*49> rects;
 unsigned n=0;
 auto append=[&](Segment line){
  Line2 l;if(!projection.line(line,l))return;
  float dx=(l.b.x-l.a.x)*width,dy=(l.b.y-l.a.y)*height;
  unsigned steps=std::clamp(unsigned(std::ceil((std::max)(std::abs(dx),std::abs(dy))/3.f)),1u,48u);
  if(n+steps+1>rects.size())return;
  for(unsigned k=0;k<=steps;++k){float t=float(k)/steps;LONG x=LONG(std::lround(l.a.x*(width-1)+t*(l.b.x-l.a.x)*(width-1)));LONG y=LONG(std::lround(l.a.y*(height-1)+t*(l.b.y-l.a.y)*(height-1)));
   rects[n++]={(std::max)(0L,x-1),(std::max)(0L,y-1),(std::min)(LONG(width),x+2),(std::min)(LONG(height),y+2)};
  }
 };
 auto flush=[&](unsigned color){if(n)context1->ClearView(rtv,colors[color],rects.data(),n);n=0;};
 for(unsigned h=0;h<2;++h){if(snapshot.valid[h]!=1)continue;for(unsigned i=0;i<count;++i)append(segment(snapshot,h,i));flush(h);}
 if(snapshot.laserValid==1){append(snapshot.laser);flush(2);}
 if(snapshot.objectVisible==1){for(auto line:snapshot.objectLines)append(line);flush(3);}
 if(snapshot.radialVisible==1){
  auto sector=[&](unsigned i){const auto& w=radialWire(i);for(unsigned j=0;j<w.size;++j)append(snapshot.wristVisible?wristSegment(w.lines[j],snapshot.wristCenter):w.lines[j]);};
  for(unsigned i=0;i<8;++i)if(int(i)!=snapshot.radialIndex)sector(i);
  flush(4);
  if(snapshot.radialIndex>=0&&snapshot.radialIndex<8){sector(unsigned(snapshot.radialIndex));flush(5);}
 }
}
}
#endif
