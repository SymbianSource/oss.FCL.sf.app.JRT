@rem Copyright(c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
@rem Script for running JavaCommonUtils unit tests from S60 eshell
cls
javaunittester -mainclass=com.nokia.mj.impl.utils.LoggerTests
javaunittester -Dcom.nokia.jvm.port=unexiting -mainclass=com.nokia.mj.impl.utils.LoggerTests
javaunittester -conf=cdc -Dcom.nokia.jvm.port=unexiting -mainclass=com.nokia.mj.impl.utils.LoggerTests
javaunittester -mainclass=com.nokia.mj.impl.utils.BufferedReaderTests
javaunittester -mainclass=com.nokia.mj.impl.utils.concurrent.ConcurrentTests
javaunittester -mainclass=com.nokia.mj.impl.rt.SystemPropertyTests
javaunittester -conf=cdc -mainclass=com.nokia.mj.impl.rt.SystemPropertyTests
javaunittester -conf=foundation -mainclass=com.nokia.mj.impl.rt.SystemPropertyTests
javaunittester -mainclass=com.nokia.mj.impl.utils.DebugUtilsTests
javaunittester -Dcom.nokia.rt.port=test -mainclass=com.nokia.mj.impl.rt.support.ApplicationUtilsTest 1111
javaunittester -Dcom.nokia.rt.port=test -mainclass=com.nokia.mj.impl.rt.support.ApplicationUtilsTest 00001
javaunittester -Dcom.nokia.rt.port=test -mainclass=com.nokia.mj.impl.rt.support.ApplicationUtilsTest 000001
javaunittester -Dcom.nokia.rt.port=test -mainclass=com.nokia.mj.impl.rt.support.ApplicationUtilsTest 0000001
javaunittester -Dcom.nokia.rt.port=test2 -mainclass=com.nokia.mj.impl.rt.support.ApplicationUtilsTest 00000001
javaunittester -Dcom.nokia.rt.port=test -mainclass=com.nokia.mj.impl.rt.support.ApplicationInfoTest
javaunittester -Dcom.nokia.rt.port=test2 -mainclass=com.nokia.mj.impl.rt.support.ApplicationInfoTest 1
javaunittester -mainclass=com.nokia.mj.impl.rt.support.JvmPortBasicTest
javaunittester -cp c:\java\jvmportapp.jar -mainclass=com.nokia.mj.impl.rt.support.JvmPortTest 1
javaunittester -conf=cdc -cp c:\java\jvmportapp.jar -mainclass=com.nokia.mj.impl.rt.support.JvmPortTest 1
javaunittester -mainclass=com.nokia.mj.impl.rt.support.JvmPortTest 01
javaunittester -conf=cdc -mainclass=com.nokia.mj.impl.rt.support.JvmPortTest 01
javaunittester -mainclass=com.nokia.mj.impl.rt.support.JvmPortTest 001
javaunittester -conf=cdc -mainclass=com.nokia.mj.impl.rt.support.JvmPortTest 001
javaunittester -mainclass=com.nokia.mj.impl.rt.support.JvmPortTest 0001
javaunittester -conf=cdc -mainclass=com.nokia.mj.impl.rt.support.JvmPortTest 0001
javaunittester -mainclass=com.nokia.mj.impl.rt.support.JvmPortTest 00001
javaunittester -conf=cdc -mainclass=com.nokia.mj.impl.rt.support.JvmPortTest 00001
javaunittester -mainclass=com.nokia.mj.impl.rt.support.JvmPortTest 000001
javaunittester -conf=cdc -mainclass=com.nokia.mj.impl.rt.support.JvmPortTest 000001
javaunittester -Xmos8K -Xmns8K -Xmine4K -mainclass=com.nokia.mj.impl.rt.support.JvmPortTest 0000001
javaunittester -conf=cdc -Xmos8K -Xmns8K -Xmine4K -mainclass=com.nokia.mj.impl.rt.support.JvmPortTest 0000001
javaunittester -mainclass=com.nokia.mj.impl.rt.support.JvmPortTest 00000001
javaunittester -conf=cdc -mainclass=com.nokia.mj.impl.rt.support.JvmPortTest 00000001
javaunittester -cp c:\java\jvmportapp2.jar -mainclass=com.nokia.mj.impl.rt.support.JvmPortTest 000000001
javaunittester -conf=cdc -cp c:\java\jvmportapp.jar -mainclass=com.nokia.mj.impl.rt.support.JvmPortTest 000000001
javaunittester -mainclass=com.nokia.mj.impl.rt.support.FinalizerTest
javaunittester -conf=cdc -mainclass=com.nokia.mj.impl.rt.support.FinalizerTest
testjavautils -nomemleaks
