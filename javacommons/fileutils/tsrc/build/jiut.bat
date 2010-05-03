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
rem Script for running FileUtils unit tests from S60 eshell
rem Ensure you have cleared the environment using cleanTestEnv.bat
rem Creating test environment
javafileutilstestinit
rem Starting tests
rem javainstaller test -mainclass=com.nokia.mj.test.fileutils.TestFileCreate
rem javainstaller test -mainclass=com.nokia.mj.test.fileutils.TestExists
rem javainstaller test -mainclass=com.nokia.mj.test.fileutils.TestIsFile
rem javainstaller test -mainclass=com.nokia.mj.test.fileutils.TestIsDir
rem javainstaller test -mainclass=com.nokia.mj.test.fileutils.TestCanRead
rem javainstaller test -mainclass=com.nokia.mj.test.fileutils.TestCanWrite
rem javainstaller test -mainclass=com.nokia.mj.test.fileutils.TestFileSize
rem javainstaller test -mainclass=com.nokia.mj.test.fileutils.TestSystemSpace
rem javainstaller test -mainclass=com.nokia.mj.test.fileutils.TestLastModified
rem javainstaller test -mainclass=com.nokia.mj.test.fileutils.TestDirCreate
rem javainstaller test -mainclass=com.nokia.mj.test.fileutils.TestRename
rem javainstaller test -mainclass=com.nokia.mj.test.fileutils.TestInputStream
rem javainstaller test -mainclass=com.nokia.mj.test.fileutils.TestList
rem javainstaller test -mainclass=com.nokia.mj.test.fileutils.TestDelete
rem javainstaller test -mainclass=com.nokia.mj.test.fileutils.TestMkdirs
rem javainstaller test -mainclass=com.nokia.mj.test.fileutils.TestContentId