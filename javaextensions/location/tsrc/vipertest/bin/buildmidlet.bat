@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description: 
@rem

@if not defined S60ROOT (
 @echo Error: S60ROOT not set
 @goto end
)

@if "%1"=="" goto usage

@setlocal
@set J2MECLASSPATH=%EPOCROOT%epoc32\release\winscw\udeb\hmidps60v30.jar
@set PREVERIFYPATH=%S60ROOT%s60\j2me\vms\cldchi\build\share\bin\win32_i386
@set INSTALLFOLDER=%EPOCROOT%epoc32\winscw\c\data\installs

@pushd .
@cd ..\src
%JAVA_1_3_1_HOME%\bin\javac -bootclasspath %J2MECLASSPATH% *.java

@if %ERRORLEVEL% NEQ 0 goto error

%PREVERIFYPATH%\preverify -classpath %J2MECLASSPATH% -d ..\bin .

@if %ERRORLEVEL% NEQ 0 goto error

@echo off
del *.class
popd
if EXIST "%1.png" set PNGFILE="%1.png"
jar cmf MANIFEST.MF %1.jar *.class %PNGFILE%

UpdateJarSize.pl %1.jar %1.jad > %INSTALLFOLDER%\%1.jad
move %1.jar %INSTALLFOLDER%
del *.class
goto end

:usage
@echo Usage: buildmidlet midletname
@goto end

:error
@popd

:end
@echo on