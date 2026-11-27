@echo off
powershell.exe -NoProfile -STA -ExecutionPolicy Bypass -File "%~dp0install-patch.ps1" -Mode Install
set "rc=%errorlevel%"
if "%rc%"=="2" echo Operation annulee.
if not "%rc%"=="0" if not "%rc%"=="2" echo Installation interrompue. Lire le message ci-dessus.
if "%rc%"=="0" echo Operation terminee avec succes.
pause
exit /b %rc%
