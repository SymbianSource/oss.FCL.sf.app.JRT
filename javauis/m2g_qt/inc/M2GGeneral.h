/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  General definitions
*
*/

#ifndef M2GGENERAL_H
#define M2GGENERAL_H

#include <e32base.h>
#include <jutils.h>

#if defined ( _DEBUG )
#if defined ( __WINSCW__ )
// #define M2G_DEBUG
#endif // __WINSCW__
#endif  // _DEBUG

#ifdef M2G_DEBUG
// #define M2G_INFO_POPUP
#define M2G_STATISTIC_DEBUG
#include <flogger.h>
#include <jdebug.h>
#ifdef M2G_STATISTIC_DEBUG
#include <HAL.h>
#include <hal_data.h>
#endif // M2G_STATISTIC_DEBUG
#endif // M2G_DEBUG

#include "JcfAutoPtr.h"// cfcommon::auto_ptr<>
#include "M2GNamespace.h"

//#if defined( __cplusplus )
//extern "C" {
//#endif

M2G_NS_START

// CONSTANTS
// == ALPHA BLEND CONSTANTS ==
const TUint8 KFullOpaque = 0xFF;
const TUint8 KFullTransparent = 0x00;

// == ERROR TEXT CONSTANTS ==
_LIT8(KM2GEmptyString8, "");
_LIT(KM2GEmptyString, "");
_LIT(KGraphicsContextErrText, "Invalid M2G graphics context");
_LIT(KOffscreenErrText, "Invalid M2G offscreen bitmap");
_LIT(KSvgProxyPointerErrText, "Invalid SVG proxy pointer");
_LIT(KSvgEnginePointerErrText, "Invalid SVG engine pointer");
_LIT(KSvgTargetGraphicsPointerErrText, "Invalid target graphics pointer");
_LIT(KNotSupportedErrText, "Not supported");

// == ERROR CODE CONSTANTS ==
const TInt KM2GOk = KErrNone;
const TInt KM2GNotOk = KErrGeneral;
const TInt KM2GHandleNotOk = KErrBadHandle;
const TInt KM2GArgNotOk = KErrArgument;
const TInt KM2GSupportedNotOk = KErrNotSupported;
const TInt KM2GMemoryNotOk = KErrNoMemory;

// Own error codes
const TInt KM2GIOException = -10;

// Java exception class names
_LIT8(KJavaExceptionClass, "java/lang/Exception");
_LIT8(KJavaIOExceptionClass, "java/io/IOException");
_LIT8(KJavaOutOfMemoryErrorClass, "java/lang/OutOfMemoryError");
_LIT8(KJavaNullPointerException, "java/lang/NullPointerException");

// == SVG CONSTANTS ==
const TInt KM2GRestartAttribute = 114; // M2GSVGConstants.AT_RESTART
const TInt KM2GRestartNever = 471; // M2GSVGConstants.RESTART_NEVER
const TInt KM2GRestartWhenNotActive = 472; // M2GSVGConstants.RESTART_WHENNOTACTIVE

// == HANDLE MACROS ==
#define M2G_INVALID_HANDLE /**/ 0

// FORWARD DECLARATIONS
class RWriteStream;

// FUNCTION PROTOTYPES

// CLASS DECLARATION
/**
 * General static methods
 * @lib -
 * @series Series S60 3.0
 */
class M2GGeneral
{
public: // STATIC METHODS
    /**
     * Check if a handle is valid
     * @since Series S60 3.0
     * @param aHandle
     * @return ETrue if a handle is valid.
     */
    template<typename T>
    inline static TBool IsHandleValid(const T& aHandle)
    {
        return ((aHandle == M2G_INVALID_HANDLE) ? EFalse : ETrue);
    }

    /**
     * Returns min
     * @since Series S60 3.0
     * @param a Left parameter
     * @param b Right parameter
     * @return Returns min
     */
    template<typename T>
    inline static T Min(const T& a, const T& b)
    {
        return ((a) <= (b) ? (a) : (b));
    }


