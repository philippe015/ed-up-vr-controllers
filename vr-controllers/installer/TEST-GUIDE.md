# ED-UP VR Test Tools v0.9.12 / Outils de test

This diagnostic installer is independent of the full integrated game build. It installs only the viewer, French configuration utility, sample INI and read-only OpenXR diagnostic under the current user's LocalAppData. No administrator rights, game DLL installation, runtime changes or automatic game launch.

FR : installation automatique des outils uniquement. Aucun fichier du jeu n'est remplacé par Setup. Le panneau de configuration peut écrire le fichier INI dans un dossier du jeu uniquement après choix du dossier et confirmation de l'utilisateur ; il conserve une sauvegarde précédente .ini.bak. Ne pas configurer un jeu en cours d'exécution. Le visualiseur --demo utilise des poses simulées, sans casque. Le diagnostic OpenXR ne lance pas de session. Python 3.10+ avec Tkinter et le lanceur py sont nécessaires pour le panneau, pas pour le visualiseur. Si PowerShell bloque les scripts, respecter la politique locale ; ne pas la contourner.

EN: use the Start menu to launch the simulated viewer or the read-only OpenXR diagnostic. The French Control Center requires Python 3.10+ with Tkinter and the py launcher. It writes game configuration only after explicit folder selection and save confirmation, preserving one previous INI backup. Uninstall from Windows Settings; any game configuration saved manually by the Control Center is retained and must be restored separately if necessary. No game DLLs are installed, no physical VR gameplay test is enabled by this setup alone.

Full integrated build and package validation are still required before installing the actual game patch. Do not mix DLLs or install incomplete proof artifacts.

FR: https://github.com/philippe015/ed-up-vr-controllers/blob/main/README.md
EN: https://github.com/philippe015/ed-up-vr-controllers/blob/main/README.en.md
