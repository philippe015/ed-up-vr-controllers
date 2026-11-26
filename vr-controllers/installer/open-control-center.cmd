@echo off
setlocal
cd /d "%~dp0"
py -3 -c "import sys, tkinter; raise SystemExit(0 if sys.version_info >= (3,10) else 1)" >nul 2>&1
if errorlevel 1 (
  echo Python 3.10+ with Tkinter and the Python launcher ^(py^) are required.
  echo Python 3.10+ avec Tkinter et le lanceur Python ^(py^) sont requis.
  echo https://www.python.org/downloads/windows/
  pause
  exit /b 1
)
py -3 "%~dp0control_center.py"
if errorlevel 1 pause
