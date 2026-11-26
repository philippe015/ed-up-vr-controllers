#include <windows.h>
// Match the upstream include order: Windows headers may already define min/max.
#include "host_onfoot.hpp"
// Compile the Windows host header and the actual Direct3D overlay function in CI.
void overlayCompileCheck(ID3D11Device* d,ID3D11DeviceContext* c,ID3D11RenderTargetView* r,const XrPosef& h,const XrView& v){
 edup::vrinput::hands::OverlayScope scope(nullptr);
 edup::vrinput::hands::drawOverlay(d,c,r,h,v,100,100);
}
