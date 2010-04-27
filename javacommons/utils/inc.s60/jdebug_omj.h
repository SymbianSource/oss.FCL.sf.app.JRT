/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#ifndef JDEBUG_OMJ_H
#define JDEBUG_OMJ_H

#define JAVA_OLD_STYLE_LOGGING_ON

#include <flogger.h>

_LIT(KLogDirectory, "java");
_LIT(KLogFileName, "java.txt");

// Strings for flogger directory and file names
#include <string>

#define DEBUG_START { char* _Dbg_str = new char[2500]; sprintf( _Dbg_str,
#define DEBUG_END   );\
                    TPtrC8 ptr( (TUint8*)_Dbg_str );\
                    HBufC* buf = HBufC::New(ptr.Length() + 1);\
                    if (buf) {\
                        TPtr16 tmpPtr = buf->Des();\
                        tmpPtr.Copy(ptr);\
                        tmpPtr.ZeroTerminate();\
                    }\
                    RFileLogger::Write(KLogDirectory, KLogFileName, EFileLoggingModeAppend,*buf);\
                    delete buf;\
                    delete _Dbg_str;\
                }
#ifdef JAVA_OLD_STYLE_LOGGING_ON

// these macros are documented below.
#define DEBUG( msg ) DEBUG_START msg DEBUG_END
#define DEBUG_STR( msg, string ) DEBUG_START msg, string DEBUG_END
#define DEBUG_STR_INT( msg, string, num ) DEBUG_START msg, string, num DEBUG_END
#define DEBUG_INT( msg, num ) DEBUG_START msg, num DEBUG_END
#define DEBUG_INT2( msg, num, num2 ) DEBUG_START msg, num, num2 DEBUG_END
#define DEBUG_INT3( msg, num, num2, num3 ) DEBUG_START msg, num, num2, num3 DEBUG_END
#define DEBUG_INT4( msg, num, num2, num3, num4 ) DEBUG_START msg, num, num2, num3, num4 DEBUG_END
#define DEBUG_INT5( msg, num, num2, num3, num4, num5) DEBUG_START msg, num, num2, num3, num4, num5 DEBUG_END
#define DEBUG_INT6( msg, num, num2, num3, num4, num5, num6 ) DEBUG_START msg, num, num2, num3, num4, num5, num6 DEBUG_END

#else

#define DEBUG( msg )
#define DEBUG_STR( msg, string )
#define DEBUG_STR_INT( msg, string )
#define DEBUG_INT( msg, num )
#define DEBUG_INT2( msg, num, num2 )
#define DEBUG_INT3( msg, num, num2, num3 )
#define DEBUG_INT4( msg, num, num2, num3, num4 )
#define DEBUG_INT5( msg, num, num2, num3, num4, num5 )
#define DEBUG_INT6( msg, num, num2, num3, num4, num5, num6 )
#endif // JAVA_OLD_STYLE_LOGGING_ON

#define ERROR( msg ) RFileLogger::Write(KLogDirectory, KLogFileName, EFileLoggingModeAppend, _L(msg))
#define ERROR_INT( msg, num ) RFileLogger::WriteFormat(KLogDirectory, KLogFileName, EFileLoggingModeAppend,_L(msg), num )
#define ERROR_STR( msg, string )  DEBUG_START msg, string DEBUG_END

#endif // JDEBUG_OMJ_H

