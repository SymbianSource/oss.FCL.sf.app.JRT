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
ren \epoc32\RELEASE\winscw\udeb\Z\system\data\midp2\security\policy\midp2_rp.xpf midp2_rp.xpf_
copy ..\..\..\data\midp2_rp.xpf \epoc32\RELEASE\winscw\udeb\Z\system\data\midp2\security\policy\

call ant compile
call ant jar
call ant deploy

rem IF THE INSTALLATION OF MIDLETS FROM COMMAND LINE WORKS THEN:
rem    . UNCOMMENT LINE NO. 12
rem    . COMMENT OUT LINES NO.14, 15, 16
rem call ant install

rem THIS IS A WORKAROUND FOR THE MIDLET NOT BEING INSTALLED FROM COMMAND LINE
echo Install and run the MIDlet and
pause

del /Q \epoc32\RELEASE\winscw\udeb\Z\system\data\midp2\security\policy\midp2_rp.xpf
ren \epoc32\RELEASE\winscw\udeb\Z\system\data\midp2\security\policy\midp2_rp.xpf_ midp2_rp.xpf