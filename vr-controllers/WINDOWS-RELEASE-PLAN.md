# Windows x64 experimental release plan

Integration branch: feature/onfoot-vr-enhanced. Keep main unchanged until reviewed. No release or installer yet.

## Required gates
- Complete two-hand runtime integration, handedness, turning comfort and tool improvements; review each feature and retain opt-in defaults.
- Run portable CTest and standalone Windows CTest including two_hand_pose.
- Reproduce the CPU performance failure on pinned upstream and integrated builds; do not weaken the test.
- Run Integrated ED-UP Windows proof build manually on this branch; require all upstream rigs and smoke test to pass.
- Inspect upstream packaging instructions and redistribution licenses before assembling a complete Windows x64 ZIP. Include all required matching runtime binaries, configuration, licenses, French installation and rollback guide, SHA256 checksums and commit provenance. Do not mix DLLs across builds or include game files.
- Publish only as experimental prerelease after automated gates pass. Physical Quest 3 Link/Air Link, in-game input, tracking loss, context transitions, comfort and tool interactions remain unvalidated until tested separately.

## Current state
Two-hand geometry and smoothing are prototypes with CTest registration, not yet connected to the runtime. No complete Windows package is ready. Existing integrated workflow is manually dispatchable; no final build requested until changes are complete.
