/*
* Copyright (c) 2002 - 2005 Nokia Corporation and/or its subsidiary(-ies).
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


/**
@file
@publishedPartner
@released

This header file contains three macros for outputing logs to a number of
different locations.

There are three macros:

JAVA_RDEBUG
    enables logging to the serial port.
JAVA_FILELOG
    writes a log to c:\logs\java\midp.txt if the directory exists.
JAVA_PRINTF
    enables printf logging.

The default behaviour for UDEB builds is to have only JAVA_FILELOG enabled.

To enable UREL file logging, the ifdef clause surrounding JAVA_FILELOG must
be removed, and one of the
/j2me/vms/extensions/src/epoc/native/util/include/enable_flogger*.bat files
must be executed before attempting to build  These batch files modify the
flogger include to be a standard library rather than a debuglibrary.

Please see /j2me/vms/extensions/src/epoc/native/util/include/readme.txt for
more details.
 */

#ifndef JDEBUG_H
#define JDEBUG_H

//////////////////////////////////////////////////////////////////////////////
// Customisation should only be required in the below section               //
//#define JAVA_RDEBUG
//#define JAVA_PRINTF
//#define JAVA_PROFILE
#ifdef _DEBUG // only on UDEB builds
#define JAVA_FILELOG
#endif

// Customisation should only be required in the above section               //
//////////////////////////////////////////////////////////////////////////////

#include <e32std.h>

// Debugging headers
#if defined (JAVA_RDEBUG) || defined (JAVA_PROFILE)
#include <e32svr.h>
#endif

#ifdef JAVA_FILELOG
#include <flogger.h>
#endif

#ifdef JAVA_PRINTF
#include <libc/stdio.h>
#endif


// Strings for flogger directory and file names
_LIT(KLogDirectory, "java\\full");
_LIT(KLogFileName, "midplegacy.log");

/**
Custom Component Logging
Add extra component literals here to use in components.
To add component logging into the main log change the
"<component>.txt" to "midp.txt"
*/

/**
To force all logging into main log file (midp.txt)
uncomment the following line */
//#define ONE_LOG

#if defined(JAVA_RDEBUG) || defined(JAVA_PRINTF) || defined(JAVA_FILELOG) || defined (JAVA_PROFILE)

// to fix compiler warnings
inline void Dummy(TBool /*aBool*/)
{
}
inline void Dummy(const TDesC& /*aComponentLogName*/)
{
}

//Splits the logging into multiple files , aComponentLogName is the log file associated
//with specific components eg
//http.txt will go into \logs\java\http.txt

inline void Debug(const TDesC& aComponentLogName,  const TDesC& aMsg, const TBool aBool)
{
#ifdef JAVA_FILELOG
    // Print to log file
#ifdef ONE_LOG
    const TDesC& ignore = aComponentLogName;
    RFileLogger::Write(KLogDirectory, KLogFileName, EFileLoggingModeAppend, aMsg);
#else
    RFileLogger::Write(KLogDirectory, aComponentLogName, EFileLoggingModeAppend, aMsg);
#endif
    Dummy(aBool);
#endif

#ifdef JAVA_PRINTF
    {
        if (aBool)
        {
            // Print to standard out
            TBuf8<0x100> msg;
            msg.Copy(aMsg);
            printf("C_DEBUG: ");
            const char* msgPtr = reinterpret_cast<const char*>(msg.PtrZ());
            printf(msgPtr);
            printf("\n");
        }
    }
#endif

#ifdef JAVA_RDEBUG
    // Print to debugger (WINS only)
    RDebug::Print(aMsg);
    Dummy(aComponentLogName);
    Dummy(aBool);
#endif
}

_LIT(KJavaProfile , "Java_profile:: low: %u , high: %u :%S");

#ifdef JAVA_PROFILE
inline void DebugProfile(const TDesC& aMsg)
{
    TTime time;
    time.UniversalTime();
    RDebug::Print(KJavaProfile , I64LOW(time.Int64()), I64HIGH(time.Int64()), &aMsg);
}
#else
inline void DebugProfile(const TDesC&)
{
}
#endif


