# Read-only discovery, bounded to known launcher locations.
function Find-EdGameFolders {
 param([string[]]$SteamRoots,[string]$EpicManifestRoot,[string[]]$ExtraRoots,[switch]$FixtureOnly)
 $roots=New-Object 'System.Collections.Generic.List[string]'
 $steam=New-Object 'System.Collections.Generic.List[string]'
 foreach($p in $SteamRoots){if($p){$steam.Add($p)}}
 foreach($p in $ExtraRoots){if($p){$roots.Add($p)}}
 if(!$FixtureOnly){
  foreach($key in @('HKCU:\Software\Valve\Steam','HKLM:\SOFTWARE\WOW6432Node\Valve\Steam','HKLM:\SOFTWARE\Valve\Steam')){
   $v=Get-ItemProperty -LiteralPath $key -ErrorAction SilentlyContinue
   if($v){foreach($n in @('SteamPath','InstallPath')){if($v.PSObject.Properties[$n] -and $v.$n){$steam.Add([string]$v.$n)}}}
  }
  $pf=[Environment]::GetEnvironmentVariable('ProgramFiles(x86)')
  if($pf){$steam.Add((Join-Path $pf 'Steam'))}
  foreach($p in @($env:LOCALAPPDATA,$env:ProgramFiles,$pf)){
   if($p){$roots.Add((Join-Path $p 'Frontier Developments/Elite Dangerous'));$roots.Add((Join-Path $p 'Frontier/EDLaunch'))}
  }
  if(!$EpicManifestRoot -and $env:ProgramData){$EpicManifestRoot=Join-Path $env:ProgramData 'Epic/EpicGamesLauncher/Data/Manifests'}
 }
 $libraries=@($steam.ToArray())
 foreach($s in @($steam.ToArray() | Select-Object -Unique -First 32)){
  try{
   $vdf=Join-Path $s 'steamapps/libraryfolders.vdf'
   if((Test-Path -LiteralPath $vdf -PathType Leaf) -and (Get-Item -LiteralPath $vdf).Length -le 1048576){
    $raw=Get-Content -LiteralPath $vdf -Raw
    foreach($m in [regex]::Matches($raw,'"path"\s*"([^"\r\n]+)"') | Select-Object -First 64){$libraries+=($m.Groups[1].Value.Replace('\\','\'))}
   }
  }catch{Write-Verbose "Steam library skipped: $_"}
 }
 foreach($lib in @($libraries | Select-Object -Unique -First 96)){
  try{
   $roots.Add((Join-Path $lib 'steamapps/common/Elite Dangerous'))
   $acf=Join-Path $lib 'steamapps/appmanifest_359320.acf'
   if((Test-Path -LiteralPath $acf -PathType Leaf) -and (Get-Item -LiteralPath $acf).Length -le 1048576){
    $m=[regex]::Match((Get-Content -LiteralPath $acf -Raw),'"installdir"\s*"([^"\\/:]+)"')
    if($m.Success -and $m.Groups[1].Value -notin @('.','..')){$roots.Add((Join-Path $lib ('steamapps/common/'+$m.Groups[1].Value)))}
   }
  }catch{Write-Verbose "Steam app skipped: $_"}
 }
 if($EpicManifestRoot -and (Test-Path -LiteralPath $EpicManifestRoot -PathType Container)){
  foreach($item in @(Get-ChildItem -LiteralPath $EpicManifestRoot -Filter '*.item' -File -ErrorAction SilentlyContinue | Select-Object -First 512)){
   try{
    if($item.Length -gt 1048576){continue}
    $j=Get-Content -LiteralPath $item.FullName -Raw | ConvertFrom-Json
    if($j.PSObject.Properties['DisplayName'] -and $j.PSObject.Properties['InstallLocation'] -and $j.DisplayName -match 'Elite\s*Dangerous' -and $j.InstallLocation){$roots.Add([string]$j.InstallLocation)}
   }catch{Write-Verbose "Epic manifest skipped: $_"}
  }
 }
 $seen=@{}
 foreach($r in @($roots.ToArray() | Select-Object -Unique -First 256)){
  foreach($suffix in @('','Products/elite-dangerous-odyssey-64','Products/elite-dangerous-64','elite-dangerous-odyssey-64','elite-dangerous-64')){
   try{
    if(!$r -or ![IO.Path]::IsPathRooted($r) -or $r.StartsWith('\\')){continue}
    $p=if($suffix){Join-Path $r $suffix}else{$r}
    if(!(Test-Path -LiteralPath (Join-Path $p 'EliteDangerous64.exe') -PathType Leaf)){continue}
    if(!(Test-Path -LiteralPath (Join-Path $p 'Openvr/win64/openvr_api.dll') -PathType Leaf) -and !(Test-Path -LiteralPath (Join-Path $p '.edup-vr-preview-backup/manifest.json') -PathType Leaf)){continue}
    $p=(Get-Item -LiteralPath $p).FullName
    if(!$seen.ContainsKey($p)){$seen[$p]=$true;$p}
   }catch{Write-Verbose "Candidate skipped: $_"}
  }
 }
}

# Explicit selection even if only one candidate is found. Manual selection always available.
function Select-EdGameFolder {
 param([string]$Mode,[string]$RecoveryRoot)
 Add-Type -AssemblyName System.Windows.Forms
 Add-Type -AssemblyName System.Drawing
 $form=New-Object Windows.Forms.Form
 $form.Text='ED-UP VR - Choix du jeu'
 $form.StartPosition='CenterScreen';$form.ClientSize=New-Object Drawing.Size(680,320)
 $form.MinimumSize=New-Object Drawing.Size(696,359)
 $form.AutoScaleMode='Dpi'
 $label=New-Object Windows.Forms.Label
 $label.Text='Choisissez le dossier du jeu contenant EliteDangerous64.exe.'
 $label.SetBounds(12,12,650,26);$label.Anchor='Top,Left,Right';$form.Controls.Add($label)
 $list=New-Object Windows.Forms.ListBox
 $list.SetBounds(12,42,656,166);$list.Anchor='Top,Bottom,Left,Right';$list.HorizontalScrollbar=$true
 $form.Controls.Add($list)
 foreach($p in @(Find-EdGameFolders -ExtraRoots @($RecoveryRoot))){
  [void]$list.Items.Add($p)
 }
 if($list.Items.Count -eq 1){$list.SelectedIndex=0}
 $hint=New-Object Windows.Forms.Label
 $hint.Text='Steam / Epic : detection indicative. Sinon, utiliser Parcourir.'
 if($list.Items.Count -eq 0){$hint.Text='Aucun dossier detecte. Utilisez Parcourir pour le selectionner.'}
 $hint.SetBounds(12,216,650,28);$hint.Anchor='Bottom,Left,Right';$form.Controls.Add($hint)
 $browse=New-Object Windows.Forms.Button
 $browse.Text='Parcourir...';$browse.SetBounds(12,264,136,36);$browse.Anchor='Bottom,Left'
 $browse.Add_Click({
  $dlg=New-Object Windows.Forms.FolderBrowserDialog
  try{
   $dlg.Description='Dossier contenant EliteDangerous64.exe'
   if($list.SelectedItem){$dlg.SelectedPath=[string]$list.SelectedItem}
   if($dlg.ShowDialog($form) -eq 'OK'){
    $index=$list.Items.IndexOf($dlg.SelectedPath)
    if($index -lt 0){$index=$list.Items.Add($dlg.SelectedPath)}
    $list.SelectedIndex=$index
   }
  }finally{$dlg.Dispose()}
 })
 $form.Controls.Add($browse)
 $ok=New-Object Windows.Forms.Button
 $ok.Text='Continuer';$ok.SetBounds(414,264,120,36);$ok.Anchor='Bottom,Right';$ok.Enabled=$list.SelectedIndex -ge 0
 $ok.DialogResult='OK';$form.AcceptButton=$ok;$form.Controls.Add($ok)
 $list.Add_SelectedIndexChanged({$ok.Enabled=$list.SelectedIndex -ge 0})
 $cancel=New-Object Windows.Forms.Button
 $cancel.Text='Annuler';$cancel.SetBounds(548,264,120,36);$cancel.Anchor='Bottom,Right'
 $cancel.DialogResult='Cancel';$form.CancelButton=$cancel;$form.Controls.Add($cancel)
 try{
  if($form.ShowDialog() -ne 'OK' -or !$list.SelectedItem){return}
  $selected=[string]$list.SelectedItem
  if($Mode -ne 'Check'){
   $action=if($Mode -eq 'Restore'){'Restaurer le jeu'}else{'Installer le patch VR experimental'}
   $answer=[Windows.Forms.MessageBox]::Show("$action ?\n\n$selected\n\nFermez Elite Dangerous et son lanceur.".Replace('\n',[Environment]::NewLine),'ED-UP VR','OKCancel','Warning')
   if($answer -ne 'OK'){return}
  }
  return $selected
 }finally{$form.Dispose()}
}
