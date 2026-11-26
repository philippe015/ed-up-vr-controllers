# On-Foot VR Enhanced — independent development

Experimental branch; no stable release or physical Quest 3 validation.

## Milestone 1: stability
- Reproduce engine_motion_cpu_test on pinned upstream and integrated builds under comparable Windows CI conditions. Preserve the fivefold threshold and report raw timings.
- Preserve focus, tracking, neutral rearm and watchdog safeguards.

## Milestone 2: comfort
- Prototype configurable dominant hand and stick response; preserve existing defaults.
- Evaluate snap turning through relative input only; do not claim exact in-game angles without calibration.
- Test neutral transitions, invalid samples, focus loss and simultaneous inputs.

## Milestone 3: tools
- Improve explicit EnergyLink, scanner and suit tool selection and user feedback.
- Haptic feedback confirms submitted input only, not successful in-game interaction.
- Evaluate two-hand pose stabilization as a relative camera input prototype, without automatic targeting or recoil compensation.

## Milestone 4: research
- Independent weapon/tool poses, physical terminal interaction and wrist UI require verified game/render interfaces; do not fabricate game state or imply implementation.

## Release gates
Portable tests, standalone Windows build, integrated Windows build and separate physical Quest 3 tests. Keep features opt-in. Do not publish a stable installer until all relevant gates pass.