inline void DebugString(const TDesC& aComponentLogName, const TDesC& aMsg, const TDesC& aString, const TBool aBool)
{
    // Print to log file
#ifdef JAVA_FILELOG
#ifdef ONE_LOG
    const TDesC& ignore = aComponentLogName;
    RFileLogger::WriteFormat(KLogDirectory, KLogFileName, EFileLoggingModeAppend, aMsg, &aString);
#else
    RFileLogger::WriteFormat(KLogDirectory, aComponentLogName, EFileLoggingModeAppend, aMsg, &aString);
#endif
    Dummy(aBool);
#endif

#ifdef JAVA_PRINTF
    // Locally scope the code because temporary variables are defined and inlined
    {
        if (aBool)
        {
            // Print to standard out
            TBuf8<0x100> msg;
            msg.Copy(aMsg);
            printf("C_DEBUG: ");
            const char* msgPtr = reinterpret_cast<const char*>(msg.PtrZ());
            TBuf8<0x100> tmpString(0xFF);
            tmpString.Copy(aString.Left(0xFF));
            printf(msgPtr);
            printf("tring:\"%s\"\n", tmpString.PtrZ());
        }
    }
#endif

#ifdef JAVA_RDEBUG
    // Print to debugger (WINS only)
    RDebug::Print(aMsg, &aString);
    Dummy(aComponentLogName);
    Dummy(aBool);
#endif
}

inline void DebugInt(const TDesC& aComponentLogName, const TDesC& aMsg, TInt aNum, const TBool aBool)
{
    // Print to log file
#ifdef JAVA_FILELOG
#ifdef ONE_LOG
    const TDesC& ignore = aComponentLogName;
    RFileLogger::WriteFormat(KLogDirectory, KLogFileName, EFileLoggingModeAppend, aMsg, aNum);
#else
    RFileLogger::WriteFormat(KLogDirectory, aComponentLogName, EFileLoggingModeAppend, aMsg, aNum);
#endif
    Dummy(aBool);
#endif

#ifdef JAVA_PRINTF
    {
        if (aBool)
        {
            // Print to standard out
            TBuf8<0x100> msg;
            msg.Copy(aMsg);
            printf("C_DEBUG: ");
            const char* msgPtr = reinterpret_cast<const char*>(msg.PtrZ());
            printf(msgPtr, aNum);
            printf("\n");
        }
    }
#endif

#ifdef JAVA_RDEBUG
    // Print to debugger (WINS only)
    RDebug::Print(aMsg, aNum);
    Dummy(aComponentLogName);
    Dummy(aBool);
#endif
}

inline void DebugInt2(const TDesC& aComponentLogName, const TDesC& aMsg, TInt aNum, TInt aNum2, const TBool aBool)
{
    // Print to log file
#ifdef JAVA_FILELOG
#ifdef ONE_LOG
    const TDesC& ignore = aComponentLogName;
    RFileLogger::WriteFormat(KLogDirectory, KLogFileName, EFileLoggingModeAppend,
                             aMsg, aNum, aNum2);
#else
    RFileLogger::WriteFormat(KLogDirectory, aComponentLogName, EFileLoggingModeAppend,
                             aMsg, aNum, aNum2);
#endif
    Dummy(aBool);
#endif

#ifdef JAVA_PRINTF
    {
        if (aBool)
        {
            // Print to standard out
            TBuf8<0x100> msg;
            msg.Copy(aMsg);
            printf("C_DEBUG: ");
            const char* msgPtr = reinterpret_cast<const char*>(msg.PtrZ());
            printf(msgPtr, aNum, aNum2);
            printf("\n");
        }
    }
#endif

#ifdef JAVA_RDEBUG
    // Print to debugger (WINS only)
    RDebug::Print(aMsg, aNum, aNum2);
    Dummy(aComponentLogName);
    Dummy(aBool);
#endif
}

inline void DebugInt3(const TDesC& aComponentLogName, const TDesC& aMsg, TInt aNum, TInt aNum2, TInt aNum3, const TBool aBool)
{
    // Print to log file
#ifdef JAVA_FILELOG
#ifdef ONE_LOG
    const TDesC& ignore = aComponentLogName;
    RFileLogger::WriteFormat(KLogDirectory, KLogFileName, EFileLoggingModeAppend,
                             aMsg, aNum, aNum2, aNum3);
#else
    RFileLogger::WriteFormat(KLogDirectory, aComponentLogName, EFileLoggingModeAppend,
                             aMsg, aNum, aNum2, aNum3);
#endif
    Dummy(aBool);
#endif

#ifdef JAVA_PRINTF
    {
        if (aBool)
        {
            // Print to standard out
            TBuf8<0x100> msg;
            msg.Copy(aMsg);
            printf("C_DEBUG: ");
            const char* msgPtr = reinterpret_cast<const char*>(msg.PtrZ());
            printf(msgPtr, aNum, aNum2, aNum3);
            printf("\n");
        }
    }
#endif

#ifdef JAVA_RDEBUG
    // Print to debugger (WINS only)
    RDebug::Print(aMsg, aNum, aNum2, aNum3);
    Dummy(aComponentLogName);
    Dummy(aBool);
#endif
}



