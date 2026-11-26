# v0.9.7 — Controller diagnostics (experimental)

The local desktop hands viewer displays the mapper mode and handedness from the actual bridge report. Snapshot protocol v3 uses a separate EDUPHands09 channel; use matching viewer and runtime binaries. Old viewers cannot connect to the new channel. Headset wireframe rendering remains as before; mode labels are desktop only, not an in-headset wrist menu. No game-side equipment status is inferred. The local channel remains optional and nonblocking. Protocol validation rejects unknown modes and invalid handedness. Automated and Windows tests pending.

---

# v0.9.6 — Odyssey Tools Enhanced (experimental)

The French Control Center now has a Préréglage outils button. It proposes six example keyboard bindings (3, 4, 5, H, T, M), checks conflicts with existing actions, and modifies only the form until the user explicitly saves. Match bindings in the game manually. `[Movement] StrictTools=1` optionally blocks tool selection when either stick is outside neutral or the dominant grip or triggers are held; release conflicting inputs and all selection buttons before retrying. Default 0 preserves existing behavior. Existing haptic feedback still confirms input submission only, never game-side success. EnergyLink, scanner and suit-specific tools remain manual; the module does not detect a charging port or tool state. New C++ and Python regression tests are included but not yet verified. Windows compilation and physical headset tests remain pending.

---

# v0.9.5 — VR Comfort (experimental)

New French Control Center Confort VR tab. `[Movement] SnapMode=0` retains continuous rotation; 1, 2, 3 select nominal 15, 30, 45 degree turns. `SnapPixelsPerDegree=10` is a calibration setting (1–30); actual in-game angles are not guaranteed because output is a relative mouse pulse, not a native OpenXR camera rotation. Stick horizontal threshold 0.75, rearm below 0.20. No repeated or queued turns. Motion clutch and tool transitions clear the latch. `ExtraLookDeadzonePercent=0` (0–40) adds deadzone to continuous look axes after InputCore processing; it does not alter movement or snap thresholds. Existing defaults unchanged. Both handedness profiles supported. Restart required after saving. Automated motion regression tests added; Windows build and headset comfort not yet verified.

---

# v0.9.4 — Dual Hand Profile (experimental)

New `[Movement] LeftHanded=1` mirrors all logical controller roles (default 0). Left-handed mode: left stick looks, right stick moves; left grip aims and right grip selects tools. For two-hand mode hold dominant grip first, then support grip. Laser and haptic channels follow the selected roles. Physical hand rendering remains unchanged. The French Control Center exposes LeftHanded and TwoHandEnabled. Restart the game after changes. Two-hand mode remains opt-in; no firing or walking in that mode. Tests cover mirrored movement, two-hand aim and tool selection. Compilation and headset testing pending.

---

# v0.9.3 — Experimental two-hand motion integration

Option `[MotionAim] TwoHandEnabled=1` (default 0; requires Enabled=1). From neutral, hold right grip first to enter normal motion aim, then hold left grip to enter two-hand mode. Left grip alone retains tool selection. While both grips are held, only relative camera rotation is emitted; ordinary actions including firing and movement are suppressed. Release both controllers to neutral to resume. Tracking loss, invalid geometry, button/trigger activation or discontinuity resets the mapper. The single-hand laser is intentionally not displayed in two-hand mode. No independent 3D weapon control, no automatic targeting. Experimental right-dominant mode only. Windows build and physical tests pending.

---

# v0.9.0 — Stability+ (prototype)

Validation renforcée des données analogiques (NaN, infinis et valeurs hors limites) et des poses OpenXR (coordonnées finies, quaternion normalisé). Les valeurs invalides bloquent le mapper au lieu de l’armer au neutre. Surveillance du délai de 100 ms avec horloge monotone Windows. Tests de défauts et 20 000 échantillons pseudo-aléatoires reproductibles. Diagnostic statique du runtime OpenXR Windows et matrice d’essais physiques dans `VALIDATION-WINDOWS.md`.

La v0.9 ne prétend pas être stable sur matériel réel : MSVC, compilation upstream, Quest 3 et jeu restent à tester. Le workflow GitHub compile la bibliothèque et le visualiseur, pas ED-UP complet. Le script `publier-github.sh` cible la branche `feature/openxr-onfoot-stability`. Aucun binaire installé ni publication effectuée.

---

# v0.8.0 — VR Control Center (FR)