    /**
     * Returns max
     * @since Series S60 3.0
     * @param a Left parameter
     * @param b Right parameter
     * @return Returns max
     */
    template<typename T>
    inline static T Max(const T& a, const T& b)
    {
        return ((a) >= (b) ? (a) : (b));
    }
    /**
     * Instantiates an Java exception object with the message specified by
     * message, and causes that exception to be thrown in Java side. A thrown
     * exception will be pending in the current thread, but does not
     * immediately disrupt native code execution.
     * @param aEnv pointer to JNI environment
     * @param aName The name of a java.lang.Throwable class.
     * @param aMsg The message used to construct the java.lang.Throwable object.
     */
    static void RaiseException(
        JNIEnv* aEnv,
        const TDesC8& aName,
        const TUint8* aMsg = (STATIC_CAST(const TDesC8&, KM2GEmptyString8)).Ptr());

    /**
     * Instantiates an Java exception object specified by error code,
     * and causes that exception to be thrown in Java side.
     * @param aEnv pointer to JNI environment
     * @param aErrorCode Error code.
     * @return Value passed in is returned if not an error.
     */
    static TInt CheckErrorCode(JNIEnv* aEnv, TInt aErrorCode);

    /**
     * If handle is invalid instantiates an Java exception object,
     * and causes that exception to be thrown in Java side.
     * @param aEnv pointer to JNI environment
     * @param aHandle Handle.
     * @return Handle passed in is returned if it is not invalid.
     */
    static TInt CheckHandle(JNIEnv* aEnv, TInt aHandle);

    /**
     * If handle is invalid or error code matches instantiates an
     * Java exception object, and causes that exception to be thrown in Java side.
     * @param aEnv pointer to JNI environment
     * @param aErrorCode
     * @param aHandle Handle.
     * @param aErrorResult Result that is returned if error.
     * @return Handle passed in is returned if no error otherwise the
     * .aErrorResult is returned.
     */
    static TInt CheckErrorCodeAndHandle(
        JNIEnv* aEnv, TInt aErrorCode, TInt aHandle, TInt aErrorResult);

    /**
     * Shows pop up dialog on the emulator
     * @since Series S60 3.1
     * @param aLine1
     * @param aLine2
    */
#ifdef M2G_INFO_POPUP
    static void PopupInfo(
        const TDesC& aLine1,
        const TDesC& aLine2);
#endif // M2G_INFO_POPUP


    /**
    * Data array wrapper.
    * @series Series S60 3.1
    */
    template <typename T>
    class TDataArrayPtr
    {

    public: // METHODS
        /**
         * Ctor
         */
        TDataArrayPtr(T* aData = 0, TInt aSize = 0) : iData(aData), iSize(aSize) {}

        /**
         * Return pointer to a data
         * @return Pointer
         */
        inline T* Begin()
        {
            return iData;
        }

        /**
         * Check if data is valid
         * @return ETrue if ok
         */
        inline TBool IsValid()
        {
            return (((iData != NULL) && (iSize > 0)) ? ETrue : EFalse);
        }

        /**
         * Return data size
         * @return Length
         */
        inline TInt Length()
        {
            return iSize;
        }

        /**
         * Operator [] const
         */
        inline const T& operator[](TInt aIndex) const
        {
            return iData[ aIndex ];
        }

        /**
         * Operator []
         */
        inline T& operator[](TInt aIndex)
        {
            return iData[ aIndex ];
        }

    private: // METHODS

        /**
         * Copy ctor
         * @param aRhs Right side array
         */
        TDataArrayPtr(const TDataArrayPtr& aRhs);

        /**
         * Assignment operator
         * @param aRhs Right side array
         * @return Array object
         */
        TDataArrayPtr& operator=(const TDataArrayPtr& aRhs);

    public: // VARIABLES
        T* iData;
        TInt iSize;
    };


    /**
     * Statistic info
     * @series Series S60 3.1
     */
#ifdef M2G_STATISTIC_DEBUG
    class TM2GStatistic
    {
    public: // METHODS
        /**
         * Ctor
         */
        TM2GStatistic();

