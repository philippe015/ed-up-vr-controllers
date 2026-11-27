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