Le panneau de configuration local est un programme Python 3.10+ avec Tkinter (inclus dans l’installation Windows standard de Python). Double-cliquer sur `ouvrir-control-center.bat` dans `vr-controllers`, ou exécuter `py -3 control_center.py` depuis ce dossier. Aucun accès Internet, aucune injection de commandes par le panneau.

1. Fermer Elite Dangerous. Choisir le dossier contenant `EliteDangerous64.exe`.
2. Attribuer les touches de sortie physiques AZERTY aux actions des contrôleurs (les boutons Quest restent ceux du profil v0.7). Reproduire exactement ces liaisons dans les paramètres du jeu. Les outils non configurés sont désactivés par défaut ; le fichier exemple propose des codes à adapter.
3. Régler la visée, le stick, la couche outils, le visualiseur, les mains, le laser et les vibrations. Vérifier, puis enregistrer. Les valeurs en double, F10 et les valeurs hors limites sont refusées. Les réglages inconnus sont conservés, mais les commentaires et la mise en forme INI sont réécrits. Une seule copie précédente `.ini.bak` est conservée.
4. Redémarrer le jeu avec le patch intégré et compilé et `EDUP_VR_ONFOOT=1` dans son environnement. Les variables historiques EDUP_VR_HANDS, EDUP_VR_OVERLAY, EDUP_VR_LASER, EDUP_VR_HAPTICS, lorsqu'elles valent explicitement 0 ou 1, priment sur le fichier. F10 arme le profil à pied, retour au neutre requis. Le grip droit recale la visée relative.

Le diagnostic vérifie uniquement le fichier et les valeurs, pas les contrôleurs ni le runtime OpenXR. La configuration et la compilation du pont Windows, l’interface graphique et les tests Quest 3 restent à valider sur Windows. Les tests locaux exécutés sont les sept suites portables C++ et une suite Python (quatre cas de test). Aucune nouvelle version binaire ni publication GitHub n'est réalisée ici. Le workflow Windows compile le module et le visualiseur, pas le patch complet.

Les sections ci-dessous documentent les versions antérieures. Les options de v0.8 remplacent les valeurs fixes décrites dans ces sections.

---

# ED-UP VR Controllers v0.7.0 — VR Interactions (prototype)

Windows 11 x64, Meta Quest 3 par Quest Link/Air Link ou SteamVR/OpenXR. Module expérimental, désactivé par défaut, non officiel. Le ZIP contient les modules cumulés v0.1 à v0.7, un pont Windows et un script d'intégration optionnel. Ce n'est pas un installateur, ni un patch binaire validé.

## Nouveauté v0.7 : pointeur visuel et retour haptique

- `EDUP_VR_LASER=1` : activer le rayon vert de 1,5 m depuis la pose aim OpenXR du contrôleur droit, en maintenant son grip (mode visée existant). Nécessite `EDUP_VR_OVERLAY=1` pour le casque ou `EDUP_VR_HANDS=1` pour le visualiseur. Le rayon disparaît quand le grip est relâché, le mode outils est actif, le mapper n'est pas armé ou le suivi / focus est perdu. Le visualiseur `--demo` montre un rayon simulé.
- `EDUP_VR_HAPTICS=1` : impulsion facultative de 30 ms à 25 % pour A (interaction) et les sélections d'outils effectivement transmises à Windows. Limite de 120 ms entre impulsions sur chaque main. Une impulsion confirme uniquement la soumission de l'entrée Windows, pas la réussite de l'interaction dans le jeu. Aucun retour lors de la gâchette principale.
- Les deux options sont désactivées par défaut. F10 reste nécessaire en jeu. Pas de clic par rayon, détection de collision, sélection spatiale de menus, manipulation de terminaux par contact, ni confirmation de recharge. Pour interagir, orienter la caméra vers l'objet puis appuyer sur A. Le rayon n'est pas aligné de façon garantie avec le viseur du jeu et peut traverser les objets : c'est un repère de diagnostic.
- Canal de diagnostic local version 2, nom EDUPHands07 : utiliser le visualiseur v0.7 avec cette version. Pas de compatibilité de données avec le visualiseur v0.5.

Les tests portables couvrent la projection du rayon, le masquage par le mapper et la limitation des impulsions. Le pont Windows, les haptics et le rendu dans le casque restent non validés. Aucun changement aux points de raccordement upstream v0.6 ; ils doivent être revérifiés au moment de l'application.

## Nouveauté v0.6 : mains filaires dans le rendu stéréoscopique (expérimental)

