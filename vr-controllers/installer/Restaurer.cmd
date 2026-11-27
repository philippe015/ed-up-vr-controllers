@echo off
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0install-patch.ps1" -Mode Restore
if errorlevel 1 echo Restauration interrompue. Lire le message ci-dessus.
pause
