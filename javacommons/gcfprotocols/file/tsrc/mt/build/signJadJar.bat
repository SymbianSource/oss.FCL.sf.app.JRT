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
@echo off
echo This script will sign given jad and jar files

rem Make sure WTK_HOME has been set
if NOT EXIST %WTK_HOME%/ktoolbarApp.ico set WTK_HOME=C:\WTK22
if NOT EXIST %WTK_HOME%/ktoolbarApp.ico echo You must define WTK_HOME!
if NOT EXIST %WTK_HOME%/ktoolbarApp.ico EXIT /B 1

rem Make sure JAVA_HOME has been set
if NOT EXIST %JAVA_HOME%/LICENSE set JAVA_HOME=C:\APPS\jdk1.5.0_10
if NOT EXIST %JAVA_HOME%/LICENSE echo You must define JAVA_HOME to JDK 1.5.0!
if NOT EXIST %JAVA_HOME%/LICENSE EXIT /B 1

rem Make sure Path to key store has been set
if NOT EXIST %JKEY_STORE%/JPerf.ks set JKEY_STORE=C:\Users\MyStuff\JSRs\JP_KeyStore
if NOT EXIST %JKEY_STORE%/JPerf.ks echo You must define location of Key store
if NOT EXIST %JKEY_STORE%/JPerf.ks EXIT /B 1

rem Make sure Path to key store has been set
set JAD_NAME=JsrTestRunner.jad
set JAR_NAME=JsrTestRunner.jar

rem Signing midlet :-)

%JAVA_HOME%\bin\java -jar %WTK_HOME%\bin\JADTool.jar -addcert -keystore %JKEY_STORE%\JPerf.ks -alias JP_Device_Manufacturer -storepass jperformance -certnum 1 -chainnum 1 -inputjad %JAD_NAME%  -outputjad %JAD_NAME%
%JAVA_HOME%\bin\java -jar %WTK_HOME%\bin\JADTool.jar -addjarsig -jarfile %JAR_NAME% -keystore %JKEY_STORE%\JPerf.ks -alias JP_Device_Manufacturer -storepass jperformance -keypass R_and_D_Device_Manufacturer -inputjad %JAD_NAME% -outputjad %JAD_NAME%