/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  General methods
*
*/

// INCLUDE FILES
#include <s32strm.h> // RWriteStream
#include "M2GUtils.h"

#ifdef M2G_INFO_POPUP
#include <eikenv.h> // CEikonEnv::InfoWinL
#endif

#include <jni.h>

#include <SvgJavaInterfaceImpl.h>
#include "M2GGeneral.h"


M2G_NS_START

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// -----------------------------------------------------------------------------
// M2GGeneral::RaiseException
// -----------------------------------------------------------------------------
void M2GGeneral::RaiseException(
    JNIEnv* aEnv, const TDesC8& aName, const TUint8* aMsg)
{
    M2G_DEBUG_0("#### M2G_DEBUG: M2GGeneral::RaiseException() ####");

    if (aEnv)
    {
        jclass clazz = aEnv->FindClass(
                           REINTERPRET_CAST(const char*, aName.Ptr()));

        if (!clazz)
        {
            // if exception class not found, then use java.lang.Exception
            clazz = aEnv->FindClass(
                        REINTERPRET_CAST(const char*, KJavaExceptionClass().Ptr()));
        }

        aEnv->ThrowNew(clazz, REINTERPRET_CAST(const char*, aMsg));
    }
}

// -----------------------------------------------------------------------------
// M2GGeneral::CheckErrorCode
// -----------------------------------------------------------------------------
TInt M2GGeneral::CheckErrorCode(JNIEnv* aEnv, TInt aErrorCode)
{
    M2G_DEBUG_1("M2G_DEBUG: M2GGeneral::CheckErrorCode: %d - begin", aErrorCode);
    TInt code = aErrorCode;
    const TUint8* msg = (STATIC_CAST(const TDesC8&, KM2GEmptyString8)).Ptr();
    jcfcommon::auto_ptr<MJavaError> error;

    // Check if an error object
    if (aErrorCode > 0)
    {
        M2G_DEBUG_0("M2G_DEBUG: M2GGeneral::CheckErrorCode(), is MJavaError");

        // Error object need to be deleted here and the auto_ptr takes care of that!!
        error.reset(JavaUnhand< MJavaError >(aErrorCode));

        // Get an error description through the MJavaError object
        msg = error->Description().Ptr();

        switch (error->ErrorCode())
        {
        case KM2GIOException:
        {
            code = error->ErrorCode();
        }
        default:
        {
            code = error->ErrorCode();
        }
        }
        M2G_DEBUG_1("M2G_DEBUG: M2GGeneral::CheckErrorCode(), MJavaError: error code=%d", code);
    }

    switch (code)
    {
    case KM2GMemoryNotOk:
    {
        M2G_DEBUG_0("#### M2G_DEBUG: M2GGeneral::CheckErrorCode() - OutOfMemoryError ####");
        M2GGeneral::RaiseException(aEnv, KJavaOutOfMemoryErrorClass, msg);
        return code;
    }
    case KM2GIOException:
    {
        M2G_DEBUG_0("#### M2G_DEBUG: M2GGeneral::CheckErrorCode() - IOException ####");
        M2GGeneral::RaiseException(aEnv, KJavaIOExceptionClass, msg);
        return code;
    }
    case KM2GHandleNotOk:
    {
        M2G_DEBUG_0("#### M2G_DEBUG: M2GGeneral::CheckErrorCode() - NullPointerException ####");
        M2GGeneral::RaiseException(aEnv, KJavaNullPointerException, msg);
        return code;
    }
    default:
    {
        M2G_DEBUG_1("M2G_DEBUG: M2GGeneral::CheckErrorCode() error code=%d - end", code);
        return code;
    }
    }
}

// -----------------------------------------------------------------------------
// M2GGeneral::CheckHandle
// -----------------------------------------------------------------------------
TInt M2GGeneral::CheckHandle(JNIEnv* aEnv, TInt aHandle)
{
    if (aHandle == M2G_INVALID_HANDLE)
    {
        M2GGeneral::RaiseException(aEnv, KJavaNullPointerException);
    }
    if (aHandle < 0)
    {
        M2GGeneral::CheckErrorCode(aEnv, aHandle);
    }
    return aHandle;
}

