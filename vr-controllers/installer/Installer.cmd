@echo off
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0install-patch.ps1" -Mode Install
if errorlevel 1 echo Installation interrompue. Lire le message ci-dessus.
pause