Le raccordement ajoute un passage optionnel dans `D3D11Stereo::renderCaptured`, juste avant le fondu de confort et dans le même chemin de rendu que les overlays plugins. Il ne nécessite pas de plugin externe, de nouveau shader ou de nouvelle session OpenXR. Les poses de chaque œil et son champ asymétrique proviennent des `XrView` réels. Les segments sont projetés individuellement dans chaque œil ; la coupe au plan proche et aux bords évite les coordonnées invalides. Un rendu de diagnostic bleu à gauche et orange à droite est prévu via `ID3D11DeviceContext1::ClearView` sur des rectangles bornés (si le pilote annonce cette capacité). Pas d'appel si le tableau de rectangles est vide. Les segments longs peuvent apparaître pointillés. Cette approche est provisoire, sans profondeur de scène ni occultation par les objets, et son coût GPU doit être mesuré.

Activation après intégration et compilation du patch complet : avant le lancement du jeu, définir `EDUP_VR_ONFOOT=1` et `EDUP_VR_OVERLAY=1` dans l'environnement du processus qui lance le jeu ; `EDUP_VR_HANDS=1` reste facultatif pour le visualiseur indépendant. Sur une scène à pied, activer F10 et remettre les contrôleurs au neutre. Désactiver `EDUP_VR_OVERLAY` pour retrouver le rendu précédent. Le rendu est bloqué si le contexte à pied ou le focus n'est plus confirmé, si la pose est invalide, si son échantillon a plus de 100 ms lors de la composition, si son instant prédit diffère de celui de la frame, si une ancienne image est rejouée, ou si les corrections de zoom/partage ou le fondu sont actifs. Le passage n'est pas exécuté lorsque le pilote ne prend pas en charge ClearView. Aucune garantie d'affichage ni de performances sur matériel réel à ce stade.

Le code ne change pas les armes, outils, modèles du personnage ou collisions. La compilation MSVC et l'affichage dans le Quest 3 ne sont pas validés ; seul le calcul portable a été testé ici. Tester assis, hors combat, avec sauvegarde des DLL d'origine et vérifier les conditions d'utilisation de Frontier. Si le rendu est instable, désactiver le module.

## Visualiseur de mains hérité de la v0.5

Cette version ajoute une représentation filaire articulée de chaque main, construite à partir de la position et de l’orientation du grip OpenXR, avec flexion approximative des doigts par gâchette et grip. Ce n’est pas le suivi anatomique des doigts (`XR_EXT_hand_tracking`), ni une modification du modèle du personnage.

Le visualiseur Windows `edup_hands_viewer.exe` est séparé du jeu. Il projette la géométrie dans deux vues de diagnostic parallèles (écart interoculaire simulé 64 mm, champ simulé 90°), et non dans les swapchains du casque. Il peut être essayé avec `edup_hands_viewer.exe --demo` (poses simulées).

Pour la lecture des poses réelles, appliquer l’intégration, compiler le patch complet et définir `EDUP_VR_ONFOOT=1` et `EDUP_VR_HANDS=1` dans l’environnement du jeu avant lancement. Utiliser le PID d’EliteDangerous64.exe (Gestionnaire des tâches) : `edup_hands_viewer.exe <PID>`. Lancer le visualiseur après le démarrage de la session XR, puis rendre le focus au jeu et activer F10 à pied. Le visualiseur doit rester visible sur un deuxième écran ou à côté du jeu sans prendre son focus ; sinon les poses sont volontairement effacées. Une pose de tête est localisée dans le même espace LOCAL, au même instant prédit que les mains. Les données restent en mémoire partagée locale, avec verrou non bloquant, sans réseau ni historique. Les échantillons vieux de plus de 250 ms sont ignorés. Les poses non valides et les mains non réarmées sont masquées. Par défaut aucun canal de diagnostic n’est créé.