inline void DebugInt4(const TDesC& aComponentLogName, const TDesC& aMsg, TInt aNum, TInt aNum2, TInt aNum3, TInt aNum4, const TBool aBool)
{
    // Print to log file
#ifdef JAVA_FILELOG
#ifdef ONE_LOG
    const TDesC& ignore = aComponentLogName;
    RFileLogger::WriteFormat(KLogDirectory, KLogFileName, EFileLoggingModeAppend,
                             aMsg, aNum, aNum2, aNum3, aNum4);
#else
    RFileLogger::WriteFormat(KLogDirectory, aComponentLogName, EFileLoggingModeAppend,
                             aMsg, aNum, aNum2, aNum3, aNum4);
#endif
    Dummy(aBool);
#endif

#ifdef JAVA_PRINTF
    {
        if (aBool)
        {
            // Print to standard out
            TBuf8<0x100> msg;
            msg.Copy(aMsg);
            printf("C_DEBUG: ");
            const char* msgPtr = reinterpret_cast<const char*>(msg.PtrZ());
            printf(msgPtr, aNum, aNum2, aNum3, aNum4);
            printf("\n");
        }
    }
#endif

#ifdef JAVA_RDEBUG
    // Print to debugger (WINS only)
    RDebug::Print(aMsg, aNum, aNum2, aNum3, aNum4);
    Dummy(aComponentLogName);
    Dummy(aBool);
#endif
}



inline void DebugString8(const TDesC& aComponentLogName, const TDesC& aMsg, const TDesC8& aString, const TBool aBool)
{
    TRAPD(error,
          HBufC16* expandedMessage = HBufC16::NewLC(aString.Length());
          expandedMessage->Des().Copy(aString);
          DebugString(aComponentLogName,aMsg, *expandedMessage, aBool);
          CleanupStack::PopAndDestroy(expandedMessage);
         )
    if (error != KErrNone)
    {
        _LIT(KError, "jdebug.h: Error expanding 8 bit String");
        DebugString(aComponentLogName, aMsg, KError, aBool);
        _LIT(KMsg, "jdebug.h: Error=%D Length=%D");
        DebugInt2(aComponentLogName, KMsg, error, aString.Length(), aBool);
    }
}



inline void DebugAssert(TBool aCondition, const TDesC& aMsg)
{
    if (!aCondition)
    {
        DebugString(KLogFileName, _L("ASSERT: %S"), aMsg, ETrue);
        ASSERT(EFalse);
    }
}

// these macros are documented below.
#define DEBUG( msg ) Debug( KLogFileName, _L( msg ), ETrue)
#define DEBUGCOMP( comp, msg ) Debug( comp, _L( msg ), ETrue)
#define DEBUG_ASSERT( condition, msg ) DebugAssert( condition, _L( msg ) )
#define DEBUG_STR( msg, string ) DebugString( KLogFileName, _L( msg ), string, ETrue )
#define DEBUGCOMP_STR( comp, msg, string ) DebugString( comp , _L( msg ), string, ETrue )
#define DEBUG_STR8( msg, string ) DebugString8( KLogFileName, _L( msg ), string, ETrue )
#define DEBUGCOMP_STR8( comp, msg, string ) DebugString8( comp, _L( msg ), string, ETrue )
#define DEBUG_INT( msg, num ) DebugInt( KLogFileName, _L( msg ), num, ETrue )
#define DEBUGCOMP_INT( comp, msg, num ) DebugInt( comp , _L( msg ), num, ETrue )
#define DEBUG_INT2( msg, num, num2 ) DebugInt2( KLogFileName, _L( msg ), num, num2, ETrue )
#define DEBUGCOMP_INT2( comp, msg, num, num2 ) DebugInt2( comp, _L( msg ), num, num2, ETrue )
#define DEBUG_INT3( msg, num, num2, num3 ) DebugInt3( KLogFileName, _L( msg ), num, num2, num3, ETrue )
#define DEBUGCOMP_INT3( comp, msg, num, num2, num3 ) DebugInt3( comp , _L( msg ), num, num2, num3, ETrue )
#define DEBUG_INT4( msg, num, num2, num3, num4 ) DebugInt4( KLogFileName, _L( msg ), num, num2, num3, num4, ETrue )
#define DEBUGCOMP_INT4( comp, msg, num, num2, num3, num4 ) DebugInt4( comp , _L( msg ), num, num2, num3, num4, ETrue )
#define DEBUG_ERR( msg, num ) if ( num < KErrNone ) DebugInt( KLogFileName, _L( msg ), num, ETrue )
#define DEBUG_PROFILE(msg) DebugProfile(_L( msg ))