        /**
         * Reset
         */
        void Reset();

        /**
         * Print
         * @param aPrintElapsedTime Print also en elapsed time
         */
        void Print(TBool aPrintElapsedTime = EFalse);

        /**
         * Print
         * @param aStream
         */
        void Print(RWriteStream& aStream);

    public: // STATIC METHODS
        static void Log();

    private: // METHODS
        /**
         * Copy ctor
         */
        TM2GStatistic(const TM2GStatistic&);

        /**
         * Assignment operator
         */
        TM2GStatistic& operator=(const TM2GStatistic&);

    private: // VARIABLES
        TTime iStartTime;
        TTime iEndTime;
        TThreadId iId;
        TName iThreadName;
    };
#endif // M2G_STATISTIC_DEBUG

private: // METHODS
    /**
     * Default constructor
     */
    M2GGeneral() {};

    /**
     * Copy constructor
     */
    M2GGeneral(const M2GGeneral&);

    /**
     * Assignment operator
     */
    M2GGeneral& operator=(const M2GGeneral&);
};

// MACROS
// == TOOL MACROS ==
#define M2G_MIN( a, b ) ( ( a ) <= ( b ) ? ( a ) : ( b ) )
#define M2G_MAX( a, b ) ( ( a ) >= ( b ) ? ( a ) : ( b ) )

// == SWITCHES ==
#define M2G_NO_EXCEPTION_SWITCH
// #define M2G_NO_NULL_POINTER_EXCEPTION_SWITCH

#if defined( M2G_DEBUG )
#define M2G_TRACE
#endif

// == EXCEPTION MACROS ==
// Common exception handling
#ifndef M2G_NO_EXCEPTION_SWITCH
#define M2G_THROW( ERR_CODE ) /**/ User::Leave( ( ERR_CODE ) )
#define M2G_THROW2( ERR_TEXT, ERR_CODE ) /**/ M2G_DEBUG_0( ERR_TEXT ); User::Leave( ( ERR_CODE ) )
#else
#define M2G_THROW( ERR_CODE ) /**/
#define M2G_THROW2( ERR_TEXT, ERR_CODE ) /**/
#endif // !M2G_NO_EXCEPTION_SWITCH

// Null pointer exception handling
#ifndef M2G_NO_NULL_POINTER_EXCEPTION_SWITCH
#define M2G_CHECK_NULL_POINTER( JNI, POINTER ) /**/ M2GGeneral::CheckHandle( ( JNI, POINTER ) )
#else
#define M2G_CHECK_NULL_POINTER( JNI, POINTER ) /**/
#endif // !M2G_NO_NULL_POINTER_EXCEPTION_SWITCH


// == STATIC LOG MACROS ==
#if defined( M2G_STATISTIC_DEBUG )
#define M2G_STATISTIC_LOG /**/ M2GGeneral::TM2GStatistic::Log()
#else // M2G_STATISTIC_DEBUG
#define M2G_STATISTIC_LOG /**/
#endif // !M2G_STATISTIC_DEBUG

// == POPUP MACROS ==
#if defined( M2G_INFO_POPUP )
#define M2G_POPUP( LINE1, LINE2 ) /**/ M2GGeneral::PopupInfo( LINE1, LINE2 );
#else // !M2G_INFO_POPUP
#define M2G_POPUP( LINE1, LINE2 ) /**/
#endif // M2G_INFO_POPUP

// == TRACE MACROS ==
#if defined( M2G_TRACE )
#define M2G_DEBUG_0( TEXT ) /**/ \
        RFileLogger::Write( KLogDirectory, KLogFileName, EFileLoggingModeAppend, _L( TEXT ) )

#define M2G_DEBUG_1( TEXT, VAL1 ) /**/ \
        RFileLogger::WriteFormat( KLogDirectory, KLogFileName, EFileLoggingModeAppend, /**/ \
        _L( TEXT ), VAL1 )

