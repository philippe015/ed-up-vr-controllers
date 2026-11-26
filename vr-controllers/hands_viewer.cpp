#include "hands_channel.hpp"
#include "interaction.hpp"
#include <cstdlib>
#include <cstdio>
#include <string_view>
using namespace edup::vrinput::hands;
static Channel channel;static bool demo=false;static DWORD pid=0;
static Snapshot sample(){Snapshot s;Pose h{};set(s,0,make({{-.19f,-.19f,-.48f},{}},h,true,.5f,.1f));set(s,1,make({{.19f,-.19f,-.48f},{}},h,false,.1f,.7f));s.laserValid=edup::vrinput::foot::pointerRay({{.19f,-.19f,-.48f},{}},h,s.laser)?1u:0u;return s;}
static LRESULT CALLBACK proc(HWND w,UINT msg,WPARAM wp,LPARAM lp){
 if(msg==WM_TIMER){InvalidateRect(w,nullptr,FALSE);return 0;}
 if(msg==WM_DESTROY){PostQuitMessage(0);return 0;}
 if(msg==WM_PAINT){PAINTSTRUCT ps;HDC dc=BeginPaint(w,&ps);RECT rc;GetClientRect(w,&rc);FillRect(dc,&rc,(HBRUSH)GetStockObject(BLACK_BRUSH));SetBkMode(dc,TRANSPARENT);SetTextColor(dc,RGB(230,230,230));
  Snapshot s{};bool ok=demo;if(demo)s=sample();else ok=channel.read(s);
  const wchar_t* label=demo?L"DEMO - poses simulees, pas de casque":ok?L"Diagnostic des mains - projection stereo simulee":L"En attente de poses valides (F10 / suivi / focus)";
  TextOutW(dc,12,10,label,lstrlenW(label));
  int width=rc.right/2,height=rc.bottom-55;float scale=(width<height?width:height)*.46f;
  for(int eye=0;eye<2;++eye){int cx=width*eye+width/2,cy=35+height/2;RECT clip{width*eye,35,width*(eye+1),rc.bottom};int save=SaveDC(dc);IntersectClipRect(dc,clip.left,clip.top,clip.right,clip.bottom);const wchar_t* name=eye?L"Oeil droit":L"Oeil gauche";TextOutW(dc,width*eye+10,38,name,lstrlenW(name));
   if(ok)for(unsigned h=0;h<3;++h)if(h<2?s.valid[h]==1:s.laserValid==1){HPEN pen=CreatePen(PS_SOLID,2,h==2?RGB(40,255,100):h?RGB(255,170,60):RGB(80,210,245));auto old=SelectObject(dc,pen);for(unsigned i=0;i<(h<2?count:1u);++i){const float ray[6]={s.laser.a.x,s.laser.a.y,s.laser.a.z,s.laser.b.x,s.laser.b.y,s.laser.b.z};const float* p=h<2?s.points[h][i]:ray;Screen a{},b{};if(!project({p[0],p[1],p[2]},eye?.032f:-.032f,1,a)||!project({p[3],p[4],p[5]},eye?.032f:-.032f,1,b))continue;if(std::abs(a.x)>8||std::abs(a.y)>8||std::abs(b.x)>8||std::abs(b.y)>8)continue;MoveToEx(dc,cx+int(scale*a.x),cy-int(scale*a.y),nullptr);LineTo(dc,cx+int(scale*b.x),cy-int(scale*b.y));}SelectObject(dc,old);DeleteObject(pen);}RestoreDC(dc,save);}
  EndPaint(w,&ps);return 0;}
 return DefWindowProcW(w,msg,wp,lp);
}
int main(int argc,char** argv){demo=argc==2&&std::string_view(argv[1])=="--demo";if(!demo){if(argc!=2){std::puts("Usage: edup_hands_viewer.exe <PID> | --demo");return 1;}char* end=nullptr;unsigned long p=std::strtoul(argv[1],&end,10);if(!p||!end||*end){std::puts("PID invalide");return 1;}pid=static_cast<DWORD>(p);if(!channel.open(pid,false)){std::puts("Canal absent. Lancer ED-UP avec EDUP_VR_HANDS=1 puis activer F10 a pied.");return 2;}}
 auto instance=GetModuleHandleW(nullptr);WNDCLASSW wc{};wc.lpfnWndProc=proc;wc.hInstance=instance;wc.lpszClassName=L"EDUPHandsViewer07";wc.hCursor=LoadCursorW(nullptr,MAKEINTRESOURCEW(32512));if(!RegisterClassW(&wc))return 3;HWND w=CreateWindowExW(0,wc.lpszClassName,L"ED-UP Virtual Hands v0.7 - diagnostic",WS_OVERLAPPEDWINDOW|WS_VISIBLE,CW_USEDEFAULT,CW_USEDEFAULT,900,570,nullptr,nullptr,instance,nullptr);if(!w)return 4;SetTimer(w,1,33,nullptr);MSG m;while(GetMessageW(&m,nullptr,0,0)>0){TranslateMessage(&m);DispatchMessageW(&m);}return 0;}
