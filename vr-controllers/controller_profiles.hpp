#pragma once
#include <array>
namespace edup::vrinput {
enum class ProfileKind {Touch, TouchPlus, Index, Simple};
struct ProfileSpec {const char* path;const char* squeeze;const char* stick;const char* stickClick;std::array<const char*,2> primary,secondary;bool full;};
constexpr ProfileSpec profileSpec(ProfileKind kind){
 switch(kind){
 case ProfileKind::Touch:return {"/interaction_profiles/oculus/touch_controller","/input/squeeze/value","/input/thumbstick","/input/thumbstick/click",{"/input/x/click","/input/a/click"},{"/input/y/click","/input/b/click"},true};
 case ProfileKind::TouchPlus:return {"/interaction_profiles/meta/touch_controller_plus","/input/squeeze/value","/input/thumbstick","/input/thumbstick/click",{"/input/x/click","/input/a/click"},{"/input/y/click","/input/b/click"},true};
 case ProfileKind::Index:return {"/interaction_profiles/valve/index_controller","/input/squeeze/value","/input/thumbstick","/input/thumbstick/click",{"/input/a/click","/input/a/click"},{"/input/b/click","/input/b/click"},true};
 case ProfileKind::Simple:return {"/interaction_profiles/khr/simple_controller",nullptr,nullptr,nullptr,{"/input/select/click","/input/select/click"},{"/input/menu/click","/input/menu/click"},false};
 }
 return {};
}
}
