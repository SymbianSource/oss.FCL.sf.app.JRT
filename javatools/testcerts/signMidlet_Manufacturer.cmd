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

@ECHO OFF
set JDK=C:\j2sdk1.4.2_09\
set WTK=C:\WTK23\
set JKEY_STORE=.\
set JAD_NAME=%1
set JAR_NAME=%2

if "%1"=="" goto :HELP
if "%2"=="" goto :HELP
if not exist %JDK%bin\java.exe goto :JDK_HELP
if not exist %WTK%bin\JADTool.jar goto :WTK_HELP
if not exist %JKEY_STORE% goto :JKEY_STORE_HELP

@ECHO ON
%JDK%bin\java.exe -jar %WTK%bin\JADTool.jar -addcert -keystore %JKEY_STORE%test_keystore.ks -alias JP_Device_Manufacturer -storepass jperformance -certnum 1 -chainnum 1 -inputjad %JAD_NAME%  -outputjad %JAD_NAME%
%JDK%bin\java.exe -jar %WTK%bin\JADTool.jar -addjarsig -jarfile %JAR_NAME% -keystore %JKEY_STORE%test_keystore.ks -alias JP_Device_Manufacturer -storepass jperformance -keypass R_and_D_Device_Manufacturer -inputjad %JAD_NAME% -outputjad %JAD_NAME%

@ECHO OFF
goto END

::::::::::::::: JDK is missing ::::::::::::::::::::::
:JDK_HELP
@ECHO OFF
ECHO "%JDK%" does NOT exist. Please set the path of the JDK environment into this script's JDK variable and try again. 
goto END

::::::::::::::: WTK is missing ::::::::::::::::::::::
:WTK_HELP
@ECHO OFF
ECHO "%WTK%" does NOT exist. Please set the path of the Wireless Toolkit into this script's WTK variable and try again. 
goto END

::::::::::::::: S60 env is missing ::::::::::::::::::::::
:JKEY_STORE_HELP
@ECHO OFF
ECHO "%JKEY_STORE%" does NOT exist. Please make sure that you run this script from the substituted drive corresponding to the S60 environment and try again.
goto END

:HELP
@ECHO OFF
ECHO Input parameters jad, jar or both missing
ECHO sigMidlet_Manufacturer.cmd "JAD_FILE.JAD" "JAR_FILE.JAR"

:END

@ECHO OFF
set JAD_NAME=
set JAR_NAME= 
set WTK=
set JKEY_STORE=
set JDK=