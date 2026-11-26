#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include "windows_onfoot.hpp"
#include <chrono>
#include <mutex>
#include <thread>
#include <stdexcept>
#include <cwchar>
#include <cerrno>
namespace edup::vrinput::foot {
using Clock=std::chrono::steady_clock;
Bindings azertyExample(){
 Bindings b{};auto key=[&](Action a,unsigned short s){b[a]={s,false,Binding::Keyboard};};
 key(Forward,0x11);key(Back,0x1f);key(Left,0x1e);key(Right,0x20); // Z S Q D on AZERTY
 key(Jump,0x39);key(Interact,0x12);key(Crouch,0x2e);key(Sprint,0x2a);key(CycleTool,0x10);key(Reload,0x13); // E C Shift A R
 b[Primary]={0,false,Binding::MouseLeft};b[Secondary]={0,false,Binding::MouseRight};
 // Tool bindings are disabled unless configured explicitly in the local INI.
 return b;
}
static unsigned readNumber(const wchar_t* path,const wchar_t* section,const wchar_t* key,const wchar_t* def,unsigned max,int radix=10){
 wchar_t v[32]{};const DWORD n=GetPrivateProfileStringW(section,key,def,v,32,path);
 if(n>=31)throw std::invalid_argument("setting too long");
 wchar_t* end=nullptr;errno=0;const auto value=std::wcstoul(v,&end,radix);
 if(errno||end==v||*end||value>max)throw std::invalid_argument("invalid setting");return static_cast<unsigned>(value);
}
Bindings loadToolBindings(const wchar_t* path,Bindings base){
 const wchar_t* names[]={L"Forward",L"Back",L"Left",L"Right",L"Jump",L"Interact",L"Crouch",L"Sprint",L"CycleTool",L"Reload",L"Primary",L"Secondary",L"EnergyLink",L"ProfileAnalyser",L"SuitTool",L"Holster",L"Flashlight",L"ToolMode"};
 static_assert(sizeof(names)/sizeof(*names)==Count);
 for(unsigned i=0;i<Count;++i){
  const bool tool=i>=EnergyLink;const auto& k=base[i];unsigned old=k.kind==Binding::MouseLeft?256:k.kind==Binding::MouseRight?257:k.kind==Binding::Keyboard?k.scan:0;
  wchar_t def[16]{};std::swprintf(def,16,L"0x%X",old);
  const unsigned v=readNumber(path,tool?L"Tools":L"Bindings",names[i],def,257,0);
  if(v==256||v==257){if(tool)throw std::invalid_argument("tool must use keyboard");base[i]={0,false,v==256?Binding::MouseLeft:Binding::MouseRight};}
  else if(v>127||v==0x44)throw std::invalid_argument("invalid physical scan code");
  else base[i]=v?Binding{static_cast<unsigned short>(v),false,Binding::Keyboard}:Binding{};
 }
 return base;
}
Settings loadAimSettings(const wchar_t* path){
 Settings s;
 s.motionAim=readNumber(path,L"MotionAim",L"Enabled",L"1",1)!=0;
 s.twoHandAim=readNumber(path,L"MotionAim",L"TwoHandEnabled",L"0",1)!=0;
 s.invertY=readNumber(path,L"MotionAim",L"InvertY",L"0",1)!=0;
 const auto sensitivity=readNumber(path,L"MotionAim",L"PixelsPerDegree",L"10",30);if(!sensitivity)throw std::invalid_argument("zero aim sensitivity");
 s.aimPixelsPerDegree=float(sensitivity);
 s.lookPixelsPerSecond=float(readNumber(path,L"Movement",L"LookPixelsPerSecond",L"900",3000));
 s.toolLayer=readNumber(path,L"Movement",L"ToolLayer",L"1",1)!=0;
 return s;
}
VisualSettings loadVisualSettings(const wchar_t* path){
 VisualSettings v;
 auto flag=[&](const wchar_t* key,const wchar_t* env){wchar_t e[4]{};auto n=GetEnvironmentVariableW(env,e,4);if(n==1&&(e[0]==L'0'||e[0]==L'1'))return e[0]==L'1';return readNumber(path,L"Visual",key,L"0",1)!=0;};
 v.hands=flag(L"Hands",L"EDUP_VR_HANDS");v.overlay=flag(L"Overlay",L"EDUP_VR_OVERLAY");v.laser=flag(L"Laser",L"EDUP_VR_LASER");v.haptics=flag(L"Haptics",L"EDUP_VR_HAPTICS");
 v.hapticStrength=readNumber(path,L"Visual",L"HapticPercent",L"25",100)/100.f;return v;
}
struct WindowsBridge::Impl {
 DWORD target; Bindings b; Mapper mapper; Keys sent; std::mutex mutex; std::thread worker;
 Clock::time_point last{}; uint64_t lastTick=0; bool active=false,quit=false,fault=false; HANDLE process=nullptr;
 Impl(DWORD pid,Bindings bindings,Settings cfg):target(pid),b(bindings),mapper(cfg){
  if(!pid)throw std::invalid_argument("invalid PID");
  for(unsigned i=0;i<Count;++i){if(b[i].kind==Binding::Keyboard&&!b[i].scan)throw std::invalid_argument("invalid scan code");for(unsigned j=0;j<i;++j)if(b[i].kind!=Binding::Disabled&&b[i].kind==b[j].kind&&(b[i].kind!=Binding::Keyboard||(b[i].scan==b[j].scan&&b[i].extended==b[j].extended)))throw std::invalid_argument("duplicate binding");}
  process=OpenProcess(SYNCHRONIZE,FALSE,pid);if(!process)throw std::runtime_error("target process not accessible");
  try{worker=std::thread([this]{watch();});}catch(...){CloseHandle(process);throw;}
 }
 bool focus() const {DWORD pid=0;HWND w=GetForegroundWindow();if(!w)return false;GetWindowThreadProcessId(w,&pid);return pid==target&&WaitForSingleObject(process,0)==WAIT_TIMEOUT;}
 bool send(Action a,bool down){const auto& k=b[a];if(k.kind==Binding::Disabled)return true;if(down&&!focus())return false;INPUT input{};
  if(k.kind==Binding::Keyboard){input.type=INPUT_KEYBOARD;input.ki.wScan=k.scan;input.ki.dwFlags=KEYEVENTF_SCANCODE|(k.extended?KEYEVENTF_EXTENDEDKEY:0)|(down?0:KEYEVENTF_KEYUP);}
  else {input.type=INPUT_MOUSE;input.mi.dwFlags=k.kind==Binding::MouseLeft?(down?MOUSEEVENTF_LEFTDOWN:MOUSEEVENTF_LEFTUP):(down?MOUSEEVENTF_RIGHTDOWN:MOUSEEVENTF_RIGHTUP);}
  return SendInput(1,&input,sizeof(input))==1;
 }
 bool release(){mapper.reset();active=false;return dispatch(sent,Keys{},[this](Action a,bool d){return send(a,d);});}
 void watch(){for(;;){std::this_thread::sleep_for(std::chrono::milliseconds(10));std::lock_guard<std::mutex> l(mutex);if(quit)return;if((active&&(sampleExpired(GetTickCount64(),lastTick)||!focus()))||(!active&&sent.any())){if(!release())fault=true;}}}
 InteractionReport submit(const Sample& sample){std::lock_guard<std::mutex> l(mutex);auto now=Clock::now();float dt=active?std::chrono::duration<float>(now-last).count():0;last=now;lastTick=GetTickCount64();
  if(!sample.enabled||!sample.focused||!focus()||fault||!sample.hand[0].ready||!sample.hand[1].ready){if(!release())fault=true;if(!sample.enabled&&sent.none())fault=false;return {};}
  if(dt>=.1f){if(!release())fault=true;return {};}
  active=true;auto out=mapper.update(sample,dt);const Keys before=sent;
  if(!dispatch(sent,out.held,[this](Action a,bool d){return send(a,d);})){fault=true;release();return {};}
  if(out.dx||out.dy){if(!focus()){if(!release())fault=true;return {};}
   INPUT m{};m.type=INPUT_MOUSE;m.mi.dx=out.dx;m.mi.dy=out.dy;m.mi.dwFlags=MOUSEEVENTF_MOVE;if(SendInput(1,&m,sizeof(m))!=1){fault=true;release();return {};}}
  if(!focus()){if(!release())fault=true;return {};}
  InteractionReport report{sent&~before,out.pointing};
  for(unsigned i=0;i<Count;++i)if(b[i].kind==Binding::Disabled)report.pressed.reset(i);
  return report;
 }
 ~Impl(){ {std::lock_guard<std::mutex> l(mutex);quit=true;release();}if(worker.joinable())worker.join();{std::lock_guard<std::mutex> l(mutex);for(int i=0;i<3&&sent.any();++i)release();}CloseHandle(process);}
};
WindowsBridge::WindowsBridge(unsigned long pid,Bindings b,Settings s):p_(std::make_unique<Impl>(pid,b,s)){}
WindowsBridge::~WindowsBridge()=default;
InteractionReport WindowsBridge::submit(const Sample& s){return p_->submit(s);}
void WindowsBridge::stop(){std::lock_guard<std::mutex> l(p_->mutex);if(!p_->release())p_->fault=true;}
}
#endif
