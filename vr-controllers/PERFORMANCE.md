# VR performance optimization — experimental branch

## Changes
- Prepare FOV tangents, normalized poses and inverse eye orientation once per eye instead of once for each line. Preserve original near-plane and unit-square clipping.
- Cache the eight static numbered radial sector geometries.
- Group ClearView rectangles by color: up to two radial calls per eye instead of eight, and at most six calls across all visual groups instead of twelve. Keep line sampling density and bounded rectangle count. Highlight is drawn last. The fixed stack buffer grows to about 69 KiB; no per-frame heap allocation is introduced.
- Skip head-pose query if no enabled feature needs it, repeated empty diagnostic-channel writes after successful clearing, and haptic focus check when no cue is requested. Keep per-frame controller tracking, focus checks on input, watchdog and neutral rearming.
- Enable Release assertions for two existing tests; add reference-vs-prepared regression with 8192 generated line/pose cases and an informational CPU benchmark. No timing-dependent pass threshold.

## Measurement
Call counts are upper bounds from code, not measured frame-rate gains. The regression benchmark reports CPU projection time only. Validate on a real headset using identical scene, runtime, resolution and graphics settings, with overlays off/on. Record CPU/GPU frame time, median and 95th percentile, dropped/reprojected frames, and perceived input latency. Compare baseline and optimized build, including both hands, radial and wrist menus. No claimed gameplay FPS gain until measured. Full installer and headset validation are separate gates.