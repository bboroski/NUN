@echo off
setlocal
py -3 "%~dp0one.py" %*
exit /b %ERRORLEVEL%
