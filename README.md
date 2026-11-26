# ED-UP VR Controllers — v0.9 Stability+

**By Philou Cyberpunk**

Experimental community contribution to [ED Unofficial Patch](https://github.com/characterecho-sean/ed-unofficial-patch), targeting on-foot VR gameplay in Elite Dangerous: Odyssey, Windows 11 x64, Meta Quest 3 and OpenXR.

## Windows VR test — v0.9.9 Experimental (Quest 3)

**[ACCESS THE WINDOWS TEST BUILD / ACCÉDER AU TEST WINDOWS](https://github.com/philippe015/ed-up-vr-controllers/actions/workflows/integrated-windows.yml?query=branch%3Afeature%2Fonfoot-vr-enhanced)**

[Validation tracking — PR #1](https://github.com/philippe015/ed-up-vr-controllers/pull/1) · [Latest experimental source code](https://github.com/philippe015/ed-up-vr-controllers/tree/feature/onfoot-vr-enhanced)

**Status: build requested, successful execution not yet confirmed.** Once the integrated build passes, look for `ED-UP-VR-v0.9.9-Windows-x64-test-build` under Artifacts. This is a test build output, not a complete installer; do not install until dependencies and packaging have been verified. Physical Quest 3 testing is pending.

**FR :** le lien ci-dessus ouvre les compilations de la branche expérimentale. Le ZIP de test n'est pas encore confirmé disponible. Ne pas mélanger les DLL avec une autre version.

---

## Source archive

[Download ED-UP-VR-Controllers-v0.9-Stability.zip](https://github.com/user-attachments/files/35450529/ED-UP-VR-Controllers-v0.9-Stability.zip)

The source archive is attached to [upstream proposal #272](https://github.com/characterecho-sean/ed-unofficial-patch/issues/272). It includes C++17 code, OpenXR controller actions, keyboard/mouse input bridge, virtual hands, experimental stereo overlay and visual laser pointer, haptics, French configuration interface, integration scripts, automated tests and Windows validation documentation. Browse the [source code and technical documentation](vr-controllers/README.md), [Windows validation plan](vr-controllers/VALIDATION-WINDOWS.md) and [MIT license](LICENSE).

## Status

Experimental prototype, not an installer or a stable VR release. Nine portable test suites were reported passing, including 20,000 simulated input samples. The standalone Windows x64 module and diagnostic viewer compiled successfully in [Windows CI](https://github.com/philippe015/ed-up-vr-controllers/actions/runs/45779308991). A separate [integrated proof build](https://github.com/philippe015/ed-up-vr-controllers/actions/workflows/integrated-windows.yml) now applies the experimental patch to a pinned upstream revision (`ba1de96c84fab3a6dde3f915d4a7fc9bb40adff1`) and checks the complete native build. That gate and physical Quest 3 testing remain unvalidated. No stable release or installer is available. Independent 3D weapon/tool manipulation and direct terminal interactions are not implemented.

MIT-licensed source contribution. Technical feedback and collaboration are welcome. This is an unofficial contribution, not an official Frontier Developments project.

## Collaboration

Please see [the original contribution proposal](https://github.com/characterecho-sean/ed-unofficial-patch/issues/272) for discussion.