// don't print to standard out
#define DEBUG_NOSTDLIB( msg) Debug( KLogFileName, _L( msg ), EFalse )
#define DEBUG_STR_NOSTDLIB( msg, string ) DebugString( KLogFileName, _L( msg ), string, EFalse )
#define DEBUG_STR8_NOSTDLIB( msg, string ) DebugString8( KLogFileName, _L( msg ), string, EFalse )
#define DEBUG_INT_NOSTDLIB( msg, num, abool ) DebugInt( KLogFileName, _L( msg ), num, EFalse )
#define DEBUG_INT2_NOSTDLIB( msg, num, num2 ) DebugInt2( KLogFileName, _L( msg ), num, num2, EFalse )
#define DEBUG_INT3_NOSTDLIB( msg, num, num2, num3 ) DebugInt3( KLogFileName, _L( msg ), num, num2, num3, EFalse )
#define DEBUG_INT4_NOSTDLIB( msg, num, num2, num3, num4 ) DebugInt4( KLogFileName, _L( msg ), num, num2, num3, num4, EFalse )
#define DEBUG_ERR_NOSTDLIB( msg, num, ETrue ) if ( num < KErrNone ) DebugInt( KLogFileName, _L( msg ), num, EFalse )


#else // #if defined(JAVA_RDEBUG) || defined(JAVA_PRINTF) || defined(JAVA_FILELOG)


/** Prints a message. */
#define DEBUG( msg )
/** comp specifies a file to put logging in eg http.txt == \logs\java\http.txt
* Literal is defined at top of this file eg KLogFileHttp */
#define DEBUGCOMP( comp, msg )
/** Tests the specified condition, and if it evaluates to
false, prints the message, preceded by the string "ASSERT: "
and raises a USER 0 panic. */
#define DEBUG_ASSERT( condition, msg )
/** Prints a message with a string argument.

Use a %S format character in the message to specify where the
string should be inserted. */
#define DEBUG_STR( msg, string )
/** comp specifies a file to put logging in eg http.txt == \logs\java\http.txt */
/* Literal is defined at top of this file eg KLogFileHttp */
#define DEBUGCOMP_STR( comp, msg, string )
/** Prints a message with a 8 bit string argument.

Use a %S format character in the message to specify where the
string should be inserted. */
#define DEBUG_STR8( msg, string )
/** comp specifies a file to put logging in eg http.txt == \logs\java\http.txt */
/* Literal is defined at top of this file eg KLogFileHttp */
#define DEBUGCOMP_STR8( comp, msg, string )
/** Prints a message with an integer argument.

Use a %D format character in the message to specify where the
integer should be inserted. */
#define DEBUG_INT( msg, num )
/** comp specifies a fire to put logging in eg http.txt == \logs\java\http.txt */
/* Literal is defined at top of this file eg KLogFileHttp */
#define DEBUGCOMP_INT( comp, msg, num )
/** Prints a message with two integer arguments.

Use %D format characters in the message to specify where the
integers should be inserted. */
#define DEBUG_INT2( msg, num, num2 )
/** comp specifies a file to put logging in eg http.txt == \logs\java\http.txt */
/* Literal is defined at top of this file eg KLogFileHttp */
#define DEBUGCOMP_INT2( comp, msg, num, num2 )
/** Prints a message with three integer arguments.

Use %D format characters in the message to specify where the
integers should be inserted. */
#define DEBUG_INT3( msg, num, num2, num3 )
/** comp specifies a fire to put logging in eg http.txt == \logs\java\http.txt */
/* Literal is defined at top of this file eg KLogFileHttp */
#define DEBUGCOMP_INT3( comp, msg, num, num2, num3 )
/** Prints a message with four integer arguments.

Use %D format characters in the message to specify where the
integers should be inserted. */
#define DEBUG_INT4( msg, num, num2, num3, num4 )
/** comp specifies a fire to put logging in eg http.txt == \logs\java\http.txt */
/* Literal is defined at top of this file eg KLogFileHttp */
#define DEBUGCOMP_INT4( comp, msg, num, num2, num3, num4 )
/** Prints a message with an integer error code argument.

Use a %D format character in the message to specify where the
error code should be inserted.

Note that a message is only printed if the integer value is
negative. */
#define DEBUG_ERR( msg, num )
/** Prints msg plus the utc time in milliseconds to allow profiling via RDebug::Print. */
#define DEBUG_PROFILE(msg)

