param([Parameter(Mandatory=$true)][string]$Package)
$ErrorActionPreference='Stop'
Set-StrictMode -Version Latest
. (Join-Path $Package 'find-game.ps1')
$root=Join-Path $env:RUNNER_TEMP ('edup-detection-'+[guid]::NewGuid().ToString('N'))
try{
 $steam=Join-Path $root 'Steam';$library=Join-Path $root 'Other Library';$epic=Join-Path $root 'Epic';$manifests=Join-Path $root 'Manifests'
 $a=Join-Path $library 'steamapps/common/Custom Elite/Products/elite-dangerous-odyssey-64'
 $b=Join-Path $epic 'Products/elite-dangerous-odyssey-64'
 foreach($p in @($a,$b)){
  New-Item -ItemType Directory -Path (Join-Path $p 'Openvr/win64') -Force | Out-Null
  Set-Content -LiteralPath (Join-Path $p 'EliteDangerous64.exe') -Value 'fixture only'
  Set-Content -LiteralPath (Join-Path $p 'Openvr/win64/openvr_api.dll') -Value 'fixture only'
 }
 New-Item -ItemType Directory -Path (Join-Path $steam 'steamapps'),$manifests -Force | Out-Null
 $escaped=$library.Replace('\','\\')
 Set-Content -LiteralPath (Join-Path $steam 'steamapps/libraryfolders.vdf') -Value ('"libraryfolders" { "1" { "path" "'+$escaped+'" } }')
 Set-Content -LiteralPath (Join-Path $library 'steamapps/appmanifest_359320.acf') -Value '"AppState" { "installdir" "Custom Elite" }'
 @{DisplayName='Elite Dangerous';InstallLocation=$epic} | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $manifests 'elite.item')
 @{DisplayName='Another game';InstallLocation=$a} | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $manifests 'other.item')
 Set-Content -LiteralPath (Join-Path $manifests 'invalid.item') -Value '{ malformed'
 $found=@(Find-EdGameFolders -SteamRoots @($steam) -EpicManifestRoot $manifests -ExtraRoots @($a,$b,$root) -FixtureOnly)
 $expected=@((Get-Item -LiteralPath $a).FullName,(Get-Item -LiteralPath $b).FullName)
 if($found.Count -ne 2 -or (Compare-Object $found $expected)){throw "Unexpected detection: $($found -join '; ')"}
 # Existing recovery must still be discoverable if the original DLL is missing.
 Remove-Item -LiteralPath (Join-Path $b 'Openvr/win64/openvr_api.dll')
 $missing=@(Find-EdGameFolders -ExtraRoots @($b) -FixtureOnly)
 if($missing.Count -ne 0){throw 'Incomplete directory accepted'}
 New-Item -ItemType Directory -Path (Join-Path $b '.edup-vr-preview-backup') | Out-Null
 Set-Content -LiteralPath (Join-Path $b '.edup-vr-preview-backup/manifest.json') -Value '{}'
 $recover=@(Find-EdGameFolders -ExtraRoots @($b) -FixtureOnly)
 if($recover.Count -ne 1){throw 'Recovery folder not found'}
 Write-Host 'PASS: Steam custom library, Epic manifest, invalid metadata, deduplication, recovery folder.'
}finally{if(Test-Path -LiteralPath $root){Remove-Item -LiteralPath $root -Recurse -Force}}
