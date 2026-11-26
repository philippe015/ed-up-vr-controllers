@echo off
cd /d "%~dp0"
py -3 control_center.py
if errorlevel 1 pause