// -----------------------------------------------------------------------------
// M2GGeneral::CheckErrorCodeAndHandle
// -----------------------------------------------------------------------------
TInt M2GGeneral::CheckErrorCodeAndHandle(
    JNIEnv* aEnv, TInt aErrorCode, TInt aHandle, TInt aErrorResult)
{
    if ((aErrorCode == KM2GOk) && (aHandle != M2G_INVALID_HANDLE))
    {
        return aHandle;
    }
    if (aErrorCode != KM2GOk)
    {
        M2GGeneral::CheckErrorCode(aEnv, aErrorCode);
    }
    else if (aHandle == M2G_INVALID_HANDLE)
    {
        M2GGeneral::RaiseException(aEnv, KJavaOutOfMemoryErrorClass);
    }
    return aErrorResult;
}

// -----------------------------------------------------------------------------
// M2GGeneral::PopupInfo
// -----------------------------------------------------------------------------
#ifdef M2G_INFO_POPUP
void M2GGeneral::PopupInfo(
    const TDesC& aLine1,
    const TDesC& aLine2)
{
    TInt err = KM2GOk;
    TRAP(err, CEikonEnv::InfoWinL(aLine1, aLine2));
    if (err == KM2GOk)
    {
        // DUMMY
    }
}
#endif // M2G_INFO_POPUP


#ifdef M2G_STATISTIC_DEBUG
// -----------------------------------------------------------------------------
// M2GGeneral::TM2GStatistic::TM2GStatistic
// -----------------------------------------------------------------------------
M2GGeneral::TM2GStatistic::TM2GStatistic()
{
    Reset();
}

// -----------------------------------------------------------------------------
// M2GGeneral::TM2GStatistic::Reset()
// -----------------------------------------------------------------------------
void M2GGeneral::TM2GStatistic::Reset()
{
    // Time
    iStartTime.UniversalTime();
    iEndTime.UniversalTime();
    // Thread
    iId = RThread().Id();
    RThread thread;
    if (thread.Open(iId) == KM2GOk)
    {
        iThreadName = thread.Name();
        thread.Close();
    }
}

// -----------------------------------------------------------------------------
// M2GGeneral::TM2GStatistic::Print()
// -----------------------------------------------------------------------------
void M2GGeneral::TM2GStatistic::Print(TBool aPrintElapsedTime)
{
    TInt totalRom = 0;
    TInt totalRam = 0;
    TInt freeRam = 0;
    HAL::Get(HALData::EMemoryROM, totalRom);
    HAL::Get(HALData::EMemoryRAM, totalRam);
    HAL::Get(HALData::EMemoryRAMFree, freeRam);

    // Update time
    iEndTime.UniversalTime();

    M2G_DEBUG_0("====================================================");
    // Write memory info
    if (aPrintElapsedTime)
    {
        M2G_DEBUG_5(
            "Rom size:%d, ram size:%d, ram free:%d, ram used:%d, elapsed time:%d",
            totalRom >> 10,
            totalRam >> 10,
            freeRam >> 10,
            (totalRam - freeRam) >> 10,
            iEndTime.MicroSecondsFrom(iStartTime).Int64());
    }
    else
    {
        M2G_DEBUG_4(
            "Rom size:%d, ram size:%d, ram free:%d, ram used:%d",
            totalRom >> 10,
            totalRam >> 10,
            freeRam >> 10,
            (totalRam - freeRam) >> 10);
    }
    // Write thread info
    RThread thread;
    if (thread.Open(iId) == KM2GOk)
    {
        TThreadStackInfo stackInfo;
        thread.StackInfo(stackInfo);
        thread.Close();
        TUint id = iId;
        M2G_DEBUG_2("Thread id:%d, size:%d", id, (stackInfo.iBase - stackInfo.iLimit) >> 10);
    }
    M2G_DEBUG_0("====================================================");
}

// -----------------------------------------------------------------------------
// M2GGeneral::TM2GStatistic::Print()
// -----------------------------------------------------------------------------
void M2GGeneral::TM2GStatistic::Print(RWriteStream& /*aStream*/)
{
}

// -----------------------------------------------------------------------------
// M2GGeneral::TM2GStatistic::Log()
// -----------------------------------------------------------------------------
void M2GGeneral::TM2GStatistic::Log()
{
    M2G_DEBUG_0("M2GGeneral::TM2GStatistic::Log()");
    M2GGeneral::TM2GStatistic statistic;
    statistic.Print();
}

#endif // M2G_STATISTIC_DEBUG

M2G_NS_END
