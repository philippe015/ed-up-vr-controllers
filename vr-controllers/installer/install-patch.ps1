param([ValidateSet('Install','Restore','Check')][string]$Mode='Install',[string]$GamePath)
$ErrorActionPreference='Stop'
Set-StrictMode -Version Latest
$orig='Openvr/win64/openvr_api.dll'
$alias='Openvr/win64/openvr_api_orig.dll'
$stateName='.edup-vr-preview-backup'
$allowed=@('d3d11.dll',$orig,'Openvr/win64/openxr_loader.dll','Openvr/win64/OPENXR-LOADER-LICENSE.txt','edup.ini','edup_profile.ini','DITTOSR-LICENSE.txt','DITTOSR-GSR-LICENSE.txt','libxess.dll','INTEL-XESS-LICENSE.txt','INTEL-XESS-THIRD-PARTY.txt','amd_fidelityfx_loader_dx12.dll','amd_fidelityfx_upscaler_dx12.dll','AMD-FIDELITYFX-LICENSE.txt','nvngx_dlss.dll','NVIDIA-DLSS-LICENSE.txt','FIDELITYFX-SDK-DX11-LICENSE.txt','edup-vr-tools.ini','EDUP-VR-Notices/LICENSE.txt','EDUP-VR-Notices/DXBC-HASH-LICENSE.txt','EDUP-VR-Notices/MINIZ-LICENSE.txt','EDUP-VR-Notices/MIN-ED-LAUNCHER-LICENSE.txt','EDUP-VR-Notices/VR-LICENSE.txt')
function Hash($p){(Get-FileHash -LiteralPath $p -Algorithm SHA256).Hash.ToLowerInvariant()}
function SafePath([string]$base,[string]$rel){
 $p=Join-Path $base $rel
 $cur=$p
 while($cur){
  if(Test-Path -LiteralPath $cur){
   $item=Get-Item -LiteralPath $cur -Force
   if($item.Attributes -band [IO.FileAttributes]::ReparsePoint){throw "Lien/reparse point refuse : $cur"}
  }
  $parent=Split-Path -Parent $cur
  if(!$parent -or $parent -eq $cur){break};$cur=$parent
 }
 return $p
}
function ReadManifest($path){
 $m=Get-Content -LiteralPath $path -Raw | ConvertFrom-Json
 if($m.schema -ne 1){throw 'Manifest schema'}
 $seen=@{}
 foreach($f in $m.files){
  if($f.path -cnotin $allowed -or $seen.ContainsKey($f.path) -or $f.sha256 -cnotmatch '^[0-9a-f]{64}$'){throw 'Manifest invalide'}
  $seen[$f.path]=$true
 }
 foreach($p in $allowed){if($p -notin @('nvngx_dlss.dll','NVIDIA-DLSS-LICENSE.txt','FIDELITYFX-SDK-DX11-LICENSE.txt') -and !$seen.ContainsKey($p)){throw "Fichier requis absent : $p"}}
 if($seen.ContainsKey('nvngx_dlss.dll') -ne $seen.ContainsKey('NVIDIA-DLSS-LICENSE.txt')){throw 'DLSS incomplet'}
 return $m
}
function GuardGame {
 if(Get-Process -Name EliteDangerous64 -ErrorAction SilentlyContinue){throw 'Fermer Elite Dangerous avant toute modification.'}
}
function PutFile($src,$dst,[bool]$replace){
 $parent=Split-Path -Parent $dst
 if(!(Test-Path -LiteralPath $parent)){New-Item -ItemType Directory -Path $parent | Out-Null}
 $tmp=Join-Path $parent ('.edup-tmp-'+[guid]::NewGuid().ToString('N'))
 try{
  [IO.File]::Copy($src,$tmp,$false)
  if((Hash $src) -ne (Hash $tmp)){throw 'Copie corrompue'}
  if($replace){[IO.File]::Replace($tmp,$dst,[NullString]::Value)}else{[IO.File]::Move($tmp,$dst)}
 }finally{if(Test-Path -LiteralPath $tmp){Remove-Item -LiteralPath $tmp -Force}}
}
function RestorePatch {
 $m=ReadManifest (SafePath $state 'manifest.json')
 $backup=SafePath $state 'original.dll'
 $old=(Get-Content -LiteralPath (SafePath $state 'original.sha256') -Raw).Trim()
 if($old -cnotmatch '^[0-9a-f]{64}$' -or (Hash $backup) -ne $old){throw 'Sauvegarde originale invalide; aucun fichier modifie.'}
 # Preflight ALL files before removing any; user changes are never deleted silently.
 foreach($f in $m.files){
  $dst=SafePath $root $f.path
  if(Test-Path -LiteralPath $dst){
   $actual=Hash $dst
   if($actual -ne $f.sha256 -and !($f.path -eq $orig -and $actual -eq $old)){throw "Fichier modifie : $dst. Sauvegarder vos changements manuellement."}
  }
 }
 $copy=SafePath $root $alias
 if((Test-Path -LiteralPath $copy) -and (Hash $copy) -ne $old){throw 'Sauvegarde OpenVR externe modifiee'}
 GuardGame
 $runtime=SafePath $root $orig
 if(!(Test-Path -LiteralPath $runtime)){PutFile $backup $runtime $false}
 elseif((Hash $runtime) -ne $old){PutFile $backup $runtime $true}
 foreach($f in $m.files){
  if($f.path -eq $orig){continue}
  $dst=SafePath $root $f.path
  if(Test-Path -LiteralPath $dst){
   if((Hash $dst) -ne $f.sha256){throw "Fichier change pendant restauration : $dst"}
   Remove-Item -LiteralPath $dst -Force
  }
 }
 if(Test-Path -LiteralPath $copy){if((Hash $copy) -ne $old){throw 'Sauvegarde OpenVR changee'};Remove-Item -LiteralPath $copy -Force}
 if((Hash $runtime) -ne $old){throw 'Verification restauration impossible'}
 # Keep original backup and journal for manual recovery; explicit completion marker.
 Set-Content -LiteralPath (Join-Path $state 'restored.txt') -Value 'Original runtime restored' -Encoding ascii
 Write-Host "Restauration terminee. Sauvegarde conservee : $state"
}
if(!$GamePath){Add-Type -AssemblyName System.Windows.Forms;$dialog=New-Object System.Windows.Forms.FolderBrowserDialog;$dialog.Description='Selectionner le dossier contenant EliteDangerous64.exe';if($dialog.ShowDialog() -ne 'OK'){exit 2};$GamePath=$dialog.SelectedPath}
$root=(Get-Item -LiteralPath $GamePath).FullName
if(!(Test-Path -LiteralPath (SafePath $root 'EliteDangerous64.exe') -PathType Leaf)){throw 'Dossier EliteDangerous64.exe incorrect'}
if($root.StartsWith('\\')){throw 'Dossier reseau non accepte'}
GuardGame
$state=SafePath $root $stateName
# Exclusive lock using a named mutex for this path; only cooperative installers are serialized.
$bytes=[Text.Encoding]::UTF8.GetBytes($root.ToLowerInvariant())
$alg=[Security.Cryptography.SHA256]::Create();try{$key=([BitConverter]::ToString($alg.ComputeHash($bytes))).Replace('-','')}finally{$alg.Dispose()}
$mutex=New-Object Threading.Mutex($false,('Local\EDUPPreviewInstall_'+$key))
$locked=$false
try{
 try{$locked=$mutex.WaitOne(0)}catch [Threading.AbandonedMutexException]{$locked=$true}
 if(!$locked){throw 'Une autre operation est en cours'}
 if($Mode -eq 'Restore'){if(!(Test-Path -LiteralPath $state -PathType Container)){throw 'Aucune sauvegarde du patch experimental'};RestorePatch;return}
 if(Test-Path -LiteralPath $state){throw "Sauvegarde precedente presente : $state. Restaurer puis archiver ce dossier manuellement avant reinstallation."}
 $m=ReadManifest (Join-Path $PSScriptRoot 'manifest.json')
 $payload=Join-Path $PSScriptRoot 'payload'
 foreach($f in $m.files){
  $src=SafePath $payload $f.path
  if((Hash $src) -ne $f.sha256){throw "SHA256 invalide : $src"}
  $dst=SafePath $root $f.path
  if($f.path -ne $orig -and (Test-Path -LiteralPath $dst)){throw "Installation existante detectee : $dst. Aucun ecrasement."}
 }
 foreach($p in @($alias,'edvr.ini','edvr-flat.ini','edup-flat.ini','d3d11_edhm.dll','EDUP-VR-Notices')){
  if(Test-Path -LiteralPath (SafePath $root $p)){throw "Patch/mod existant : $p"}
 }
 $runtime=SafePath $root $orig
 if(!(Test-Path -LiteralPath $runtime -PathType Leaf)){throw 'OpenVR original absent'}
 $old=Hash $runtime
 if($old -eq ($m.files | Where-Object path -eq $orig).sha256){throw 'Le runtime est deja celui du patch'}
 if($Mode -eq 'Check'){Write-Host 'Preflight OK; aucun fichier modifie.';return}
 GuardGame
 New-Item -ItemType Directory -Path $state | Out-Null
 [IO.File]::Copy($runtime,(Join-Path $state 'original.dll'),$false)
 if((Hash (Join-Path $state 'original.dll')) -ne $old){throw 'Sauvegarde corrompue'}
 Set-Content -LiteralPath (Join-Path $state 'original.sha256') -Value $old -Encoding ascii
 [IO.File]::Copy((Join-Path $PSScriptRoot 'manifest.json'),(Join-Path $state 'manifest.json'),$false)
 try{
  PutFile (Join-Path $state 'original.dll') (SafePath $root $alias) $false
  foreach($f in $m.files){
   GuardGame
   $dst=SafePath $root $f.path
   if($f.path -eq $orig -and (Hash $dst) -ne $old){throw 'Runtime original change pendant installation'}
   PutFile (SafePath $payload $f.path) $dst ($f.path -eq $orig)
   if((Hash $dst) -ne $f.sha256){throw 'Verification apres copie echouee'}
  }
  Write-Host "Installation experimentale terminee. Sauvegarde : $state"
 }catch{
  $failure=$_
  try{RestorePatch}catch{Write-Warning "Restauration automatique incomplete : $_. Conserver $state."}
  throw $failure
 }
}finally{if($locked){$mutex.ReleaseMutex()};$mutex.Dispose()}
