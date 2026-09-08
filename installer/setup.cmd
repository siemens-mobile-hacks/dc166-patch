@echo off
setlocal EnableExtensions DisableDelayedExpansion
title TASKING C166 v8.6r1 Patched Installer

set "ROOT=%~dp0"

if defined TASKING_PATCH_ONLY goto locate_install

echo Starting the original TASKING C166 v8.6r1 installer...
start "" /wait "%ROOT%original-installer.exe"
if errorlevel 1 (
  echo.
  echo Original installer failed or was cancelled. No files were replaced.
  goto failure
)

:locate_install
set "INSTALL_DIR="
if defined TASKING_INSTALL_DIR set "INSTALL_DIR=%TASKING_INSTALL_DIR%"

if defined INSTALL_DIR goto replace_files
for /f "tokens=2,*" %%A in ('reg query "HKLM\SOFTWARE\TASKING\SW019060\v8.6 r1" /v InstallDir 2^>nul') do if /i "%%A"=="REG_SZ" set "INSTALL_DIR=%%B"
if defined INSTALL_DIR goto replace_files
for /f "tokens=2,*" %%A in ('reg query "HKLM\SOFTWARE\WOW6432Node\TASKING\SW019060\v8.6 r1" /v InstallDir 2^>nul') do if /i "%%A"=="REG_SZ" set "INSTALL_DIR=%%B"

if defined INSTALL_DIR goto replace_files
if not "%ProgramFiles(x86)%"=="" set "INSTALL_DIR=%ProgramFiles(x86)%\TASKING\dc166 v8.6"
if defined INSTALL_DIR if exist "%INSTALL_DIR%\bin\c166.exe" goto replace_files
if defined ProgramFiles set "INSTALL_DIR=%ProgramFiles%\TASKING\dc166 v8.6"

:replace_files
if not defined INSTALL_DIR goto not_found
if not exist "%INSTALL_DIR%\bin\c166.exe" goto not_found
if not exist "%INSTALL_DIR%\bin\l166.exe" goto not_found
if not exist "%INSTALL_DIR%\bin\xfw166.exe" goto not_found
if not exist "%INSTALL_DIR%\bin\disim166.dll" goto not_found

if not exist "%INSTALL_DIR%\bin\c166.exe.pre-patched.bak" copy /b "%INSTALL_DIR%\bin\c166.exe" "%INSTALL_DIR%\bin\c166.exe.pre-patched.bak" >nul
if errorlevel 1 goto copy_failed
if not exist "%INSTALL_DIR%\bin\l166.exe.pre-patched.bak" copy /b "%INSTALL_DIR%\bin\l166.exe" "%INSTALL_DIR%\bin\l166.exe.pre-patched.bak" >nul
if errorlevel 1 goto copy_failed
if not exist "%INSTALL_DIR%\bin\xfw166.exe.pre-patched.bak" copy /b "%INSTALL_DIR%\bin\xfw166.exe" "%INSTALL_DIR%\bin\xfw166.exe.pre-patched.bak" >nul
if errorlevel 1 goto copy_failed
if not exist "%INSTALL_DIR%\bin\disim166.dll.pre-patched.bak" copy /b "%INSTALL_DIR%\bin\disim166.dll" "%INSTALL_DIR%\bin\disim166.dll.pre-patched.bak" >nul
if errorlevel 1 goto copy_failed

copy /b /y "%ROOT%payload\c166-patched.exe" "%INSTALL_DIR%\bin\c166.exe" >nul
if errorlevel 1 goto copy_failed
copy /b /y "%ROOT%payload\l166-patched.exe" "%INSTALL_DIR%\bin\l166.exe" >nul
if errorlevel 1 goto copy_failed
copy /b /y "%ROOT%payload\xfw166-patched.exe" "%INSTALL_DIR%\bin\xfw166.exe" >nul
if errorlevel 1 goto copy_failed
copy /b /y "%ROOT%payload\disim166-patched.dll" "%INSTALL_DIR%\bin\disim166.dll" >nul
if errorlevel 1 goto copy_failed

if not exist "%INSTALL_DIR%\lib\ext" mkdir "%INSTALL_DIR%\lib\ext"
if errorlevel 1 goto copy_failed
if not exist "%INSTALL_DIR%\lib\ext2" mkdir "%INSTALL_DIR%\lib\ext2"
if errorlevel 1 goto copy_failed
for %%V in (ext ext2) do for %%M in (t m l h) do for %%F in (c166 fp166 rt166) do (
  copy /b /y "%ROOT%payload\lib\%%V\%%F%%M.lib" "%INSTALL_DIR%\lib\%%V\%%F%%M.lib" >nul
  if errorlevel 1 goto copy_failed
)
for %%V in (ext ext2) do (
  copy /b /y "%ROOT%payload\lib\%%V\fp166s.lib" "%INSTALL_DIR%\lib\%%V\fp166s.lib" >nul
  if errorlevel 1 goto copy_failed
)

echo.
echo TASKING C166 v8.6r1 patched successfully.
exit /b 0

:not_found
echo.
echo TASKING C166 v8.6r1 installation was not found.
goto failure

:copy_failed
echo.
echo Could not replace TASKING files. Run this installer as Administrator.

:failure
if not defined TASKING_NO_PAUSE pause
exit /b 1
