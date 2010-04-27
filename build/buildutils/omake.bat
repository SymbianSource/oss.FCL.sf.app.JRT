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
for %%a in (. .. ..\.. ..\..\.. ..\..\..\.. ..\..\..\..\.. ..\..\..\..\..\.. ..\..\..\..\..\..\..) do if "%JAVA_SRC_ROOT%" == "" if exist %%a\build\Makefile.comp call :setroot %%a

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
