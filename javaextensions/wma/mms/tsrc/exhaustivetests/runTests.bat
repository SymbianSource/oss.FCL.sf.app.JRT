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
set start_path=%CD%

rem ******************* Compilation and installation of the tests ************************************************************************
call ant 

rem IF THE INSTALLATION OF MIDLETS FROM COMMAND LINE WORKS THEN:
rem    . UNCOMMENT LINE NO. 12
rem    . COMMENT OUT LINES NO.14, 15, 16
rem call ant install

rem THIS IS A WORKAROUND FOR THE MIDLET NOT BEING INSTALLED FROM COMMAND LINE
echo Install the MIDlet, exit the emulator and
pause

rem ********************* TEST PREPARATIONS: configure MMSEngine for the auto-receiving of some MMSs **************************************
copy .\data\localmode.ini \epoc32\WINSCW\C\system\data\
cd \epoc32\WINSCW\C\Data\Images\
mkdir wma20
mkdir mmstest
cd %start_path%
unzip .\data\MMSs.zip -d \epoc32\WINSCW\C\Data\Images\wma20\
ren \epoc32\release\winscw\udeb\Z\resource\java\security\policies\untrusted.ser untrusted.ser_
copy .\data\untrusted.ser \epoc32\release\winscw\udeb\Z\resource\java\security\policies

rem IF RUNNING OF MIDLETS FROM COMMAND LINE WORKS THEN:
rem    . UNCOMMENT LINE NO.31
rem    . COMMENT OUT LINES NO.33, 34, 35
rem call ant run

rem THIS IS A WORKAROUND FOR NOT BEING ABLE TO RUN THE MIDLET FROM COMMAND LINE
echo Run the MIDlet, exit the emulator and
pause


:cleanup
del /Q \epoc32\WINSCW\C\system\data\localmode.ini
del /Q \epoc32\WINSCW\C\Data\Images\mmstest\*.*
del /Q \epoc32\WINSCW\C\Data\Images\wma20\*.*
cd \epoc32\WINSCW\C\Data\Images\
rmdir wma20
rmdir mmstest
del /Q \epoc32\release\winscw\udeb\Z\resource\java\security\policies\untrusted.ser
ren \epoc32\release\winscw\udeb\Z\resource\java\security\policies\untrusted.ser_ untrusted.ser
cd %start_path%

:end