/*
* Copyright (c) 2008 - 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: Java platform 2.0 javaapppreconverter process
*
*/


#ifndef NOARMLOGS_H
#define NOARMLOGS_H

// Logging makes the process dependent on OMJ javautils.dll and javaenv.dll.
// However they are no available in device when converter processes are executed
// during OMJ installation before OMJ binaries have been copied to device
// nor when converter processes are executed  during OMJ uninstallation after
// OMJ binaries have been removed from device.
// If included in ARMV5 compilation this header file redefines all OMJ
// logging macros so that they use directly RFileLogger so that they can be used without
// javautils.dll and javaenv.dll. Otherwise includes normal logging header file because
// in emulator javautils.dll and javaenv.dll are available always.

#ifndef __WINS__

#undef JELOG
#undef JELOG2
#undef JELOG3
#undef JELOG4

#undef LOG
#undef LOG1
#undef LOG2
#undef LOG3
#undef LOG4

#undef WLOG
#undef WLOG1
#undef WLOG2
#undef WLOG3
#undef WLOG4

#undef ELOG
#undef ELOG1
#undef ELOG2
#undef ELOG3
#undef ELOG4

#undef LOG1WSTR

#include <flogger.h>
_LIT(KJavaLogDir, "java\\full");
_LIT(KJavaLogFile, "JavaConverters.log");

#define JELOG(component, str)
#define JELOG2(component)
#define JELOG3(component, level, str)
#define JELOG4(component, level)

#define LOG(component, level, str)  { TBuf8<1024> f((const TUint8 *)str); RFileLogger::Write(KJavaLogDir, KJavaLogFile, EFileLoggingModeAppend, f); }
#define LOG1(component, level, str, a)  { TBuf8<1024> f((const TUint8 *)str); RFileLogger::WriteFormat(KJavaLogDir, KJavaLogFile, EFileLoggingModeAppend, f, a); }
#define LOG2(component, level, str, a, b) { TBuf8<1024> f((const TUint8 *)str); RFileLogger::WriteFormat(KJavaLogDir, KJavaLogFile, EFileLoggingModeAppend, f, a, b); }
#define LOG3(component, level, str, a, b, c) { TBuf8<1024> f((const TUint8 *)str); RFileLogger::WriteFormat(KJavaLogDir, KJavaLogFile, EFileLoggingModeAppend, f, a, b, c); }
#define LOG4(component, level, str, a, b, c, d) { TBuf8<1024> f((const TUint8 *)str); RFileLogger::WriteFormat(KJavaLogDir, KJavaLogFile, EFileLoggingModeAppend, f, a, b, c, d); }

#define WLOG(component, str)  { TBuf8<1024> f((const TUint8 *)str); RFileLogger::Write(KJavaLogDir, KJavaLogFile, EFileLoggingModeAppend, f); }
#define WLOG1(component, str, a) { TBuf8<1024> f((const TUint8 *)str); RFileLogger::WriteFormat(KJavaLogDir, KJavaLogFile, EFileLoggingModeAppend, f, a); }
#define WLOG2(component, str, a, b) { TBuf8<1024> f((const TUint8 *)str); RFileLogger::WriteFormat(KJavaLogDir, KJavaLogFile, EFileLoggingModeAppend, f, a, b); }
#define WLOG3(component, str, a, b, c) { TBuf8<1024> f((const TUint8 *)str); RFileLogger::WriteFormat(KJavaLogDir, KJavaLogFile, EFileLoggingModeAppend, f, a, b, c); }
#define WLOG4(component, str, a, b, c, d) { TBuf8<1024> f((const TUint8 *)str); RFileLogger::WriteFormat(KJavaLogDir, KJavaLogFile, EFileLoggingModeAppend, f, a, b, c, d); }

#define ELOG(component, str) { TBuf8<1024> f((const TUint8 *)str); RFileLogger::Write(KJavaLogDir, KJavaLogFile, EFileLoggingModeAppend, f); }
#define ELOG1(component, str, a) { TBuf8<1024> f((const TUint8 *)str); RFileLogger::WriteFormat(KJavaLogDir, KJavaLogFile, EFileLoggingModeAppend, f, a); }
#define ELOG2(component, str, a, b) { TBuf8<1024> f((const TUint8 *)str); RFileLogger::WriteFormat(KJavaLogDir, KJavaLogFile, EFileLoggingModeAppend, f, a, b); }
#define ELOG3(component, str, a, b, c) { TBuf8<1024> f((const TUint8 *)str); RFileLogger::WriteFormat(KJavaLogDir, KJavaLogFile, EFileLoggingModeAppend, f, a, b, c); }
#define ELOG4(component, str, a, b, c, d) { TBuf8<1024> f((const TUint8 *)str); RFileLogger::WriteFormat(KJavaLogDir, KJavaLogFile, EFileLoggingModeAppend, f, a, b, c, d); }

#define LOG1WSTR(component, level, str, wstr) { TBuf8<1024> f((const TUint8 *)str); RFileLogger::Write(KJavaLogDir, KJavaLogFile, EFileLoggingModeAppend, f); TBuf16<KLogBufferSize> w((const TUint16 *)wstr); RFileLogger::Write(KJavaLogDir, KJavaLogFile, EFileLoggingModeAppend, w); }

#else

// normal logging
#include "logger.h"

#endif  // __WINS__

#endif // NOARMLOGS_H
