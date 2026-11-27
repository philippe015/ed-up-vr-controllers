# Quality review — v0.9.14 / v0.9.15

Review date: 2026-11-27. Scope: recent controller, equipment gesture, Object Lab, Windows bridge, OpenXR diagnostic, configuration, viewer and CI paths. This is a targeted code review, not a full security audit.

## Verified baseline
- Source af409e0bfc32df34f162361d5eb608f50e353a46: [Windows module and installer](https://github.com/philippe015/ed-up-vr-controllers/actions/runs/45895233381), 13/13 CTest tests passed, silent install/uninstall passed, diagnostic ZIP produced.
- [Integrated Windows proof](https://github.com/philippe015/ed-up-vr-controllers/actions/runs/45895233293): both pinned upstream baseline and integrated jobs passed; raw test binaries staged. Upstream emitted compiler warnings, e.g. shadowing and narrowing; not a zero-warning build. No physical XR session verified. Raw DLL bundle is not a complete validated gameplay installer.
- Stable diagnostic installer: [v0.9.14 ZIP](https://github.com/philippe015/ed-up-vr-controllers/actions/runs/45895233381/artifacts/15153461970).

## Findings and changes in v0.9.15
| Finding | Correction | Verification |
|---|---|---|
| Object grab could resume after a long frame stall | Reset object lab at >=100 ms or backwards tick; re-arm only on released grips; verify transformed object bounds | Regression case added |
| Head pose located twice for lab/overlay | Reuse the same sampled pose for interaction and stereo visualization | Windows compile check |
| Input action activation could change without a profile change | Track active-action bitmask; require neutral on any mask transition | Compile check; live OpenXR test still required |
| Diagnostic accepted an oversized or malformed manifest | Limit to 1 MiB and verify object structure and nonempty format/library strings; matching PS validation | Malformed, oversized and invalid UTF-8 fixtures |
| Random mixed-input tests omitted equipment gesture mode | Enable gestures in deterministic fault-injection coverage; preserve exclusive tool-layer invariants (manual reload allowed) | Existing stability test extended |

## Remaining validation
1. Re-run Windows module and integrated baseline/comparison; do not promote v0.9.15 until tests pass.
2. Check focus loss, disconnect/reconnect, runtime switching, tracking loss and low-framerate operation on Quest 3 and Valve Index via SteamVR.
3. Test game binding pulses, equipment ergonomics, frame times, occlusion and release on actual hardware. Current overlay has no native scene-depth occlusion.
4. Full Windows installer for game binaries requires package/dependency/license review, backup/rollback and installation test. The diagnostic installer is not that installer.
5. Native Odyssey object physics and inventory interaction are not implemented; no game engine interface has been established.
6. Static analysis/sanitizers, automated UI tests and physical performance measurements are not yet established. Upstream warnings remain outside the scope of this targeted change.
