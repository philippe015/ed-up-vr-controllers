# ED-UP VR Controllers — v0.9.9 Experimental

**By Philou Cyberpunk — Elite Dangerous: Odyssey / Windows 11 x64 / Meta Quest 3 / OpenXR**

**Français** | [English — Full installation and user guide](README.en.md)

## Installation automatique — outils de test Windows

[![Télécharger Windows x64](https://img.shields.io/badge/TÉLÉCHARGER-WINDOWS%20x64-238636?style=for-the-badge&logo=windows)](https://github.com/philippe015/ed-up-vr-controllers/actions/runs/45866178749/artifacts/15139264119)

**[TÉLÉCHARGER L'INSTALLATEUR DE TEST (ZIP)](https://github.com/philippe015/ed-up-vr-controllers/actions/runs/45866178749/artifacts/15139264119)** — connexion GitHub requise, disponible jusqu'au 10 décembre 2026.

[Accéder à la compilation de l'installateur de test](https://github.com/philippe015/ed-up-vr-controllers/actions/workflows/windows-test-installer.yml)

Fichier disponible : `ED-UP-VR-Test-Setup-v0.9.9-x64.exe`. Assistant FR/EN, installation sans administrateur dans LocalAppData, raccourcis, visualiseur simulé, diagnostic OpenXR en lecture seule, panneau de configuration (Python 3.10+ requis) et désinstallation. Le workflow compile les outils, exécute CTest et vérifie installation/désinstallation avant publication de l'artefact. Compilation Windows et tests d'installation/désinstallation réussis le 26 novembre 2026. **Cet installateur ne modifie aucune DLL du jeu et ne permet pas à lui seul de jouer en VR avec les nouvelles commandes.** Le patch complet exige encore validation du build et du conditionnement.

> Prototype non officiel. Aucun installateur complet validé disponible à ce jour. La compilation intégrale et les essais sur casque réel restent à confirmer.

**[ACCÉDER À LA COMPILATION ET AU FUTUR ZIP DE TEST WINDOWS](https://github.com/philippe015/ed-up-vr-controllers/actions/workflows/integrated-windows.yml?query=branch%3Afeature%2Fonfoot-vr-enhanced)**

[Suivi de validation PR #1](https://github.com/philippe015/ed-up-vr-controllers/pull/1) · [Sources v0.9.9](https://github.com/philippe015/ed-up-vr-controllers/tree/feature/onfoot-vr-enhanced) · [Documentation technique](https://github.com/philippe015/ed-up-vr-controllers/blob/feature/onfoot-vr-enhanced/vr-controllers/README.md) · [Signaler un problème](https://github.com/philippe015/ed-up-vr-controllers/issues)

## 1. Présentation et possibilités

Le projet complète [ED Unofficial Patch](https://github.com/characterecho-sean/ed-unofficial-patch) par des commandes de manettes VR pour le mode à pied d'Odyssey. Les fonctions suivantes sont présentes dans les sources expérimentales, mais leur fonctionnement dans le jeu et le casque doit être validé.

| Fonction | Description |
|---|---|
| Déplacements | Stick de la main non dominante : marche numérique dans quatre directions, sprint, accroupissement et saut. |
| Orientation | Stick dominant : mouvement relatif de souris, rotation fluide ou par paliers indicatifs. |
| Visée par mouvement | Grip dominant : rotation relative de la caméra à partir de la pose OpenXR de la manette. |
| Visée à deux mains | Option expérimentale : grip dominant puis grip secondaire ; rotation uniquement, sans tir ni marche. |
| Interactions | Appui physique sur un bouton ; action principale/secondaire via les gâchettes. |
| Outils Odyssey | Sélection manuelle du transfert d'énergie, analyseur de profil, outil spécifique de combinaison, rangement, lampe et changement de mode. |
| Mains virtuelles | Représentation filaire approximative de la position et de l'orientation des manettes ; incrustation stéréoscopique facultative. |
| Rayon de visée | Repère graphique facultatif, pas de collision ni de sélection spatiale. |
| Vibrations | Courtes impulsions lors de certaines commandes transmises à Windows, pas de confirmation du résultat dans le jeu. |
| Profil gaucher | Inversion des rôles des manettes. |
| Configuration | Interface locale en français, codes physiques clavier AZERTY, vérification des doublons et sauvegarde INI. |
| Diagnostic | Visualiseur Windows séparé : suivi des poses, mode de commande et profil gaucher/droitier. |
| Protections | Activation volontaire F10, retour au neutre, contrôle du focus et du suivi, watchdog d'environ 100 ms hors délais d'ordonnancement. |

**Limites :** pas de déplacement analogique progressif, pas de suivi anatomique des doigts, pas d'arme ou outil 3D indépendant de la caméra, pas de manipulation directe des écrans/terminaux, pas de recharge automatique. L'overlay est un diagnostic sans occultation par les objets. Aucun gain FPS mesuré. Le logiciel ne doit pas être utilisé pour contourner les protections du jeu.

## 2. Prérequis

- PC Windows 11 64 bits, Elite Dangerous: Odyssey installé et fonctionnel.
- Meta Quest 3, deux manettes Touch Plus, Quest Link USB ou Air Link opérationnel ; runtime OpenXR actif correspondant à la configuration Meta ou SteamVR utilisée.
- Version intégrée d'ED-UP compilée à partir de la révision upstream épinglée `ba1de96c84fab3a6dde3f915d4a7fc9bb40adff1`. Ne pas mélanger les DLL de versions différentes.
- Python 3.10+ avec Tkinter pour le panneau de configuration ; Git, Visual Studio 2022 (C++), CMake 3.20+ et Python 3.12 pour compiler.
- Vérifier les règles d'utilisation de Frontier avant tout essai. Aucune approbation officielle ou garantie de compatibilité n'est revendiquée.

## 3. Obtenir la version Windows

1. Ouvrir le lien de compilation en haut de cette page et sélectionner le workflow correspondant à la branche `feature/onfoot-vr-enhanced`.
2. Vérifier que les tests portables, le module Windows, le build intégré et le build de référence ont réussi. Consulter aussi la PR #1.
3. Si l'archive est disponible dans **Artifacts**, télécharger `ED-UP-VR-v0.9.9-Windows-x64-test-build`. Un compte GitHub connecté peut être nécessaire. Extraire dans un dossier de test séparé.
4. Vérifier le contenu et les empreintes SHA-256. Depuis PowerShell dans le dossier extrait :

```powershell
Get-Content .\SHA256SUMS | ForEach-Object {
    if ($_ -notmatch '^([0-9a-fA-F]{64})  (.+)$') { throw "Ligne SHA256 invalide" }
    $expected = $Matches[1]; $name = $Matches[2]
    $actual = (Get-FileHash -LiteralPath $name -Algorithm SHA256).Hash
    if ($actual -ne $expected) { throw "Empreinte incorrecte : $name" }
    Write-Host "OK : $name"
}
```

**Attention :** cet artefact contient des résultats de compilation, pas encore un installateur ni nécessairement toutes les dépendances redistribuables. Les sommes vérifient l'intégrité, pas l'authenticité d'une publication signée. Si le workflow échoue ou si les fichiers/empreintes manquent, arrêter ici. Ne pas télécharger l'ancien ZIP v0.9 en pensant obtenir la v0.9.9.

## 4. Installation du futur pack complet — procédure conditionnelle

Ces étapes décrivent le parcours prévu après validation du conditionnement. Ne pas installer l'artefact de compilation incomplet actuellement annoncé.

1. Fermer le jeu, le lanceur et les logiciels VR. Dans Steam : Bibliothèque > Elite Dangerous > Propriétés > Fichiers installés > Parcourir. Repérer le dossier contenant réellement `EliteDangerous64.exe` (le chemin varie selon l'installation).
2. Sauvegarder tous les fichiers qu'un installateur ou une installation manuelle remplacera, ainsi que les réglages du patch et les commandes du jeu. Les bindings personnels se trouvent habituellement dans `%LOCALAPPDATA%\Frontier Developments\Elite Dangerous\Options\Bindings`. Conserver les sauvegardes hors du dossier du jeu et noter les fichiers ajoutés.
3. Une fois le pack complet validé, suivre ses instructions de disposition des fichiers ED-UP (y compris les sous-dossiers OpenVR/OpenXR, dépendances et notices) et conserver ensemble les fichiers issus du même build. Ne pas déplacer toutes les DLL à la racine sans tenir compte de la disposition upstream. Ne pas utiliser le lanceur officiel ED-UP pour installer une archive expérimentale non prévue pour lui.
4. Copier uniquement le fichier de configuration `edup-vr-tools.ini` à côté de `EliteDangerous64.exe` et configurer les commandes comme décrit ci-dessous. Les fonctionnalités VR du module sont désactivées sans activation explicite.
5. Connecter le casque, vérifier le runtime, puis effectuer les essais progressifs de la section 8. Si le jeu ne démarre pas, restaurer immédiatement les fichiers d'origine.

Le plan de conditionnement complet reste à valider : [Windows Release Plan](https://github.com/philippe015/ed-up-vr-controllers/blob/feature/onfoot-vr-enhanced/vr-controllers/WINDOWS-RELEASE-PLAN.md). Aucun chemin de copie définitif des DLL n'est publié tant que le pack n'est pas prêt.

## 5. Configuration en français

Dans le dossier du programme extrait (ou `vr-controllers` depuis les sources), exécuter :

```powershell
py -3 control_center.py
```

Choisir le dossier contenant `EliteDangerous64.exe`, attribuer les touches, vérifier puis enregistrer. Reproduire exactement les liaisons dans Options > Commandes > À pied du jeu. Fermer le jeu avant toute modification et le redémarrer ensuite. Une sauvegarde précédente `.ini.bak` est conservée. Les touches sont des codes physiques Windows, pas des caractères ; les exemples AZERTY ci-dessous ne sont pas les réglages officiels du jeu.

| Section / option INI | Valeur / effet |
|---|---|
| Movement / LeftHanded | 0 droitier, 1 gaucher |
| Movement / LookPixelsPerSecond | 900 par défaut, maximum 3000 |
| Movement / SnapMode | 0 fluide ; 1, 2, 3 : paliers nominaux 15°, 30°, 45° |
| Movement / SnapPixelsPerDegree | 10 par défaut, 1–30 ; calibrer selon sensibilité du jeu |
| Movement / ExtraLookDeadzonePercent | 0–40, zone morte supplémentaire de la visée fluide |
| Movement / ToolLayer | 1 : couche outils activée |
| Movement / StrictTools | 1 : refuser les outils si sticks, gâchettes ou grip dominant non neutres |
| MotionAim / Enabled | 1 : visée par mouvement, 0 : stick seul |
| MotionAim / TwoHandEnabled | 0 par défaut ; 1 : visée deux mains expérimentale |
| MotionAim / PixelsPerDegree | 10 par défaut, 1–30 |
| MotionAim / InvertY | 0 normal, 1 inversé |
| Visual / Hands, Overlay, Laser, Haptics | 0 par défaut, 1 pour activer individuellement |
| Visual / HapticPercent | 25 par défaut, intensité 0–100 |

Pour commencer, choisir MotionAim Enabled=0, TwoHandEnabled=0, SnapMode=0, Hands=0, Overlay=0, Laser=0, Haptics=0. Activer une option à la fois. Le rayon nécessite Hands ou Overlay. Les angles réels des paliers dépendent de la sensibilité du jeu.

## 6. Commandes Meta Quest 3 — profil droitier

| Manette | Commande | Action | Exemple AZERTY |
|---|---|---|---|
| Gauche | Stick | Déplacement | Z S Q D |
| Gauche | Appui stick | Sprint | Maj gauche |
| Gauche | X | Accroupissement | C |
| Gauche | Y | Changer d'outil | A |
| Gauche | Gâchette | Action secondaire | Clic droit |
| Gauche | Grip | Sélection des outils (maintenir) | Modificateur interne |
| Droite | Stick | Orientation | Souris |
| Droite | Appui stick | Recharger l'arme | R |
| Droite | A | Interagir | E |
| Droite | B | Sauter | Espace |
| Droite | Gâchette | Action principale | Clic gauche |
| Droite | Grip | Visée par mouvement (si activée) | Modificateur interne |

### Couche outils : maintenir le grip gauche

Relâcher les autres entrées, puis appuyer sur un bouton seulement.

| Bouton | Fonction | Exemple de touche |
|---|---|---|
| X | Transfert d'énergie (EnergyLink) | Rangée 3 |
| Y | Analyseur de profil | Rangée 4 |
| A | Outil spécifique à la combinaison | Rangée 5 |
| B | Ranger l'équipement | H |
| Appui stick gauche | Lampe | T |
| Appui stick droit | Changer le mode de l'outil | M |

Relâcher le grip et revenir au neutre avant l'utilisation de l'outil. Pour une borne, sélectionner EnergyLink, vérifier manuellement le mode transfert, orienter la vue vers la borne et utiliser la gâchette droite. Le découpeur à arc (Maverick) et l'échantillonneur génétique (Artemis) utilisent l'action outil spécifique. Le module ne détecte pas la borne ni le résultat. Profil gaucher : inverser les rôles, les boutons X/Y et A/B suivent les rôles des mains.

### Visée deux mains

Activer TwoHandEnabled=1 et Enabled=1. Depuis le neutre, maintenir le grip dominant, puis le grip de soutien. Garder les mains séparées d'environ 15 à 85 cm. Seule la rotation relative est transmise ; aucun tir ni déplacement. Relâcher les deux grips et revenir au neutre pour sortir.

## 7. Activer et désactiver le module

Dans PowerShell, avant de lancer le jeu via son lanceur habituel :

```powershell
$env:EDUP_VR_ONFOOT = '1'
```

La variable doit être héritée par le véritable processus du jeu : fermer complètement le lanceur/Steam avant de le relancer depuis le même environnement, si nécessaire. Ne pas lancer directement l'exécutable en contournant le lanceur ou l'authentification. Le fichier INI règle les options visuelles ; les variables historiques EDUP_VR_HANDS, EDUP_VR_OVERLAY, EDUP_VR_LASER et EDUP_VR_HAPTICS, lorsqu'elles valent 0 ou 1, priment dessus.

Une fois à pied, jeu au premier plan et manettes au neutre, appuyer sur **F10** au clavier. F10 désactive aussi le mode. Après une perte de focus ou de contexte, réactiver volontairement si nécessaire. Le mode n'est pas prévu pour piloter un vaisseau ou SRV.

Pour désactiver entièrement, fermer le jeu et retirer la variable dans l'environnement de lancement :

```powershell
Remove-Item Env:EDUP_VR_ONFOOT -ErrorAction SilentlyContinue
```

Si définie au niveau utilisateur/système, la retirer aussi à ce niveau puis redémarrer le lanceur.

### Visualiseur Windows

```powershell
.\edup_hands_viewer.exe --demo
```

Démo simulée, sans casque. Pour les poses réelles, activer Hands=1, lancer la session du jeu, relever son PID dans le gestionnaire des tâches et exécuter :

```powershell
.\edup_hands_viewer.exe 12345
```

Remplacer 12345 par le PID réel. Rendre le focus au jeu, activer F10. Utiliser le visualiseur compatible protocole v3 du même build. Les libellés de mode sont affichés sur le bureau, pas dans un menu au poignet.

## 8. Premier test physique Quest 3

Tester dans un endroit dégagé, sans combat ni enjeu, de préférence assis. Interrompre en cas de malaise ou de mouvement imprévu.

- [ ] Démo du visualiseur sans jeu.
- [ ] Jeu VR normal, module désactivé.
- [ ] F10 à pied, déplacement puis orientation simple.
- [ ] Interactions, saut, accroupissement, rechargement.
- [ ] Perte de suivi, Alt-Tab, déconnexion d'une manette, retour au vaisseau/SRV : vérifier l'arrêt des commandes.
- [ ] Couche outils, recharge manuelle sur borne, changement de mode.
- [ ] Visée une main, puis deux mains ; paliers et profil gaucher.
- [ ] Mains filaires, rayon et vibrations, séparément.
- [ ] Mesurer FPS et stabilité avec/sans module ; vérifier une session de 30 minutes.

Ne pas tenir simultanément les touches physiques mappées. Le watchdog n'est pas une garantie en cas de crash. En cas de blocage, relâcher les manettes, désactiver F10 si possible, fermer le jeu. [Matrice détaillée des tests](https://github.com/philippe015/ed-up-vr-controllers/blob/feature/onfoot-vr-enhanced/vr-controllers/VALIDATION-WINDOWS.md).

## 9. Désinstallation / dépannage

Fermer jeu et lanceur. Retirer EDUP_VR_ONFOOT de leur environnement, restaurer les fichiers ED-UP et réglages sauvegardés ensemble, retirer uniquement les fichiers ajoutés identifiés dans la notice du futur pack. Restaurer les bindings personnels si nécessaire. Ne jamais effacer à l'aveugle une DLL appartenant au jeu. Si le jeu ne fonctionne plus, utiliser la vérification des fichiers du lanceur après sauvegarde des réglages ; elle ne retire pas nécessairement les fichiers tiers ajoutés.

| Symptôme | Vérification |
|---|---|
| Aucune commande | Runtime, deux manettes suivies, contexte à pied, focus, F10, héritage de la variable, bindings |
| Outil incorrect | Codes physiques, doublons et liaisons en jeu ; retour au neutre |
| Caméra instable | Désactiver MotionAim / TwoHandEnabled / SnapMode ; diminuer sensibilité |
| Visualiseur vide | Hands=1, PID correct, session XR ouverte, jeu au premier plan, version du visualiseur |
| Aucune vibration | Haptics=1, intensité, liaisons actives et runtime compatible |

## 10. Compiler soi-même (développeurs)

Cloner les sources expérimentales, utiliser une copie isolée de l'upstream épinglé, appliquer le script après vérification des ancres puis contrôler le diff. Ne pas utiliser de branche de production.

```powershell
git clone -b feature/onfoot-vr-enhanced https://github.com/philippe015/ed-up-vr-controllers.git contribution
git clone https://github.com/characterecho-sean/ed-unofficial-patch.git upstream
git -C upstream checkout --detach ba1de96c84fab3a6dde3f915d4a7fc9bb40adff1
python contribution/appliquer-integration.py upstream --check
# Stopper si la commande précédente échoue.
Copy-Item contribution/vr-controllers upstream/vr-controllers -Recurse
python contribution/appliquer-integration.py upstream
# Stopper si la commande précédente échoue.
git -C upstream diff --check
git -C upstream diff
```

Compiler d'abord le module et exécuter CTest, puis installer les dépendances hash-pinnées et SDKs selon la [documentation upstream à la révision épinglée](https://github.com/characterecho-sean/ed-unofficial-patch/blob/ba1de96c84fab3a6dde3f915d4a7fc9bb40adff1/docs/building.md).

```powershell
cmake -S contribution/vr-controllers -B build-vr -A x64 "-DOPENXR_INCLUDE_DIR=$PWD/upstream/third_party/openxr/include"
cmake --build build-vr --config Release
ctest --test-dir build-vr -C Release --output-on-failure
# Ne poursuivre que si les tests réussissent et les SDKs sont vérifiés.
Set-Location upstream
cmd /c "build.bat --wait-lock --jobs 4"
# Après succès uniquement :
.\build\smoke.exe .\build\d3d11.dll
```

Le build complet et le test smoke ne remplacent pas les essais sur casque. Ne pas contourner les contrôles du packager upstream (version exacte et reçu de compilation). Le workflow de preuve compare upstream original et intégré ; un précédent test CPU a échoué, cause non confirmée.

## 11. Rapports de test et contribution

Indiquer version du jeu, commit, runtime OpenXR, Link/Air Link, GPU, étapes de reproduction, résultats, FPS et logs utiles. Masquer les données personnelles et chemins privés. Aucun envoi automatique. [Ouvrir un ticket](https://github.com/philippe015/ed-up-vr-controllers/issues/new). [Proposition upstream #272](https://github.com/characterecho-sean/ed-unofficial-patch/issues/272).

## 12. Historique et licence

v0.9.3 visée deux mains ; v0.9.4 gaucher/droitier ; v0.9.5 confort ; v0.9.6 outils ; v0.9.7 diagnostic ; v0.9.8 stabilité ; v0.9.9 validation Windows. v1.0 expérimentale prévue uniquement après les contrôles.

[Ancienne archive de sources v0.9 (pas un installateur)](https://github.com/user-attachments/files/35450529/ED-UP-VR-Controllers-v0.9-Stability.zip). [Licence MIT](LICENSE). Les composants tiers conservent leurs licences respectives.

## English overview

Experimental community contribution for on-foot VR controls in Elite Dangerous: Odyssey, Windows x64 and Quest 3. See the French installation and usage manual above. The build link opens Actions, not a verified installer. Motion aiming uses relative mouse input, tools remain manually controlled, and physical headset testing is pending. Do not install incomplete proof artifacts or mix DLLs from different builds. Feedback is welcome through GitHub Issues.
