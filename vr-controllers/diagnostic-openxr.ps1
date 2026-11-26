param([string]$GameFolder = '')
$ErrorActionPreference = 'Stop'
$report = [ordered]@{ Version='0.9.0'; Date=(Get-Date).ToString('o'); Windows=[Environment]::OSVersion.VersionString; OS64=[Environment]::Is64BitOperatingSystem; Process64=[Environment]::Is64BitProcess; Runtime=$null; Source=$null; ManifestExists=$false; ManifestValid=$false; RuntimeLibraryExists=$false; GameExe=$null; Config=$null; Notes=@() }
try {
 if ($env:XR_RUNTIME_JSON) { $report.Runtime=$env:XR_RUNTIME_JSON; $report.Source='XR_RUNTIME_JSON (override)' }
 else {
  $base=[Microsoft.Win32.RegistryKey]::OpenBaseKey([Microsoft.Win32.RegistryHive]::LocalMachine,[Microsoft.Win32.RegistryView]::Registry64)
  try { $key=$base.OpenSubKey('SOFTWARE\Khronos\OpenXR\1'); if($key){try {$report.Runtime=$key.GetValue('ActiveRuntime');$report.Source='HKLM 64-bit'}finally{$key.Dispose()}} } finally {$base.Dispose()}
 }
 if ($report.Runtime -and (Test-Path -LiteralPath $report.Runtime -PathType Leaf)) {
  $report.ManifestExists=$true
  $manifest=Get-Content -LiteralPath $report.Runtime -Raw | ConvertFrom-Json
  $lib=$manifest.runtime.library_path
  if ($lib -and $manifest.file_format_version) {
   $report.ManifestValid=$true
   if(-not [IO.Path]::IsPathRooted($lib)){$lib=Join-Path (Split-Path -Parent $report.Runtime) $lib}
   $report.RuntimeLibraryExists=Test-Path -LiteralPath $lib -PathType Leaf
  }
 }
} catch { $report.Notes+=('Erreur de lecture du runtime : '+$_.Exception.Message) }
if ($GameFolder) { $report.GameExe=Test-Path -LiteralPath (Join-Path $GameFolder 'EliteDangerous64.exe') -PathType Leaf; $report.Config=Test-Path -LiteralPath (Join-Path $GameFolder 'edup-vr-tools.ini') -PathType Leaf }
$report.Notes+='Diagnostic statique uniquement : ne lance pas OpenXR, ne confirme pas la connexion du casque ni le fonctionnement des controleurs.'
$report | ConvertTo-Json -Depth 4