Le workflow Windows est prévu pour compiler le visualiseur, la bibliothèque et le nouveau contrôle de compilation des en-têtes Windows/Direct3D, pas le patch complet. Le visualiseur reste indépendant ; l'incrustation v0.6 passe par le chemin de composition du runtime, à côté de l'interface `onRenderEye` du [projet original](https://github.com/characterecho-sean/ed-unofficial-patch/blob/main/include/edvr_plugin_api.h).

## Profil Quest 3 proposé (AZERTY)

Configurer ces touches dans **Options > Commandes > À pied** avant tout essai. Ce ne sont pas des valeurs officielles du jeu ; elles sont modifiables dans `azertyExample()` de `windows_onfoot.cpp` (codes de touches physiques, pas caractères). Le grip gauche active la couche outils ; le grip droit active la visée par mouvement (voir v0.4).

| Contrôleur | Entrée | Action | Exemple de liaison |
|---|---|---|---|
| Gauche | Stick | Avancer/reculer/latéral | Z S Q D |
| Gauche | Appui stick | Sprint | Maj gauche |
| Gauche | X | S'accroupir | C |
| Gauche | Y | Changer d'outil (ou action libre) | A |
| Gauche | Gâchette | Action secondaire/visée | Clic droit |
| Droit | Stick | Orientation à la souris | Mouvement relatif |
| Droit | Appui stick | Recharger l'arme | R |
| Droit | A | Interagir | E |
| Droit | B | Sauter | Espace |
| Droit | Gâchette | Action principale | Clic gauche |

Le jeu doit accepter les entrées synthétiques de Windows ; cela n'est pas encore confirmé. Aucun verrouillage de cible, macro, répétition automatique, visée automatique ou modification des mécaniques. Le déplacement est numérique (pas une vitesse analogique). Le stick droit ne représente pas encore une visée indépendante de la tête ni la position de l'arme. La sélection directe des outils est implémentée dans le mapper ; le fonctionnement en jeu des outils et le pointeur laser restent à valider. Les mains filaires ne sont qu’un overlay de diagnostic, non un modèle du jeu.

## Nouveau : couche outils v0.3

Maintenir le grip gauche, relâcher les autres commandes, puis appuyer sur un seul bouton :

| Bouton | Commande du jeu à associer | Exemple physique |
|---|---|---|
| X gauche | Select EnergyLink / outil de transfert d'énergie | 3 (0x04) |
| Y gauche | Select Profile Analyser | 4 (0x05) |
| A droit | Select Suit Specific Tool | 5 (0x06) |
| B droit | Holster / ranger l'équipement | H (0x23) |
| Appui stick gauche | Lampe | T (0x14) |
| Appui stick droit | Changer le mode de l'outil | M (0x32) |

Copier `vr-controllers/edup-vr-tools.ini` à côté de `EliteDangerous64.exe` après avoir attribué les mêmes touches dans les commandes à pied. Les valeurs sont des codes physiques hexadécimaux Windows et **non des réglages officiels garantis**. Sur AZERTY, utiliser les touches de la rangée numérique. Mettre 0 pour désactiver une action. Sans INI, les six nouvelles liaisons sont désactivées (le grip garde néanmoins sa fonction de changement de couche). Les doublons avec les touches du profil de base et les valeurs invalides désactivent le pont à l'initialisation. Une erreur d'initialisation est signalée par OutputDebugStringW. Redémarrer le jeu après modification.

Pour recharger sur une borne : sélectionner EnergyLink avec grip gauche + X, relâcher complètement le grip et les boutons, orienter la vue vers la borne et maintenir la gâchette droite selon le fonctionnement de l'outil. Le mode transfert doit être sélectionné dans le jeu ; le module ne sait pas lire le mode courant de l'outil. Le changement de mode reste une action manuelle. Aucune recharge automatique, aucune interaction déclenchée à distance, aucun changement de mode automatique. Le même principe s'applique à l'échantillonneur génétique (Artemis) ou au découpeur à arc (Maverick), via grip + A. La v0.4 ajoute la rotation de caméra par pose de la main, voir ci-dessous.

La couche outils bloque déplacement, rotation et gâchettes. Les transitions imposent le retour au neutre ; les appuis multiples sont bloqués jusqu'au relâchement. La perte de focus/tracking réinitialise également la couche. Le watchdog précédent est conservé. Les tests portables vérifient la logique, pas le fonctionnement de SendInput sur Windows.

Référence des commandes : https://wiki.herzbube.ch/index.php/EDControls et https://dseelinger.github.io/d47/capabilities/on-foot-controls.html ; vérifier les libellés et touches dans le jeu installé.

## Nouveauté v0.4 : Motion Aiming

Maintenir le grip droit pour calibrer la direction actuelle de la main et convertir ses rotations en déplacements relatifs de souris. Relâcher le grip pour réorienter physiquement la main sans déplacer la vue ; le stick droit reprend son rôle normal. Le stick gauche conserve les déplacements et le grip gauche reste le sélecteur des outils. Le grip droit ne déclenche aucune action du jeu par lui-même.

- Dans `edup-vr-tools.ini`, `[MotionAim]` : `Enabled=1`, `PixelsPerDegree=10` (entier 1 à 30), `InvertY=0` (1 pour inverser le vertical, affecte aussi le stick). Redémarrer après modification. Mettre `Enabled=0` pour conserver le mode stick seul.
- La première pose après l'appui devient la référence : aucun saut vers une orientation absolue. Le module utilise le quaternion OpenXR `aim` dans LOCAL, sa direction -Z, sans tenir compte du roulis. La caméra tourne en réponse aux deltas de lacet et tangage ; une rotation de la tête seule ne génère pas ces deltas si la main reste fixe dans LOCAL.
- Le franchissement de ±180 degrés est géré ; les orientations non finies/non normalisées, proches de la verticale, ou les sauts >25 degrés entre échantillons sont rejetés. La sortie est bornée à 180 pixels par axe et par échantillon, sans accumulation des dépassements. Après une erreur, relâcher les deux manettes au neutre avant de reprendre. Les changements de référence OpenXR effectivement signalés à l'hôte et les erreurs de pose suspendent aussi le mode (F10 nécessaire pour le réactiver).
- La couche outils a priorité et neutralise la visée. Aucun ciblage automatique, aucune compensation de recul. Le module ne modifie ni la position ni l'orientation des objets du jeu.

**Limite essentielle :** il s'agit d'une rotation relative de souris dans le jeu, pas d'un système d'orientation indépendante de l'arme ou de l'outil dans l'espace 3D. L'orientation finale de la vue et la sensibilité dépendent du moteur du jeu, de ses bindings et du runtime VR. Le déplacement des mains peut être représenté par l’overlay expérimental v0.6, sous réserve de validation Windows. Les tests portables vérifient la conversion mathématique et la logique, non la réception des commandes par Elite Dangerous. Si la caméra est mal couplée au casque, désactiver la fonction. Tester assis, dans un espace dégagé, avec une sensibilité faible.

## Compilation de la bibliothèque et tests Windows

Visual Studio 2022 C++, CMake 3.20+, en-têtes OpenXR SDK 1.1.65. Dans PowerShell, depuis la racine du projet :

```powershell
git clone https://github.com/KhronosGroup/OpenXR-SDK.git
cd OpenXR-SDK
git checkout 24a1f2b92d212501a2b1b8d30ffd53abf55fcfa8
cd ..
cmake -S vr-controllers -B build-vr -A x64 -DOPENXR_INCLUDE_DIR="$PWD/OpenXR-SDK/include"
cmake --build build-vr --config Release
ctest --test-dir build-vr -C Release --output-on-failure
```

Le workflow GitHub Actions construit la bibliothèque `.lib` et exécute les tests, pas le patch complet. Pour le test portable sans OpenXR : `cmake -S vr-controllers -B build-vr -DEDUP_POLICY_ONLY=ON` puis `cmake --build build-vr` et `ctest --test-dir build-vr --output-on-failure`.

## Intégration expérimentale à ED-UP (à faire sur une branche d'essai)

1. Créer le fork, cloner le dépôt sur une branche isolée et copier `vr-controllers/` et `appliquer-integration.py` à la racine. Ne pas exécuter sur une branche de production. Le script ne change que `src/openxr/native_runtime_host.h`, `src/openxr/d3d11_stereo.cpp` et `build.bat` ; vérifier le diff.
2. `python appliquer-integration.py . --check` vérifie que toutes les ancres sont uniques. `python appliquer-integration.py .` applique les modifications. Si upstream a changé, le script s'arrête avant d'écrire. Ce n'est pas un outil de fusion automatique.
3. Lire le diff (`git diff`) puis compiler ED-UP selon sa [documentation de compilation](https://github.com/characterecho-sean/ed-unofficial-patch/blob/main/docs/building.md). Utiliser les dépendances épinglées par upstream et conserver ensemble les DLL du même build ; ne pas mélanger les binaires de versions différentes. Le build complet n'a pas été exécuté ici.
4. Par défaut, le module ne s'active pas. Pour un essai contrôlé, définir `EDUP_VR_ONFOOT=1` dans l'environnement du processus du jeu avant lancement (Steam et son lanceur doivent hériter de la variable). Retirer cette variable pour désactiver complètement l'initialisation du module. Le runtime OpenXR Meta ou SteamVR doit être configuré normalement. Ne pas modifier les DLL originales sans sauvegarde ni accord sur les règles Frontier.
5. Sur une scène à pied, avec focus du jeu, relâcher tous les boutons, appuyer F10 pour activer, puis relâcher les contrôleurs au neutre avant d'agir. F10 désactive le profil ; utiliser une liaison F10 non réservée par le jeu. Le mode se désactive en sortie du contexte à pied, perte de focus XR/Windows ou erreur de synchronisation ; une nouvelle activation est alors requise. Ne pas utiliser les mêmes touches simultanément sur un clavier physique.

### Architecture

`InputCore` : actions OpenXR, grip/aim 6DoF, contrôle de validité. `Mapper` : conversion portable, seuils et hystérésis, mouvement relatif de souris, transitions. `WindowsBridge` : SendInput sur les transitions, contrôle du PID de la fenêtre et watchdog 100 ms (vérification toutes les 10 ms). `HostOnFoot` : intégration sur le thread propriétaire existant, pas de seconde session XR. `appliquer-integration.py` : ajoute l'extension Touch Plus si annoncée et demandée, attache l'action set avant début de session, lit le `SeatContext` publié (`gameplay`, `onFootKnown`, `onFoot`, `!cameraOpen`), synchronise après la validation de la pose de tête et libère les handles avant la destruction de la session/instance. Le build de l'hôte ne reçoit le macro `EDUP_ENABLE_ONFOOT` que pour le runtime DLL, pas pour les autres rigs.

La compatibilité de cette lecture du contexte et l'absence d'autres action sets à attacher/synchroniser doivent être reconfirmées avant compilation upstream. Si des action sets sont ajoutés ailleurs, il faut les attacher/synchroniser ensemble (une seule opération attach par session). Le profil Khronos Simple Controller est un repli limité : il ne fournit pas nécessairement sticks et gâchettes analogiques. Le profil Oculus Touch est le repli de Quest 3 si l'extension Touch Plus n'est pas activée.

### Sécurité et limites

Le mapper exige les deux mains suivies et prêtes, un contexte valide, et un passage au neutre après interruption. Le pont Windows n'envoie que les événements correspondant à des appuis physiques. Le watchdog vise un relâchement dans les ~100–110 ms en cas d'arrêt des données (hors délais de scheduling). Une erreur SendInput déclenche une désactivation logique et une tentative de relâchement. Windows SendInput est global : course possible entre contrôle du focus et émission, impossibilité de garantir un relâchement en cas de crash/arrêt forcé, et conflit possible avec une touche physique tenue. Ne jamais tester en situation de jeu à enjeu ; vérifier d'abord avec une scène de test, et réserver les touches mappées. Le module ne cherche pas à contourner les protections Windows ou Frontier. L'acceptation par le jeu et les règles d'utilisation doivent être vérifiées. La branche ne doit pas publier de binaire prétendument validé tant que les essais ne sont pas terminés.

## Publication

`bash publier-github.sh` publie les sources et le script d'intégration sur une nouvelle branche `feature/openxr-onfoot-interactions` du fork `philippe015/ed-unofficial-patch` via GitHub CLI. Il ne modifie pas upstream et s'arrête en cas de conflit. L'intégration est volontairement séparée : le script de publication n'applique pas les modifications à l'hôte. Activer GitHub Actions sur le fork si nécessaire.

## Validation

- CTest Linux Release en mode portable : 7/7 tests réussis (safety, onfoot, odyssey_tools, motion_aim, virtual_hands, stereo_overlay_projection, vr_interactions), incluant retour au neutre, perte de tracking/focus, hystérésis, déplacement/visée, refus de nouvelles pressions après erreur de relâchement.
- Inspection de la branche upstream `main` : 15 ancres hôte, 2 ancres rendu et 1 ancre build trouvées exactement une fois ; compilation hôte non effectuée.
- À faire : Windows MSVC, test du pont SendInput, test OpenXR sur Quest 3 Link/Air Link, réarmement F10, transitions à pied/vaisseau/SRV/menus/caméra, déconnexion de chaque manette et absence de régression du rendu. Puis test de chaque outil, de la recharge sur borne, et du contrôle relatif par la main. La visée réellement indépendante de la caméra reste à développer.

Sources : [hôte ED-UP](https://github.com/characterecho-sean/ed-unofficial-patch/blob/main/src/openxr/native_runtime_host.h), [build ED-UP](https://github.com/characterecho-sean/ed-unofficial-patch/blob/main/build.bat), [OpenXR](https://registry.khronos.org/OpenXR/specs/1.1/html/xrspec.html), [SendInput](https://learn.microsoft.com/windows/win32/api/winuser/nf-winuser-sendinput). Code du module sous MIT, voir LICENSE. Contribution non officielle.
