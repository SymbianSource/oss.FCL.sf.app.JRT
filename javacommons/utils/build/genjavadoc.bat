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
setlocal

if """%JAVA_HOME%"""=="""""" goto NoJavaHome


set JAVADOC=%JAVA_HOME%\bin\javadoc 

%JAVADOC% -d ./javadocs -public -sourcepath ..\javasrc;..\..\jvms\cldc_1.1.1\javasrc;..\..\fileutils\javasrc com.nokia.mj.impl.rt.support com.nokia.mj.impl.utils java.security com.nokia.mj.impl.fileutils
goto OKEnd

:NoJavaHome
echo JAVA_HOME must be defined
:OKEnd
endlocal
