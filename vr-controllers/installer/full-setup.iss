[Setup]
AppId=EDUP-VR-Preview-Full-Philou
AppName=ED-UP VR Experimental Patch Installer
AppVersion=0.9.18-preview
AppPublisher=Philou Cyberpunk
DefaultDirName={localappdata}\EDUP-VR-Preview-Full
DefaultGroupName=ED-UP VR Experimental Patch
PrivilegesRequired=lowest
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
OutputDir=output
OutputBaseFilename=ED-UP-VR-Full-Setup-v0.9.18-preview-x64
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
DisableDirPage=yes
CloseApplications=no
RestartApplications=no
[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "french"; MessagesFile: "compiler:Languages\French.isl"
[CustomMessages]
english.Scope=Experimental VR-only patch. Setup extracts the offline installer. The separate game installation step asks for your game directory. Existing patches are refused. Restore the game BEFORE uninstalling these tools. No real headset validation; no automatic updates.
french.Scope=Patch VR experimental uniquement. Ce programme extrait les outils hors ligne. Une seconde etape demande le dossier du jeu. Les patchs existants sont refuses. Restaurer le jeu AVANT de desinstaller ces outils. Aucun test sur casque ni mise a jour automatique.
english.RunPatch=Choose the Elite Dangerous directory and install the experimental patch
french.RunPatch=Choisir le dossier Elite Dangerous et installer le patch experimental
english.Restore=Restore game files (run before uninstalling tools)
french.Restore=Restaurer le jeu (avant de desinstaller les outils)
[Files]
Source: "full-payload\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
[Icons]
Name: "{group}\Installer le patch"; Filename: "{app}\Installer.cmd"; WorkingDir: "{app}"
Name: "{group}\{cm:Restore}"; Filename: "{app}\Restaurer.cmd"; WorkingDir: "{app}"
Name: "{group}\Notice"; Filename: "{app}\FULL-INSTALL-GUIDE.md"
[Run]
Filename: "{app}\Installer.cmd"; Description: "{cm:RunPatch}"; Flags: postinstall shellexec skipifsilent unchecked
[Code]
function InitializeSetup(): Boolean;
begin
  Result := True;
  if not WizardSilent then
    Result := MsgBox(CustomMessage('Scope'), mbInformation, MB_OKCANCEL) = IDOK;
end;
