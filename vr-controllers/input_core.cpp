#include "input_core.hpp"
#include "stability.hpp"
#include <cstdio>
#include <string>
#include <vector>
namespace edup::vrinput {
namespace { constexpr const char* side[]={"/user/hand/left","/user/hand/right"};
bool validPose(const XrPosef& p){return poseComponentsValid(p.position.x,p.position.y,p.position.z,p.orientation.x,p.orientation.y,p.orientation.z,p.orientation.w);}
constexpr XrSpaceLocationFlags trackedFlags=XR_SPACE_LOCATION_POSITION_VALID_BIT|XR_SPACE_LOCATION_ORIENTATION_VALID_BIT|XR_SPACE_LOCATION_POSITION_TRACKED_BIT|XR_SPACE_LOCATION_ORIENTATION_TRACKED_BIT;
}
#define TRY(expr) do { const XrResult r=(expr); if(r!=XR_SUCCESS)return r; } while(false)
XrResult InputCore::initialize(XrInstance instance, PFN_xrGetInstanceProcAddr resolver, bool plus){
 if(instance_||!instance||!resolver)return XR_ERROR_VALIDATION_FAILURE;
 instance_=instance;
#define LOAD(n) {PFN_xrVoidFunction f=nullptr; XrResult r=resolver(instance_,"xr" #n,&f); if(XR_FAILED(r)||!f){shutdown();return XR_FAILED(r)?r:XR_ERROR_FUNCTION_UNSUPPORTED;} n##_=reinterpret_cast<PFN_xr##n>(f);}
 EDUP_XR_FUNCS(LOAD)
#undef LOAD
 auto build=[&]()->XrResult{
  for(unsigned i=0;i<2;++i)TRY(StringToPath_(instance_,side[i],&paths_[i]));
  XrActionSetCreateInfo si{XR_TYPE_ACTION_SET_CREATE_INFO};
  std::snprintf(si.actionSetName,sizeof(si.actionSetName),"edup_vr_input");
  std::snprintf(si.localizedActionSetName,sizeof(si.localizedActionSetName),"ED-UP VR Controllers");
  TRY(CreateActionSet_(instance_,&si,&set_));
  const char* names[]={"grip_pose","aim_pose","trigger","squeeze","thumbstick","primary","secondary","stick_click","vibration"};
  const XrActionType types[]={XR_ACTION_TYPE_POSE_INPUT,XR_ACTION_TYPE_POSE_INPUT,XR_ACTION_TYPE_FLOAT_INPUT,XR_ACTION_TYPE_FLOAT_INPUT,XR_ACTION_TYPE_VECTOR2F_INPUT,XR_ACTION_TYPE_BOOLEAN_INPUT,XR_ACTION_TYPE_BOOLEAN_INPUT,XR_ACTION_TYPE_BOOLEAN_INPUT,XR_ACTION_TYPE_VIBRATION_OUTPUT};
  for(unsigned i=0;i<Count;++i){ XrActionCreateInfo a{XR_TYPE_ACTION_CREATE_INFO}; a.actionType=types[i]; a.countSubactionPaths=2;a.subactionPaths=paths_.data();std::snprintf(a.actionName,sizeof(a.actionName),"%s",names[i]);std::snprintf(a.localizedActionName,sizeof(a.localizedActionName),"%s",names[i]);TRY(CreateAction_(set_,&a,&actions_[i]));}
  // Each profile is independent. A runtime can reject a profile it does not support.
  bool supported=false; XrResult last=XR_ERROR_PATH_UNSUPPORTED;
  auto attempt=[&](ProfileKind p){last=suggest(p);supported|=last==XR_SUCCESS;};
  attempt(ProfileKind::Touch);
  if(plus)attempt(ProfileKind::TouchPlus);
  attempt(ProfileKind::Index);
  attempt(ProfileKind::Simple);
  return supported?XR_SUCCESS:last;
 }; XrResult result=build();if(result!=XR_SUCCESS)shutdown();return result;
}
XrResult InputCore::suggest(ProfileKind kind){
 const auto profile=profileSpec(kind);
 XrPath p;TRY(StringToPath_(instance_,profile.path,&p));std::vector<XrActionSuggestedBinding> b;
 for(unsigned i=0;i<2;++i){
  auto add=[&](A action,const char* suffix)->XrResult{XrPath path;auto str=std::string(side[i])+suffix;TRY(StringToPath_(instance_,str.c_str(),&path));b.push_back({actions_[action],path});return XR_SUCCESS;};
  TRY(add(Grip,"/input/grip/pose"));TRY(add(Aim,"/input/aim/pose"));TRY(add(Haptic,"/output/haptic"));
  TRY(add(Primary,profile.primary[i]));TRY(add(Secondary,profile.secondary[i]));
  if(profile.full){TRY(add(Trigger,"/input/trigger/value"));TRY(add(Squeeze,profile.squeeze));TRY(add(Stick,profile.stick));TRY(add(StickClick,profile.stickClick));}
 }
 XrInteractionProfileSuggestedBinding info{XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING};info.interactionProfile=p;info.countSuggestedBindings=static_cast<uint32_t>(b.size());info.suggestedBindings=b.data();return SuggestInteractionProfileBindings_(instance_,&info);
}
XrResult InputCore::attach(XrSession session,const XrActionSet* other,uint32_t count){
 if(!set_||!session||session_||(count&&!other))return XR_ERROR_VALIDATION_FAILURE;
 session_=session;
 auto prepare=[&]()->XrResult{
  for(unsigned i=0;i<2;++i)for(unsigned j=0;j<2;++j){XrActionSpaceCreateInfo ci{XR_TYPE_ACTION_SPACE_CREATE_INFO};ci.action=actions_[j==0?Grip:Aim];ci.subactionPath=paths_[i];ci.poseInActionSpace.orientation.w=1;TRY(CreateActionSpace_(session_,&ci,&spaces_[i][j]));}
  std::vector<XrActionSet> sets;for(uint32_t i=0;i<count;++i)sets.push_back(other[i]);sets.push_back(set_);XrSessionActionSetsAttachInfo info{XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO};info.countActionSets=static_cast<uint32_t>(sets.size());info.actionSets=sets.data();return AttachSessionActionSets_(session_,&info);
 };auto r=prepare();if(r!=XR_SUCCESS){releaseSession();return r;}attached_=true;return XR_SUCCESS;
}
void InputCore::neutralize() noexcept {snapshot_={};gates_={};}
XrResult InputCore::poll(bool focused,XrSpace base,XrTime time){
 if(!attached_||!session_||!base){neutralize();return XR_ERROR_VALIDATION_FAILURE;}
 if(!focused){neutralize();return XR_SESSION_NOT_FOCUSED;}
 XrActiveActionSet active{set_,XR_NULL_PATH};XrActionsSyncInfo si{XR_TYPE_ACTIONS_SYNC_INFO};si.countActiveActionSets=1;si.activeActionSets=&active;auto result=SyncActions_(session_,&si);if(result!=XR_SUCCESS){neutralize();return result;}
 std::array<Hand,2> next{};
 for(unsigned i=0;i<2;++i){
  Hand h; XrActionStateGetInfo gi{XR_TYPE_ACTION_STATE_GET_INFO};gi.subactionPath=paths_[i];
  auto pose=[&](unsigned j,XrPosef& output){gi.action=actions_[j==0?Grip:Aim];XrActionStatePose s{XR_TYPE_ACTION_STATE_POSE};if(GetActionStatePose_(session_,&gi,&s)!=XR_SUCCESS||!s.isActive)return false;XrSpaceLocation l{XR_TYPE_SPACE_LOCATION};if(LocateSpace_(spaces_[i][j],base,time,&l)!=XR_SUCCESS||(l.locationFlags&trackedFlags)!=trackedFlags||!validPose(l.pose))return false;output=l.pose;return true;};
  h.tracked=pose(0,h.grip)&&pose(1,h.aim);
  XrInteractionProfileState ps{XR_TYPE_INTERACTION_PROFILE_STATE};if(GetCurrentInteractionProfile_(session_,paths_[i],&ps)==XR_SUCCESS)h.profile=ps.interactionProfile;
  if(snapshot_[i].profile!=h.profile)gates_[i].armed=false;
  bool ok=true;
  auto fl=[&](A a){gi.action=actions_[a];XrActionStateFloat s{XR_TYPE_ACTION_STATE_FLOAT};if(GetActionStateFloat_(session_,&gi,&s)!=XR_SUCCESS){ok=false;return 0.f;}if(s.isActive&&!analogValid(s.currentState,0.f,1.f)){ok=false;return 0.f;}return s.isActive?unit(s.currentState):0.f;};
  auto bo=[&](A a){gi.action=actions_[a];XrActionStateBoolean s{XR_TYPE_ACTION_STATE_BOOLEAN};if(GetActionStateBoolean_(session_,&gi,&s)!=XR_SUCCESS){ok=false;return false;}return s.isActive&&s.currentState==XR_TRUE;};
  h.trigger=fl(Trigger);h.squeeze=fl(Squeeze);h.primary=bo(Primary);h.secondary=bo(Secondary);h.stickClick=bo(StickClick);
  gi.action=actions_[Stick];XrActionStateVector2f s{XR_TYPE_ACTION_STATE_VECTOR2F};if(GetActionStateVector2f_(session_,&gi,&s)!=XR_SUCCESS)ok=false;else if(s.isActive){if(!analogValid(s.currentState.x,-1.f,1.f)||!analogValid(s.currentState.y,-1.f,1.f))ok=false;else {h.x=axis(s.currentState.x);h.y=axis(s.currentState.y);}}
  bool neutral=h.trigger<.05f&&h.squeeze<.05f&&!h.primary&&!h.secondary&&!h.stickClick&&h.x==0&&h.y==0;
  h.ready=gates_[i].accept(true,h.tracked&&ok&&h.profile!=XR_NULL_PATH,neutral);
  if(!h.ready){h.trigger=h.squeeze=h.x=h.y=0;h.primary=h.secondary=h.stickClick=false;}
  if(!h.tracked){h.grip=Hand{}.grip;h.aim=Hand{}.aim;}
  next[i]=h;
 }
 snapshot_=next;return XR_SUCCESS;
}
bool InputCore::locateHead(XrSpace view,XrSpace base,XrTime time,XrPosef& pose) const {
 if(!attached_||!LocateSpace_||!view||!base)return false;
 XrSpaceLocation l{XR_TYPE_SPACE_LOCATION};if(LocateSpace_(view,base,time,&l)!=XR_SUCCESS||(l.locationFlags&trackedFlags)!=trackedFlags||!validPose(l.pose))return false;pose=l.pose;return true;
}
XrResult InputCore::pulse(unsigned hand,float strength){
 if(hand>=2||!attached_||!snapshot_[hand].ready)return XR_ERROR_VALIDATION_FAILURE;
 XrHapticActionInfo ai{XR_TYPE_HAPTIC_ACTION_INFO};ai.action=actions_[Haptic];ai.subactionPath=paths_[hand];XrHapticVibration v{XR_TYPE_HAPTIC_VIBRATION};v.amplitude=unit(strength);v.duration=30000000;v.frequency=XR_FREQUENCY_UNSPECIFIED;return ApplyHapticFeedback_(session_,&ai,reinterpret_cast<const XrHapticBaseHeader*>(&v));
}
void InputCore::releaseSession(){neutralize();if(attached_&&StopHapticFeedback_)for(auto p:paths_){XrHapticActionInfo ai{XR_TYPE_HAPTIC_ACTION_INFO};ai.action=actions_[Haptic];ai.subactionPath=p;StopHapticFeedback_(session_,&ai);}for(auto& hand:spaces_)for(auto& space:hand){if(space&&DestroySpace_)DestroySpace_(space);space=XR_NULL_HANDLE;}session_=XR_NULL_HANDLE;attached_=false;}
void InputCore::shutdown(){releaseSession();if(set_&&DestroyActionSet_)DestroyActionSet_(set_);set_=XR_NULL_HANDLE;actions_={};paths_={};instance_=XR_NULL_HANDLE;}
#undef TRY
}
