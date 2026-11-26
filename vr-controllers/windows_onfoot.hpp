#pragma once
#ifdef _WIN32
#include "interaction.hpp"
#include <memory>
namespace edup::vrinput::foot {
// Physical scan codes (not virtual-key codes); match the configured game bindings.
struct Binding {unsigned short scan=0; bool extended=false; enum Kind {Disabled,Keyboard,MouseLeft,MouseRight} kind=Disabled;};
using Bindings=std::array<Binding,Count>;
Bindings azertyExample();
// Load numeric physical scan codes from the optional INI next to the game EXE.
Bindings loadToolBindings(const wchar_t* path, Bindings base=azertyExample());
Settings loadAimSettings(const wchar_t* path);
struct VisualSettings {bool hands=false,overlay=false,laser=false,haptics=false,objectLab=false;float hapticStrength=.25f;};
VisualSettings loadVisualSettings(const wchar_t* path);
class WindowsBridge {
 struct Impl; std::unique_ptr<Impl> p_;
public:
 // Opt-in: requires explicit enabled=true in every submitted sample. target PID is fixed.
 explicit WindowsBridge(unsigned long targetPid, Bindings bindings=azertyExample(), Settings settings={});
 ~WindowsBridge();
 WindowsBridge(const WindowsBridge&)=delete;WindowsBridge& operator=(const WindowsBridge&)=delete;
 InteractionReport submit(const Sample& sample);
 void stop(); // disables output and attempts to release all keys
};
}
#endif