#define M2G_DEBUG_2( TEXT, VAL1, VAL2 ) /**/ \
        RFileLogger::WriteFormat( KLogDirectory, KLogFileName, EFileLoggingModeAppend, /**/ \
        _L( TEXT ), VAL1, VAL2 )

#define M2G_DEBUG_3( TEXT, VAL1, VAL2, VAL3 ) /**/ \
        RFileLogger::WriteFormat( KLogDirectory, KLogFileName, EFileLoggingModeAppend, /**/ \
        _L( TEXT ), VAL1, VAL2, VAL3 )

#define M2G_DEBUG_4( TEXT, VAL1, VAL2, VAL3, VAL4 ) /**/ \
        RFileLogger::WriteFormat( KLogDirectory, KLogFileName, EFileLoggingModeAppend, /**/ \
        _L( TEXT ), VAL1, VAL2, VAL3, VAL4 )

#define M2G_DEBUG_5( TEXT, VAL1, VAL2, VAL3, VAL4, VAL5 ) /**/ \
        RFileLogger::WriteFormat( KLogDirectory, KLogFileName, EFileLoggingModeAppend, /**/ \
        _L( TEXT ), VAL1, VAL2, VAL3, VAL4, VAL5 )

#define M2G_DEBUG_6( TEXT, VAL1, VAL2, VAL3, VAL4, VAL5, VAL6 ) /**/ \
        RFileLogger::WriteFormat( KLogDirectory, KLogFileName, EFileLoggingModeAppend, /**/ \
        _L( TEXT ), VAL1, VAL2, VAL3, VAL4, VAL5, VAL6 )

#define M2G_DEBUG_7( TEXT, VAL1, VAL2, VAL3, VAL4, VAL5, VAL6, VAL7 ) /**/ \
        RFileLogger::WriteFormat( KLogDirectory, KLogFileName, EFileLoggingModeAppend, /**/ \
        _L( TEXT ), VAL1, VAL2, VAL3, VAL4, VAL5, VAL6, VAL7 )
#else // !M2G_TRACE
#define M2G_POPUP( LINE1, LINE2 ) /**/

#define M2G_DEBUG_0( TEXT ) /**/
#define M2G_DEBUG_1( TEXT, VAL1 ) /**/
#define M2G_DEBUG_2( TEXT, VAL1, VAL2 ) /**/
#define M2G_DEBUG_3( TEXT, VAL1, VAL2, VAL3 ) /**/
#define M2G_DEBUG_4( TEXT, VAL1, VAL2, VAL3, VAL4 ) /**/
#define M2G_DEBUG_5( TEXT, VAL1, VAL2, VAL3, VAL4, VAL5 ) /**/
#define M2G_DEBUG_6( TEXT, VAL1, VAL2, VAL3, VAL4, VAL5, VAL6 ) /**/
#define M2G_DEBUG_7( TEXT, VAL1, VAL2, VAL3, VAL4, VAL5, VAL6, VAL7 ) /**/
#endif // M2G_TRACE

// DATA TYPES
typedef TInt TM2GSvgEngineHandle;
typedef TInt TM2GSvgDocumentHandle;
typedef TInt TM2GSvgElementHandle;
typedef TInt TM2GBitmapHandle;
typedef TInt TM2GSvgPathHandle;
typedef TInt16 TM2GSvgAttrType;
typedef TUint8 TM2GPixelComponent;

//For Qimage related changes
typedef TUint TM2GSvgBitmapHandle;

typedef TFixedArray<TInt, 3> TM2GColorData;
typedef TFixedArray<TReal32, 6> TM2GMatrixData;
typedef M2GGeneral::TDataArrayPtr<TReal32> TM2GMatrixData2;
typedef TFixedArray<TReal32, 4> TM2GRectData;
typedef TFixedArray<TReal32, 4> TM2GScreenBBoxData;
typedef TFixedArray<TReal32, 6> TM2GPathCurveData;


M2G_NS_END

//#if defined( __cplusplus )
//} // extern "C"
//#endif

#endif // M2GGENERAL_H
