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
rem Script for running File API unit tests from S60 eshell
rem Ensure you have cleared the environment using cleanTestEnv.bat
rem Creating test environment
javafiletestinit
rem Starting tests
javainstaller test -mainclass=com.nokia.mj.test.file.TestFile_ALL -domain=MFD