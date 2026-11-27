# ED-UP VR — Installation complète expérimentale (VR uniquement)

Windows x64, installation locale hors ligne, sans mise à jour automatique du projet d'origine. Fermer Elite Dangerous et son lanceur. Extraire tout le paquet, vérifier SHA256SUMS et exécuter Installer.cmd. Sélectionner le dossier contenant EliteDangerous64.exe (pas le dossier Steam racine). Le programme vérifie chaque fichier, refuse les mods et installations déjà présents et sauvegarde OpenVR dans .edup-vr-preview-backup et openvr_api_orig.dll. Ne pas lancer le jeu pendant l'opération. Droits d'écriture requis : aucun contournement des permissions. Le script utilise ExecutionPolicy Bypass uniquement pour son processus PowerShell, sans modifier la politique système.

Pour activer les commandes à pied, définir EDUP_VR_ONFOOT=1 dans les variables d'environnement utilisateur Windows, puis redémarrer complètement Steam / le lanceur avant de démarrer le jeu. F10 active le mode à pied. Réglages dans edup-vr-tools.ini. Les fonctions expérimentales restent désactivées par défaut. Ne pas utiliser un menu VR invisible. Le runtime OpenXR est celui configuré dans Windows.

## Restaurer ou réinstaller

Fermer le jeu puis exécuter Restaurer.cmd et choisir le même dossier. Les fichiers binaires et les notices modifiés par un autre programme bloquent la restauration afin d'éviter un écrasement. Les réglages modifiés (edup.ini, edup_profile.ini, edup-vr-tools.ini) sont copiés et vérifiés par SHA-256 dans .edup-vr-preview-backup/user-settings avant la restauration. Les fichiers de configuration sauvegardés portent leur empreinte et l'extension .bak ; ils ne sont pas réappliqués automatiquement.

Même après suppression de l'assistant Windows, une copie de Restaurer.cmd et du script de récupération reste dans .edup-vr-preview-backup. Ouvrir ce dossier dans le répertoire du jeu et sélectionner le même répertoire dans l'assistant. Ne jamais supprimer ces sauvegardes avant d'avoir vérifié le jeu. Une seconde restauration terminée ne modifie rien. Lors d'une nouvelle installation, l'ancienne sauvegarde restaurée est archivée automatiquement sous un nom unique ; aucun ancien réglage n'est écrasé.

Pour vérifier le dossier sans modifier les fichiers du jeu, lancer depuis PowerShell :
```powershell
.\install-patch.ps1 -Mode Check
```
Fermer le jeu, sélectionner le répertoire, puis lire le résultat. Retirer EDUP_VR_ONFOOT des variables utilisateur si inutile. Aucun jeu ni compte n'est inclus. Ne pas mélanger avec les DLL d'autres versions.

Build de preuve non signé. Tests sur dossier fictif uniquement ; Quest 3, SteamVR, permissions réelles et lancement d'Odyssey restent à valider. Distribution VR uniquement, EDHM et mode plat non inclus. Le package amont est vérifié avant assemblage ; aucun manifeste de mise à jour amont non signé ni lanceur auto-update n'est distribué.
