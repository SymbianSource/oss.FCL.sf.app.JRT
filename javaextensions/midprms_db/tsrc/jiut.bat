@rem
@rem Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
@rem Description: Script for running RMS unit tests in S60 Emulator (eshell)
@rem
javainstaller install -jad=c:\data\installs\rmssharedtests.jad -silent
javainstaller install -jad=c:\data\installs\rmstests.jad -silent
javainstaller install -jad=c:\data\installs\rmsintersuite.jad -silent
javarms_tester -v
javainstaller test -mainclass=com.nokia.mj.test.rms.RmsMain
