# ED-UP VR Controllers — v0.9.13 Experimental

**By Philou Cyberpunk — Elite Dangerous: Odyssey / Windows 11 x64 / Meta Quest 3 / OpenXR**

[Français](README.md) | **English**

## Automatic Windows test-tools installer

[![Download Windows x64](https://img.shields.io/badge/DOWNLOAD-WINDOWS%20x64-238636?style=for-the-badge&logo=windows)](https://github.com/philippe015/ed-up-vr-controllers/actions/runs/45894846539/artifacts/15152768437)

**[DOWNLOAD TEST INSTALLER (ZIP)](https://github.com/philippe015/ed-up-vr-controllers/actions/runs/45894846539/artifacts/15152768437)** — GitHub sign-in required, expires December 10, 2026.

[Open the test installer build workflow](https://github.com/philippe015/ed-up-vr-controllers/actions/workflows/windows-test-installer.yml)

Filename: `ED-UP-VR-Test-Setup-v0.9.13-x64.exe`. English/French wizard, per-user installation under LocalAppData without elevation, shortcuts, simulated viewer, read-only OpenXR diagnostic, configuration utility (Python 3.10+ required) and uninstall. CI builds the tools, runs CTest and checks installation/uninstallation before uploading the artifact. Windows build and automated install/uninstall tests passed on November 26, 2026. **This setup does not modify game DLLs and does not by itself enable the new in-game VR controls.** Full patch build and packaging validation remain required.

### How to install: double-click the setup file

1. Download the ZIP above and select **Extract All** in Windows.
2. Open the extracted directory and double-click `ED-UP-VR-Test-Setup-v0.9.13-x64.exe`. Verify its origin and supplied SHA-256 checksum before running. The setup is unsigned; Windows SmartScreen may display a warning.
3. Select **English** or **Français** in the setup wizard.
4. Read and accept the experimental scope notice. Follow the wizard, optionally select the desktop shortcut, then click **Install**.
5. Setup automatically copies the tools to `%LOCALAPPDATA%\EDUP-VR-Test-Tools` without administrator privileges or changes to game DLLs.
6. Click **Finish**, then open **ED-UP VR Test Tools** in the Windows Start menu.

| Shortcut | Purpose |
|---|---|
| Hands viewer (simulated demo) | Demonstration without the game or headset. |
| Control Center (French interface) | Configure controls; requires Python 3.10+ with Tkinter and the py launcher. Select the game directory and explicitly save the INI file. |
| Read-only OpenXR diagnostic | Static runtime check; does not confirm physical Quest 3 connectivity. |
| Installation and test guide | Local FR/EN documentation. |
| Uninstall ED-UP VR Test Tools | Removes installed tools; separately saved game configuration is retained. |

Setup does not automatically download dependencies or launch the game. Testing the new controls in Odyssey still requires the full integrated patch. The tools are free to download.

> Unofficial experimental prototype. No validated complete installer is currently available. Full integrated Windows build and physical headset testing remain unconfirmed.

**[ACCESS THE WINDOWS BUILD AND FUTURE TEST ZIP](https://github.com/philippe015/ed-up-vr-controllers/actions/workflows/integrated-windows.yml?query=branch%3Afeature%2Fonfoot-vr-enhanced)**

[Validation PR #1](https://github.com/philippe015/ed-up-vr-controllers/pull/1) · [v0.9.9 source](https://github.com/philippe015/ed-up-vr-controllers/tree/feature/onfoot-vr-enhanced) · [Technical documentation](https://github.com/philippe015/ed-up-vr-controllers/blob/feature/onfoot-vr-enhanced/vr-controllers/README.md) · [Report an issue](https://github.com/philippe015/ed-up-vr-controllers/issues)

### Next development — v0.9.10: Odyssey weapon selection

Experimental direct selection: hold the support grip and deflect the dominant stick up (primary weapon) or down (secondary weapon). WeaponSelect is off by default; configure the new bindings in the Armes tab. Return to neutral between selections; firing is blocked during selection. The portable module tests and v0.9.10 Windows installer passed. The integrated patch and in-game headset tests still require validation.

[v0.9.10 technical notes](https://github.com/philippe015/ed-up-vr-controllers/blob/feature/onfoot-vr-enhanced/vr-controllers/README.md)

### v0.9.11 — SteamVR and OpenXR (validation pending)

New SteamVR / OpenXR tab: read-only configured runtime detection, indicative SteamVR, Meta or VDXR identification, manifest and runtime DLL presence checks. Runtime selection remains in the VR software settings. Physical controller connectivity and gameplay are not validated. Index, Vive and Pico profiles remain future work. The v0.9.11 installer build and automated tests passed.

### v0.9.12 — Valve Index (experimental)

Valve Index OpenXR profile added in source: A/B on both hands, sticks, triggers, analog squeeze, poses and haptics. Touch profiles unchanged. Windows module compilation and installer tests passed. Headset tests remain pending. [Build tracking](https://github.com/philippe015/ed-up-vr-controllers/actions).

### v0.9.13 — Immersive gestures (validation pending)

Hold support grip then dominant grip in a virtual zone: hip (secondary weapon), shoulder (primary weapon), opposite chest (EnergyLink, Profile Analyser or suit tool), dominant front chest (reload). EquipmentGestures off by default, scale 70–130%. These are keyboard shortcuts, not native 3D manipulation. Graphical radial menu and wrist UI remain future work. Windows build and automated tests passed. Hardware tests pending.

### v0.9.14 — 3D grab lab (experimental)

Patch-owned virtual cube: contact and grip grab, 6DoF translation and rotation, release. Visual/ObjectLab off by default, exclusive mode without game keyboard/mouse input. Requires the integrated patch; standalone viewer only shows a simulated cube. No native Odyssey object or physics access. Build and headset tests pending. [Technical documentation](https://github.com/philippe015/ed-up-vr-controllers/blob/feature/onfoot-vr-enhanced/vr-controllers/README.md).

## 1. Overview and features

This community project extends [ED Unofficial Patch](https://github.com/characterecho-sean/ed-unofficial-patch) with VR controller input for Odyssey on-foot gameplay. The following features are implemented in experimental source code; their operation in the game and headset still requires validation.

| Feature | Description |
|---|---|
| Movement | Support-hand stick: digital four-direction movement; sprint, crouch and jump buttons. |
| Camera control | Dominant stick: relative mouse movement, smooth turning or nominal snap turns. |
| Motion aiming | Dominant grip: relative camera rotation derived from the OpenXR controller pose. |
| Two-handed aiming | Experimental: hold dominant grip, then support grip. Rotation only, with no firing or walking. |
| Interactions | Physical button presses; primary and secondary actions via triggers. |
| Odyssey tools | Manual selection of EnergyLink, Profile Analyser, suit-specific tool, holster, flashlight and tool mode. |
| Virtual hands | Approximate wireframe representations of controller position and orientation; optional stereo overlay. |
| Visual pointer | Optional ray for visual reference, without collision detection or spatial selection. |
| Haptics | Brief pulses for selected inputs submitted to Windows, not confirmation of in-game success. |
| Handedness | Switch logical roles between left and right controllers. |
| Configuration | Local French-language configuration utility, physical AZERTY keyboard codes, duplicate-binding checks and INI backup. |
| Diagnostics | Separate Windows viewer showing tracked poses, input mode and handedness. |
| Safety controls | Manual F10 activation, neutral-input requirement, focus and tracking checks, approximately 100 ms watchdog subject to scheduling delays. |

**Limitations:** no progressive analog locomotion, anatomical finger tracking, independent 3D weapon/tool manipulation, direct interaction with screens or terminals, or automatic recharging. The overlay is a diagnostic visualization without scene occlusion. No measured FPS improvement is claimed. Do not use this software to circumvent game protections.

## 2. Requirements

- Windows 11 x64 PC with a working installation of Elite Dangerous: Odyssey.
- Meta Quest 3 with two Touch Plus controllers, working Quest Link USB or Air Link connection, and the appropriate active Meta or SteamVR OpenXR runtime.
- Integrated ED-UP build based on pinned upstream revision `ba1de96c84fab3a6dde3f915d4a7fc9bb40adff1`. Never mix DLLs from different builds.
- Python 3.10+ with Tkinter for the Control Center; Git, Visual Studio 2022 with C++ workload, CMake 3.20+ and Python 3.12 for building from source.
- Check Frontier's rules before testing. No official approval or compatibility guarantee is claimed.

## 3. Obtain the Windows build

1. Open the build link above and select the workflow for `feature/onfoot-vr-enhanced`.
2. Verify that the portable tests, standalone Windows module, integrated build and reference build have passed. Also check PR #1.
3. If available under **Artifacts**, download `ED-UP-VR-v0.9.9-Windows-x64-test-build`. A signed-in GitHub account may be required. Extract it into a separate test directory.
4. Verify the contents and SHA-256 hashes. Run PowerShell in the extracted directory:

```powershell
Get-Content .\SHA256SUMS | ForEach-Object {
    if ($_ -notmatch '^([0-9a-fA-F]{64})  (.+)$') { throw "Invalid SHA256 line" }
    $expected = $Matches[1]; $name = $Matches[2]
    $actual = (Get-FileHash -LiteralPath $name -Algorithm SHA256).Hash
    if ($actual -ne $expected) { throw "Hash mismatch: $name" }
    Write-Host "OK: $name"
}
```

**Important:** this artifact contains build outputs, not a complete installer, and may not include all redistributable dependencies. Checksums verify integrity, not the authenticity of a signed release. Stop if the workflow fails or files/checksums are missing. The older v0.9 source ZIP is not v0.9.9.

## 4. Installing the future complete package — conditional procedure

These steps describe the intended installation process after packaging has been validated. Do not install the currently announced incomplete build artifact.

1. Close the game, launcher and VR software. In Steam, open Library > Elite Dangerous > Properties > Installed Files > Browse. Locate the actual directory containing `EliteDangerous64.exe`; its path depends on the installation.
2. Back up all files that the installer or manual installation will replace, as well as patch settings and game bindings. Personal bindings are usually in `%LOCALAPPDATA%\Frontier Developments\Elite Dangerous\Options\Bindings`. Keep backups outside the game directory and record added files.
3. Once the complete package is validated, follow its ED-UP file layout instructions, including OpenVR/OpenXR subdirectories, dependencies and notices. Keep files from the same build together. Do not place every DLL in the game root indiscriminately. Do not use the official ED-UP launcher to install an experimental archive that it was not designed to accept.
4. Place the configuration file `edup-vr-tools.ini` beside `EliteDangerous64.exe` and configure bindings as described below. This module's VR functionality requires explicit activation.
5. Connect the headset, verify the active runtime and follow the progressive tests in section 8. If the game fails to start, restore the original files immediately.

The [Windows release plan](https://github.com/philippe015/ed-up-vr-controllers/blob/feature/onfoot-vr-enhanced/vr-controllers/WINDOWS-RELEASE-PLAN.md) remains subject to validation. Final DLL installation paths will be documented when the complete package is ready.

## 5. Configuration utility and settings

From the extracted program directory, or `vr-controllers` in the source tree, run:

```powershell
py -3 control_center.py
```

The current Control Center UI is in French. Select the directory containing `EliteDangerous64.exe`, assign physical keys, validate and save. Reproduce the same bindings in the game's Options > Controls > On Foot. Close the game before editing and restart afterward. One previous `.ini.bak` backup is retained. Bindings use physical Windows scan codes, not characters. The AZERTY examples below are not official default game bindings.

| INI section / option | Value / effect |
|---|---|
| Movement / LeftHanded | 0 right-handed, 1 left-handed |
| Movement / LookPixelsPerSecond | Default 900, maximum 3000 |
| Movement / SnapMode | 0 smooth; 1, 2, 3 nominal 15°, 30°, 45° |
| Movement / SnapPixelsPerDegree | Default 10, range 1–30; calibrate to game sensitivity |
| Movement / ExtraLookDeadzonePercent | 0–40, additional smooth-look deadzone |
| Movement / ToolLayer | 1 enables the tool layer |
| Movement / StrictTools | 1 blocks tool selection if sticks, triggers or dominant grip are not neutral |
| MotionAim / Enabled | 1 enables motion aiming; 0 uses stick only |
| MotionAim / TwoHandEnabled | Default 0; 1 enables experimental two-handed aiming |
| MotionAim / PixelsPerDegree | Default 10, range 1–30 |
| MotionAim / InvertY | 0 normal, 1 inverted |
| Visual / Hands, Overlay, Laser, Haptics | Default 0; set each to 1 to enable |
| Visual / HapticPercent | Default 25, intensity 0–100 |

For initial tests use MotionAim Enabled=0, TwoHandEnabled=0, SnapMode=0, Hands=0, Overlay=0, Laser=0, Haptics=0. Enable one option at a time. The pointer requires Hands or Overlay. Actual snap angles depend on game sensitivity.

## 6. Meta Quest 3 controls — right-handed profile

| Controller | Input | Action | AZERTY example |
|---|---|---|---|
| Left | Stick | Movement | Z S Q D |
| Left | Stick click | Sprint | Left Shift |
| Left | X | Crouch | C |
| Left | Y | Cycle tool | A |
| Left | Trigger | Secondary action | Right mouse button |
| Left | Grip | Hold to select tools | Internal modifier |
| Right | Stick | Camera orientation | Mouse movement |
| Right | Stick click | Reload weapon | R |
| Right | A | Interact | E |
| Right | B | Jump | Space |
| Right | Trigger | Primary action | Left mouse button |
| Right | Grip | Motion aiming (if enabled) | Internal modifier |

On QWERTY, the same physical movement scan codes correspond to W S A D, and the example AZERTY A key for cycling tools corresponds to Q. Always configure the actual keys in your game.

### Tool layer: hold the left grip

Release other inputs, then press only one button.

| Button | Function | Example physical key |
|---|---|---|
| X | EnergyLink | Number row 3 |
| Y | Profile Analyser | Number row 4 |
| A | Suit-specific tool | Number row 5 |
| B | Holster equipment | H |
| Left stick click | Flashlight | T |
| Right stick click | Change tool mode | M |

Release the grip and return to neutral before using a tool. To recharge at a power port, select EnergyLink, manually check transfer mode, orient the camera toward the port and use the right trigger. The Arc Cutter (Maverick) and Genetic Sampler (Artemis) use the suit-specific tool action. The module does not detect the port or confirm the outcome. For the left-handed profile, reverse the logical controller roles; X/Y and A/B follow those roles.

### Two-handed aiming

Enable TwoHandEnabled=1 and Enabled=1. Starting from neutral, hold the dominant grip first, then the support grip. Keep hands approximately 15–85 cm apart. Only relative camera rotation is emitted; no firing or movement. Release both grips and return to neutral to exit.

## 7. Enable and disable the module

In PowerShell, before launching the game through its normal launcher:

```powershell
$env:EDUP_VR_ONFOOT = '1'
```

The actual game process must inherit this environment variable. Fully close the launcher/Steam and restart it from the same environment if needed. Do not launch the game executable directly to bypass authentication or the normal launcher. Visual settings come from the INI file; legacy environment variables EDUP_VR_HANDS, EDUP_VR_OVERLAY, EDUP_VR_LASER and EDUP_VR_HAPTICS override them when explicitly set to 0 or 1.

Once on foot, with the game focused and both controllers neutral, press **F10** on the keyboard. F10 also disables the mode. After losing focus or on-foot context, deliberately reactivate when appropriate. This module is not intended for piloting ships or SRVs.

To disable completely, close the game and remove the variable from the launch environment:

```powershell
Remove-Item Env:EDUP_VR_ONFOOT -ErrorAction SilentlyContinue
```

If configured as a persistent user/system variable, remove it there too and restart the launcher.

### Windows hands viewer

```powershell
.\edup_hands_viewer.exe --demo
```

This is a simulated demonstration without a headset. For real poses, enable Hands=1, start the game's XR session, find the game PID in Task Manager and run:

```powershell
.\edup_hands_viewer.exe 12345
```

Replace 12345 with the actual PID. Return focus to the game and press F10. Use the matching protocol-v3 viewer from the same build. Mode labels appear on the desktop, not in an in-headset wrist menu.

## 8. First physical Quest 3 test

Use a clear space, preferably seated, without combat or consequential gameplay. Stop if you experience discomfort or unexpected movement.

- [ ] Run the viewer demonstration without the game.
- [ ] Verify normal VR gameplay with the module disabled.
- [ ] Enable F10 on foot and test simple movement and turning.
- [ ] Test interaction, jump, crouch and reload.
- [ ] Test tracking loss, Alt-Tab, controller disconnection and return to ship/SRV; verify that input stops.
- [ ] Test tool selection, manual recharge and tool mode changes.
- [ ] Test one-handed then two-handed aiming, snap turns and left-handed profile.
- [ ] Test wireframe hands, pointer and haptics separately.
- [ ] Measure FPS and stability with and without the module; check a 30-minute session.

Do not hold mapped physical keyboard keys simultaneously. The watchdog is not a guarantee against crashes or forced termination. If input gets stuck, release the controllers, disable with F10 if possible and close the game. See the [detailed test matrix](https://github.com/philippe015/ed-up-vr-controllers/blob/feature/onfoot-vr-enhanced/vr-controllers/VALIDATION-WINDOWS.md).

## 9. Uninstall and troubleshooting

Close the game and launcher. Remove EDUP_VR_ONFOOT from the launch environment, restore the backed-up ED-UP files and settings together, and remove only the added files identified by the future package instructions. Restore personal bindings if needed. Never blindly delete a DLL belonging to the game. If the game fails, use the launcher's file verification after backing up settings; verification may not remove added third-party files.

| Symptom | Check |
|---|---|
| No input | Runtime, both controllers tracked, on-foot context, focus, F10, inherited environment variable and game bindings |
| Wrong tool | Physical scan codes, duplicate bindings, in-game bindings and return to neutral |
| Unstable camera | Disable MotionAim, TwoHandEnabled and SnapMode; reduce sensitivity |
| Empty viewer | Hands=1, correct PID, active XR session, game focused, matching viewer version |
| No vibration | Haptics=1, intensity, active bindings and compatible runtime |

## 10. Build from source (developers)

Clone experimental sources and an isolated checkout of pinned upstream, check the integration anchors, apply the script and inspect the diff. Do not use a production branch.

```powershell
git clone -b feature/onfoot-vr-enhanced https://github.com/philippe015/ed-up-vr-controllers.git contribution
git clone https://github.com/characterecho-sean/ed-unofficial-patch.git upstream
git -C upstream checkout --detach ba1de96c84fab3a6dde3f915d4a7fc9bb40adff1
python contribution/appliquer-integration.py upstream --check
# Stop if the preceding command fails.
Copy-Item contribution/vr-controllers upstream/vr-controllers -Recurse
python contribution/appliquer-integration.py upstream
# Stop if the preceding command fails.
git -C upstream diff --check
git -C upstream diff
```

Build the standalone module and run CTest, then install hash-pinned dependencies and SDKs according to the [pinned upstream build documentation](https://github.com/characterecho-sean/ed-unofficial-patch/blob/ba1de96c84fab3a6dde3f915d4a7fc9bb40adff1/docs/building.md).

```powershell
cmake -S contribution/vr-controllers -B build-vr -A x64 "-DOPENXR_INCLUDE_DIR=$PWD/upstream/third_party/openxr/include"
cmake --build build-vr --config Release
ctest --test-dir build-vr -C Release --output-on-failure
# Continue only after tests pass and SDKs are verified.
Set-Location upstream
cmd /c "build.bat --wait-lock --jobs 4"
# Only after a successful build:
.\build\smoke.exe .\build\d3d11.dll
```

A full build and smoke test do not replace headset testing. Do not bypass upstream packaging checks for exact version stamps and build receipts. The proof workflow compares unmodified and integrated builds; a previous CPU benchmark failed for an unconfirmed reason.

## 11. Test reports and contributions

Include game version, source commit, OpenXR runtime, Link/Air Link, GPU, reproduction steps, results, FPS and relevant logs. Remove personal data and private paths. No automatic reports are sent. [Open an issue](https://github.com/philippe015/ed-up-vr-controllers/issues/new) or read the [upstream proposal #272](https://github.com/characterecho-sean/ed-unofficial-patch/issues/272).

## 12. Version history and license

v0.9.3 two-handed aiming; v0.9.4 handedness; v0.9.5 VR comfort; v0.9.6 Odyssey tools; v0.9.7 diagnostics; v0.9.8 stability; v0.9.9 Windows validation. An experimental v1.0 release is planned only after validation.

[Older v0.9 source archive (not an installer)](https://github.com/user-attachments/files/35450529/ED-UP-VR-Controllers-v0.9-Stability.zip). [MIT license](LICENSE). Third-party components retain their respective licenses.

Unofficial community contribution, not an official Frontier Developments project.
