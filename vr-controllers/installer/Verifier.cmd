@echo off
powershell.exe -NoProfile -STA -ExecutionPolicy Bypass -File "%~dp0install-patch.ps1" -Mode Check
set "rc=%errorlevel%"
if "%rc%"=="0" echo Verification reussie. Aucun fichier du jeu modifie.
if "%rc%"=="2" echo Operation annulee.
if not "%rc%"=="0" if not "%rc%"=="2" echo Verification impossible. Lire le motif ci-dessus.
pause
exit /b %rc%
