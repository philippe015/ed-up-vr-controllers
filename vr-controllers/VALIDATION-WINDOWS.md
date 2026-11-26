# Validation v0.9 — Windows 11 / Quest 3

Prototype non officiel. Ne pas installer un mélange de DLL. Sauvegarder les fichiers et paramètres originaux. Vérifier les règles Frontier avant essai. Aucun binaire Windows validé n'est fourni.

## 1. Préflight
- Quest Link ou Air Link connecté, runtime OpenXR Meta ou SteamVR choisi dans son logiciel. Exécuter `diagnostic-openxr.bat` (ou `powershell -NoProfile -File diagnostic-openxr.ps1 -GameFolder "C:\\..."`). Vérifier manifest et DLL du runtime. Le diagnostic ne lance aucune session XR.
- Compiler le module avec Visual Studio 2022 C++, CMake et les en-têtes OpenXR SDK épinglés (README). Exécuter CTest. Le workflow GitHub ne compile que le module et le visualiseur.
- Sur une branche de test ED-UP, appliquer le script d'intégration après `--check`, inspecter le diff puis compiler le patch complet selon docs/building.md upstream. Conserver ensemble les DLL du même build.
- Régler les touches avec le Control Center, fermer le jeu avant modification. Lancer le jeu avec EDUP_VR_ONFOOT=1, initialement sans overlay, laser, haptique ni visée par mouvement. F10 active le profil après retour au neutre.

## 2. Matrice d'essai physique (non exécutée)
| Test | Résultat attendu | Résultat réel |
|---|---|---|
| Jeu sans variable EDUP_VR_ONFOOT | Aucune commande injectée | Non testé |
| F10 à pied et deux sticks neutres | Activation volontaire | Non testé |
| Sticks gauche/droit | Déplacement/orientation sans répétitions anormales | Non testé |
| Perte de suivi d'une main | Relâchement, retour au neutre avant reprise | Non testé |
| Alt-Tab ou perte de focus XR | Relâchement, réactivation volontaire | Non testé |
| Déconnexion/reconnexion | Aucun appui spontané | Non testé |
| Retour au vaisseau/SRV/caméra | Profil bloqué | Non testé |
| Grip gauche + X / EnergyLink | Sélection et utilisation manuelle sur borne | Non testé |
| Grip droit et visée | Mouvement relatif de souris, pas visée 3D indépendante | Non testé |
| Overlay/laser activés séparément | Affichage dans chaque œil, disparition hors contexte | Non testé |
| Vibrations activées | Courtes impulsions, aucun blocage | Non testé |
| Session de 30 min | Pas de touche coincée, aucun crash, FPS mesurés | Non testé |

En cas de blocage : F10 si le jeu répond, relâcher les contrôleurs, revenir au clavier, fermer le jeu si nécessaire. Retirer la variable d'activation avant de relancer. Le watchdog est une protection en fonctionnement normal, pas une garantie en cas de crash ou de terminaison forcée. Les touches injectées par SendInput sont globales et peuvent entrer en conflit avec des touches physiques.

## 3. Rapport
Noter runtime et version du patch/jeu, mode Link, FPS avec et sans overlay, scénario précis, logs ED-UP, résultat de chaque test. Ne publier aucun identifiant personnel ni chemin privé sans le vérifier. Aucun envoi automatique.
