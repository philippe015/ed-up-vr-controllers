param([Parameter(Mandatory=$true)][string]$Package)
$ErrorActionPreference='Stop'
$script=Join-Path $Package 'install-patch.ps1'
$m=Get-Content (Join-Path $Package 'manifest.json') -Raw | ConvertFrom-Json
$fixture=Join-Path $env:RUNNER_TEMP ('edup-fixture-'+[guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path (Join-Path $fixture 'Openvr/win64') -Force | Out-Null
$exe=Join-Path $fixture 'EliteDangerous64.exe'
$orig=Join-Path $fixture 'Openvr/win64/openvr_api.dll'
Set-Content $exe 'fixture, not a real game'
Set-Content $orig 'fixture original runtime'
$old=(Get-FileHash $orig).Hash
function InvokePatch($mode,[bool]$success,[string]$entry=$script){
 & powershell.exe -NoProfile -ExecutionPolicy Bypass -File $entry -Mode $mode -GamePath $fixture
 if(($LASTEXITCODE -eq 0) -ne $success){throw "Unexpected $mode exit $LASTEXITCODE"}
}
try{
 InvokePatch Check $true
 if(Test-Path (Join-Path $fixture '.edup-vr-preview-backup')){throw 'Check writes state'}
 $conflict=Join-Path $fixture 'd3d11.dll'
 Set-Content $conflict 'other mod'
 InvokePatch Install $false
 if((Get-Content $conflict) -ne 'other mod' -or (Get-FileHash $orig).Hash -ne $old){throw 'Conflict modified fixture'}
 Remove-Item $conflict
 # A corrupt payload must be refused before state or game files are written.
 $source=Join-Path $Package 'payload/edup-vr-tools.ini'
 $sourceBytes=[IO.File]::ReadAllBytes($source)
 try{
  Add-Content -LiteralPath $source '; corrupt payload'
  InvokePatch Install $false
  if((Test-Path (Join-Path $fixture '.edup-vr-preview-backup')) -or (Get-FileHash $orig).Hash -ne $old){throw 'Tampered payload modified game'}
 }finally{[IO.File]::WriteAllBytes($source,$sourceBytes)}
 InvokePatch Install $true
 foreach($f in $m.files){if((Get-FileHash (Join-Path $fixture $f.path)).Hash.ToLowerInvariant() -ne $f.sha256){throw "Installed file mismatch: $($f.path)"}}
 if((Get-FileHash (Join-Path $fixture 'Openvr/win64/openvr_api_orig.dll')).Hash -ne $old){throw 'Backup mismatch'}
 InvokePatch Install $false
 $state=Join-Path $fixture '.edup-vr-preview-backup'
 $config=Join-Path $fixture 'edup-vr-tools.ini'
 Add-Content -LiteralPath $config '; fixture user change'
 $customHash=(Get-FileHash $config).Hash.ToLowerInvariant()
 # Changed DLL still blocks restoration before any removal or config archiving.
 $dllBytes=[IO.File]::ReadAllBytes($conflict)
 try{
  Add-Content -LiteralPath $conflict 'foreign modification'
  InvokePatch Restore $false
  if(!(Test-Path $config) -or !(Test-Path (Join-Path $fixture 'Openvr/win64/openvr_api_orig.dll'))){throw 'Partial removal on DLL conflict'}
 }finally{[IO.File]::WriteAllBytes($conflict,$dllBytes)}
 $recovery=Join-Path $state 'install-patch.ps1'
 if(!(Test-Path (Join-Path $state 'Restaurer.cmd'))){throw 'Missing independent recovery shortcut'}
 InvokePatch Restore $true $recovery
 $saved=Join-Path $state ('user-settings/edup-vr-tools.ini.'+$customHash+'.bak')
 if((Get-FileHash $saved).Hash.ToLowerInvariant() -ne $customHash){throw 'Modified settings lost'}
 InvokePatch Restore $true $recovery
 if((Get-FileHash $orig).Hash -ne $old){throw 'Original not restored'}
 foreach($f in $m.files){if($f.path -ne 'Openvr/win64/openvr_api.dll' -and (Test-Path (Join-Path $fixture $f.path))){throw "Residual file: $($f.path)"}}
 InvokePatch Check $true
 InvokePatch Install $true
 $archives=@(Get-ChildItem -LiteralPath $fixture -Directory -Filter '.edup-vr-preview-backup-archive-*' -Force)
 if($archives.Count -ne 1){throw 'Previous backup not archived'}
 $saved=Join-Path $archives[0].FullName ('user-settings/edup-vr-tools.ini.'+$customHash+'.bak')
 if((Get-FileHash $saved).Hash.ToLowerInvariant() -ne $customHash){throw 'Archive lost settings'}
 InvokePatch Restore $true
 if((Get-FileHash $orig).Hash -ne $old){throw 'Reinstall rollback failed'}
 Write-Host 'PASS: preflight, tampering, conflicts, preserved settings, independent recovery, repeat restore and reinstall.'
}finally{Remove-Item -LiteralPath $fixture -Recurse -Force}
