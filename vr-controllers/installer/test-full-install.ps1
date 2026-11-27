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
function InvokePatch($mode,[bool]$success){
 & powershell.exe -NoProfile -ExecutionPolicy Bypass -File $script -Mode $mode -GamePath $fixture
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
 InvokePatch Install $true
 foreach($f in $m.files){if((Get-FileHash (Join-Path $fixture $f.path)).Hash.ToLowerInvariant() -ne $f.sha256){throw "Installed file mismatch: $($f.path)"}}
 if((Get-FileHash (Join-Path $fixture 'Openvr/win64/openvr_api_orig.dll')).Hash -ne $old){throw 'Backup mismatch'}
 InvokePatch Install $false
 $config=Join-Path $fixture 'edup-vr-tools.ini'
 $original=[IO.File]::ReadAllBytes($config)
 Add-Content $config '; fixture user change'
 InvokePatch Restore $false
 if(!(Test-Path $conflict)){throw 'Partial removal after changed file'}
 [IO.File]::WriteAllBytes($config,$original)
 InvokePatch Restore $true
 InvokePatch Restore $true
 if((Get-FileHash $orig).Hash -ne $old){throw 'Original not restored'}
 foreach($f in $m.files){if($f.path -ne 'Openvr/win64/openvr_api.dll' -and (Test-Path (Join-Path $fixture $f.path))){throw "Residual file: $($f.path)"}}
 Write-Host 'Installation, conflicts, modified config and repeatable restoration fixture passed.'
}finally{Remove-Item -LiteralPath $fixture -Recurse -Force}
