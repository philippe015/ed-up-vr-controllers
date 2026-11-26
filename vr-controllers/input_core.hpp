#pragma once
#include <openxr/openxr.h>
#include <array>
#include "safety.hpp"
#include "controller_profiles.hpp"
namespace edup::vrinput {
struct Hand {
 bool tracked=false, ready=false, primary=false, secondary=false, stickClick=false;
 float trigger=0, squeeze=0, x=0, y=0;
 XrPosef grip{{0,0,0,1},{0,0,0}}, aim{{0,0,0,1},{0,0,0}};
 XrPath profile=XR_NULL_PATH;
};
class InputCore {
public:
 InputCore()=default;
 InputCore(const InputCore&)=delete;
 InputCore& operator=(const InputCore&)=delete;
 // Call on the existing XR owner thread. Explicit shutdown before destroying instance.
 XrResult initialize(XrInstance instance, PFN_xrGetInstanceProcAddr resolver, bool touchPlusEnabled);
 // Pass all other action sets here: OpenXR only permits one attach per session.
 XrResult attach(XrSession session, const XrActionSet* other=nullptr, uint32_t count=0);
 XrResult poll(bool focused, XrSpace base, XrTime displayTime);
 XrResult pulse(unsigned hand, float strength);
 bool locateHead(XrSpace view, XrSpace base, XrTime time, XrPosef& pose) const;
 const std::array<Hand,2>& hands() const noexcept {return snapshot_;}
 XrActionSet actionSet() const noexcept {return set_;}
 void neutralize() noexcept;
 void releaseSession(); // call before xrDestroySession
 void shutdown(); // call after session destruction, before instance destruction
private:
 XrInstance instance_=XR_NULL_HANDLE; XrSession session_=XR_NULL_HANDLE;
 XrActionSet set_=XR_NULL_HANDLE;
 enum A {Grip,Aim,Trigger,Squeeze,Stick,Primary,Secondary,StickClick,Haptic,Count};
 std::array<XrAction,Count> actions_{};
 std::array<XrPath,2> paths_{};
 std::array<std::array<XrSpace,2>,2> spaces_{};
 std::array<Hand,2> snapshot_{}; std::array<Gate,2> gates_{};
 bool attached_=false;
#define EDUP_XR_FUNCS(X) X(StringToPath) X(CreateActionSet) X(DestroyActionSet) X(CreateAction) X(SuggestInteractionProfileBindings) X(AttachSessionActionSets) X(CreateActionSpace) X(DestroySpace) X(SyncActions) X(GetActionStateFloat) X(GetActionStateVector2f) X(GetActionStateBoolean) X(GetActionStatePose) X(LocateSpace) X(GetCurrentInteractionProfile) X(ApplyHapticFeedback) X(StopHapticFeedback)
#define DECL(n) PFN_xr##n n##_=nullptr;
 EDUP_XR_FUNCS(DECL)
#undef DECL
 XrResult suggest(ProfileKind profile);
};
}
