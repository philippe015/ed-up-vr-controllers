#include "controller_profiles.hpp"
#include <string_view>
using namespace edup::vrinput;
using namespace std::literals;
static_assert(profileSpec(ProfileKind::Index).path=="/interaction_profiles/valve/index_controller"sv);
static_assert(profileSpec(ProfileKind::Index).primary[0]=="/input/a/click"sv);
static_assert(profileSpec(ProfileKind::Index).primary[1]=="/input/a/click"sv);
static_assert(profileSpec(ProfileKind::Index).secondary[0]=="/input/b/click"sv);
static_assert(profileSpec(ProfileKind::Index).secondary[1]=="/input/b/click"sv);
static_assert(profileSpec(ProfileKind::Index).squeeze=="/input/squeeze/value"sv);
static_assert(profileSpec(ProfileKind::Index).stick=="/input/thumbstick"sv);
static_assert(profileSpec(ProfileKind::Index).full);
static_assert(profileSpec(ProfileKind::Touch).primary[0]=="/input/x/click"sv);
static_assert(profileSpec(ProfileKind::TouchPlus).secondary[0]=="/input/y/click"sv);
static_assert(!profileSpec(ProfileKind::Simple).full);
static_assert(profileSpec(ProfileKind::Simple).squeeze==nullptr);
int main(){return 0;}