/** Prints a message.

This is the same as DEBUG except the message is not printed
to the standard output. */
#define DEBUG_NOSTDLIB( msg )
/** Prints a message with a string argument.

This is the same as DEBUG_STR except the message is not
printed to the standard output. */
#define DEBUG_STR_NOSTDLIB( msg, string )
/** Prints a message with a string argument.

This is the same as DEBUG_STR except the message is not
printed to the standard output. */
#define DEBUG_STR8_NOSTDLIB( msg, string )
/** Prints a message with an integer argument.

This is the same as DEBUG_INT except the message is not
printed to the standard output. */
#define DEBUG_INT_NOSTDLIB( msg, num )
/** Prints a message with two integer arguments.

This is the same as DEBUG_INT2 except the message is not
printed to the standard output. */
#define DEBUG_INT2_NOSTDLIB( msg, num, num2 )
/** Prints a message with three integer arguments.

This is the same as DEBUG_INT3 except the message is not
printed to the standard output. */
#define DEBUG_INT3_NOSTDLIB( msg, num, num2, num3 )
/** Prints a message with four integer arguments.

This is the same as DEBUG_INT4 except the message is not
printed to the standard output. */
#define DEBUG_INT4_NOSTDLIB( msg, num, num2, num3, num4 )
/** Prints a message with an integer error code argument.

This is the same as DEBUG_ERR except the message is not
printed to the standard output. */
#define DEBUG_ERR_NOSTDLIB( msg, num )


#endif // #if defined(JAVA_RDEBUG) || defined(JAVA_PRINTF) || defined(JAVA_FILELOG)


#ifdef _DEBUG

#define DEBUG_LOG_ONLY( code ) code
#define DEBUG_ONLY( code ) code

// The COMPILE_ASSERT macro is useful for ensuring that number values
// defined in Java retain the same values as their native counterparts,
// for example in the case of error codes
template <TBool> struct ASSERTION_FAILURE;
TEMPLATE_SPECIALIZATION struct ASSERTION_FAILURE<ETrue> {};
template <TInt> struct __assertion_test;
#define COMPILE_ASSERT( B ) void __compile_assert(::__assertion_test<sizeof(::ASSERTION_FAILURE<(B)>)>)
#define OOM_DEBUG_LOOP_START TInt error = KErrNoMemory; for( TInt fail = 1; error == KErrNoMemory; fail++ ) { DEBUG_INT( "create connection OOM loop: %d", fail );  __UHEAP_FAILNEXT(fail); __UHEAP_MARK;
#define OOM_DEBUG_LOOP_END __UHEAP_MARKEND; } __UHEAP_SETFAIL(RHeap::ENone,0);

#else

/** Code inside the brackets is only for debug or for logging in release build  */
#if defined(JAVA_RDEBUG) || defined(JAVA_PRINTF) || defined (JAVA_PROFILE)
#define DEBUG_LOG_ONLY( code ) code
#else
#define DEBUG_LOG_ONLY( code )
#endif

/** Marks the code inside the brackets as being for debug
builds only. */
#define DEBUG_ONLY( code )

/** Asserts that the code inside the brackets must be true at
compile time, otherwise compilation fails.

This macro is useful for ensuring that numeric values defined in
Java retain the same values as their native counterparts, for
example in the case of error codes. */
#define COMPILE_ASSERT( B )

/** Marks the start of out-of-memory testing.

This and OOM_DEBUG_LOOP_END are used to test that all heap memory
allocated in the code block enclosed by the two macros is deallocated.
The code block is called in a loop, and heap allocation failure is
simulated each time around. If the code fails the test, a panic occurs. */
#define OOM_DEBUG_LOOP_START
/** Marks the end of out-of-memory testing. */
#define OOM_DEBUG_LOOP_END

#endif // #ifdef _DEBUG

#endif // JDEBUG_H


