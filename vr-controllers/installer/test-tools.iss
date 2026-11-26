#define AppVersion "0.9.9"
[Setup]
AppId=EDUP-VR-TEST-TOOLS-Philou
AppName=ED-UP VR Test Tools
AppVersion={#AppVersion}
AppPublisher=Philou Cyberpunk
DefaultDirName={localappdata}\EDUP-VR-Test-Tools
DefaultGroupName=ED-UP VR Test Tools
DisableProgramGroupPage=yes
DisableDirPage=yes
PrivilegesRequired=lowest
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
OutputDir=output
OutputBaseFilename=ED-UP-VR-Test-Setup-v0.9.9-x64
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
UninstallDisplayIcon={app}\edup_hands_viewer.exe
SetupLogging=yes
CloseApplications=yes
RestartApplications=no
[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "french"; MessagesFile: "compiler:Languages\French.isl"
[CustomMessages]
english.Scope=Diagnostic tools only. This experimental setup does not install or replace game DLLs. No VR gameplay validation is claimed. Python 3.10+ with Tkinter is required for the optional configuration tool.
french.Scope=Outils de diagnostic uniquement. Cet installateur experimental ne remplace aucune DLL du jeu. Les essais VR en jeu restent a valider. Python 3.10+ avec Tkinter est requis pour le panneau de configuration facultatif.
english.DesktopIcon=Create desktop shortcut (viewer demo)
french.DesktopIcon=Creer un raccourci bureau (demo du visualiseur)
english.Viewer=Hands viewer (simulated demo)
french.Viewer=Visualiseur des mains (demo simulee)
english.Config=Control Center (French interface)
french.Config=Panneau de configuration
english.Runtime=Read-only OpenXR diagnostic
french.Runtime=Diagnostic OpenXR en lecture seule
english.Manual=Installation and test guide
french.Manual=Notice d'installation et de test
[Tasks]
Name: "desktopicon"; Description: "{cm:DesktopIcon}"; Flags: unchecked
[Files]
Source: "payload\edup_hands_viewer.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "payload\control_center.py"; DestDir: "{app}"; Flags: ignoreversion
Source: "payload\edup-vr-tools.ini"; DestDir: "{app}"; DestName: "edup-vr-tools.example.ini"; Flags: ignoreversion
Source: "payload\diagnostic-openxr.ps1"; DestDir: "{app}"; Flags: ignoreversion
Source: "payload\open-control-center.cmd"; DestDir: "{app}"; Flags: ignoreversion
Source: "payload\TEST-GUIDE.md"; DestDir: "{app}"; Flags: ignoreversion
Source: "payload\LICENSE.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "payload\build-provenance.txt"; DestDir: "{app}"; Flags: ignoreversion
[Icons]
Name: "{group}\{cm:Viewer}"; Filename: "{app}\edup_hands_viewer.exe"; Parameters: "--demo"; WorkingDir: "{app}"
Name: "{group}\{cm:Config}"; Filename: "{app}\open-control-center.cmd"; WorkingDir: "{app}"
Name: "{group}\{cm:Runtime}"; Filename: "{sys}\WindowsPowerShell\v1.0\powershell.exe"; Parameters: "-NoProfile -NoExit -File ""{app}\diagnostic-openxr.ps1"""; WorkingDir: "{app}"
Name: "{group}\{cm:Manual}"; Filename: "{app}\TEST-GUIDE.md"
Name: "{group}\Uninstall ED-UP VR Test Tools"; Filename: "{uninstallexe}"
Name: "{userdesktop}\ED-UP VR Test Tools"; Filename: "{app}\edup_hands_viewer.exe"; Parameters: "--demo"; Tasks: desktopicon
[Code]
function InitializeSetup(): Boolean;
begin
  Result := True;
  if not WizardSilent then
    Result := MsgBox(CustomMessage('Scope'), mbInformation, MB_OKCANCEL) = IDOK;
end;
