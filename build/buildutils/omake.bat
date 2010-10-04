@echo off
rem 
rem  Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
rem  All rights reserved.
rem  This component and the accompanying materials are made available
rem  under the terms of "Eclipse Public License v1.0"
rem  which accompanies this distribution, and is available
rem  at the URL "http://www.eclipse.org/legal/epl-v10.html".
rem 
rem  Initial Contributors:
rem  Nokia Corporation - initial contribution.
rem 
rem  Contributors:
rem 
rem  Description: Helper utility for building by developers
rem 

rem -------------------
rem Determine java root
setlocal
set JAVA_SRC_ROOT=

rem See if we are within jrt package (indepent of actual location)
for %%a in (. .. ..\.. ..\..\.. ..\..\..\.. ..\..\..\..\.. ..\..\..\..\..\.. ..\..\..\..\..\..\..) do if "%JAVA_SRC_ROOT%" == "" if exist %%a\build\Makefile.comp call :setroot %%a
if not "%JAVA_SRC_ROOT%" == "" goto resolved

rem See if we are within jrtext package co-located with jrt (indepent of actual location)
for %%a in (. .. ..\.. ..\..\.. ..\..\..\.. ..\..\..\..\.. ..\..\..\..\..\.. ..\..\..\..\..\..\..) do if "%JAVA_SRC_ROOT%" == "" if "%%~nxa" == "jrtext" if exist %%a\build\omj.pri if exist %%a\..\jrt\build\Makefile.comp call :setroot %%a\..\jrt
if not "%JAVA_SRC_ROOT%" == "" goto resolved

rem See if this is subdirectory within \ext\app\jrtext
if "%JAVA_SRC_ROOT%" == "" for /F "delims=\ tokens=2-4" %%a in ("%CD%") do if "%%a\%%b\%%c" == "ext\app\jrtext" set JAVA_SRC_ROOT=\sf\app\jrt
if not "%JAVA_SRC_ROOT%" == "" goto :resolved

echo Could not determine JAVA_SRC_ROOT.
goto :EOF


:resolved
echo JAVA_SRC_ROOT=%JAVA_SRC_ROOT%

rem Make sbs to flush stdout immediately
set PYTHONUNBUFFERED=True

rem -------------------
rem Run with correct makefile 

for %%i in (%CD%) do set BUILDDIR=%%~nxi
if %BUILDDIR% == build (
  if exist *.pro (
    make -f %JAVA_SRC_ROOT%\build\Makefile.comp %*
  ) else (
    make -f %JAVA_SRC_ROOT%\build\Makefile.nonqt %*
  )
) else if exist subsystem.mk (
  make -f subsystem.mk %*
) else if exist Makefile (
  make %*
) else (
  echo Could not determine component type - please check current directory
)

goto :EOF

:setroot
pushd %1
set JAVA_SRC_ROOT=%CD%
popd
