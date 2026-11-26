# v0.9.9 — Windows x64 validation campaign

Branch: feature/onfoot-vr-enhanced. Main and upstream remain unchanged.

## Status
Source features v0.9.3–v0.9.8 are integrated in the branch, but not yet validated. The full proof workflow now targets the preview branch as well. Two independent Windows jobs use the same upstream pin and dependency verification: baseline (unmodified upstream) and integrated (with controllers). Integrated job runs the standalone C++ module and CTest with the upstream OpenXR headers before the full build. Both jobs require all upstream rigs and smoke test to pass. Compare CPU benchmark diagnostics; do not relax thresholds. No successful run is claimed until inspected.

## Packaging gates
- Inspect both job results and standalone/portable CTest; resolve regressions before packaging.
- Upstream package.bat requires a clean, matching full-build receipt and exact version stamp. Do not bypass this check: use a local integration commit/tag if packaging is later approved.
- Review redistribution licenses and launcher update behavior before assembling a Windows x64 experimental ZIP with matching DLLs, configuration, French instructions, rollback, SHA256 and provenance. No game files, no upstream signing identity.
- Publish as experimental prerelease only after automated gates pass. Quest 3 Link/Air Link, tracking, in-game controls and comfort remain separate physical tests.

No complete Windows ZIP is ready yet.
