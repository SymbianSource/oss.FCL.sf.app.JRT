/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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

#include "CMIDBuffer.h"
#include "CMIDToolkit.h"
#ifdef RD_JAVA_NGA_ENABLED
#include "monitor.h"
#endif
#include <jdebug.h>

#if defined(_DEBUG) && defined(_TRACE)
#define TRACE(x) RDebug::Print(_L(x))
#else
#define TRACE(x)
#endif

enum TBufferPriority
{
    EBufferPriorityNormal=0,
    EBufferPriorityHigh=20,
    EBufferPriorityRedraw=50,    // == EActivePriorityRedrawEvents
    EBufferPriorityInput=100    // == EActivePriorityWsEvents -> could starve key events.
};


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CMIDBuffer::New(JNIEnv& aJni, jobject aPeer, TJavaEventServer aServer)
{
    TInt handle;
    TRAP(handle,
         TConstructor self(aJni);
         self->ConstructJniL(aJni,aPeer,aServer);
         handle = self.GetHandle();
        );
    return handle;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CMIDBuffer::CMIDBuffer()
        : iState(EClosed)
{
    iBufferEnd = &iBuffer[ KBufferSize ];
    iRead      = iBuffer;
    iReadEnd   = iBuffer;
    iProcEnd   = iBuffer;
    iState = EComplete;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CMIDBuffer::~CMIDBuffer()
{
#ifdef RD_JAVA_NGA_ENABLED
    delete iMonitor;
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CMIDBuffer::RequestProcess(JNIEnv& aEnv, jintArray aBuffer, TInt aCount)
{
    ASSERT(aCount <= Size());

    aEnv.GetIntArrayRegion(aBuffer, 0, aCount, reinterpret_cast<jint*>(iBuffer));
    iRead    = &iBuffer[0];
    iReadEnd = iRead + aCount;
    iProcEnd = iRead;
    iState   = ENewRequest;
#ifdef RD_JAVA_NGA_ENABLED
    TBool async = Execute(CMIDBuffer::DoProcess, this);
    if (async)
    {
        iMonitor->wait();
    }
#else
    ExecuteTrap(CMIDBuffer::DoProcessL, this);
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
#ifdef RD_JAVA_NGA_ENABLED
TBool CMIDBuffer::DoProcess(CMIDBuffer* aBuffer)
{
    TBool ret = EFalse;
    TRAPD(err, ret = aBuffer->ProcessL());
    if (err != KErrNone)
    {
        ret = EFalse;
    }
    return ret;
}
#else
void CMIDBuffer::DoProcessL(CMIDBuffer* aBuffer)
{
    aBuffer->ProcessL();
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CMIDBuffer::ConstructJniL(JNIEnv& aJni, jobject aPeer, TJavaEventServer aServer)
{
    CJavaEventSourceBase::ConstructL(aJni, aPeer, aServer);
#ifdef RD_JAVA_NGA_ENABLED
    try
    {
        iMonitor = java::util::Monitor::createMonitor();
    }
    catch (std::exception& e)
    {
        User::Leave(KErrNoMemory);
    }
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CMIDBuffer::FinalizeJni(JNIEnv& /*aEnv*/)
{
    ASSERT(iState == EComplete);
    iState = EClosed;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
#ifdef RD_JAVA_NGA_ENABLED
TBool CMIDBuffer::ProcessL()
#else
void CMIDBuffer::ProcessL()
#endif
{
    iCycles = KTotalCycles;

#ifdef RD_JAVA_NGA_ENABLED
    TBool ret = EFalse;
#endif

    if (!iRead->IsActivate())
    {
        ELOG1(EJavaUI, "Buffer didn't start with header! %X", iRead->Header());
        User::Leave(KErrCorrupt);
    }
    while (iRead < iReadEnd)
    {
        const TMIDBufferOp* blockEnd = NULL;

        // Either setup a new buffer processor, or continue the current
        // processor after a defer, or a leave.
        if (iRead->IsActivate())
        {
            MMIDComponent* component = MIDUnhand<MMIDComponent>(iRead->Header()&0x7fffffff);
            iProcessor = component->Processor();

            // Skip activation header.
            ++iRead;

            const TInt words = *reinterpret_cast<const TInt*>(iRead);

            // Skip activation length
            ++iRead;

            blockEnd = iRead + words;

            iProcEnd = blockEnd;

            ASSERT(iProcEnd <= iReadEnd);
        }

        if (iRead < iReadEnd)
        {
#ifdef RD_JAVA_NGA_ENABLED
            // SYNC or SYNC_RECT op codes are sent to only one object at a time,
            // so it is safe to have only one boolean indicating async operation.
            // Currently only Canvas does asyncronous flush.
            TBool async = iProcessor->ProcessL(iRead, iProcEnd, iCycles, iMonitor);
            ret |= async;
#else
            iProcessor->ProcessL(iRead, iProcEnd, iCycles);
#endif
        }

        // Must have reached end of processor range.
        ASSERT(iRead == iProcEnd);

        // Next command must be an activate or we must have reached
        // the end of the buffer.
        ASSERT((iRead == iReadEnd) || iRead->IsActivate());
    }

    Complete();

#ifdef RD_JAVA_NGA_ENABLED
    return ret;
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CMIDBuffer::Cancel()
{
    //
    // Cancel any async operation a processor may be doing.
    //
    if (iState == EAsyncPending)
    {
        if (iProcessor)
        {
            iProcessor->AbortAsync();
        }
    }
    iState = ECancelled;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CMIDBuffer::Complete()
{
    ASSERT(iRead == iReadEnd);
    iRead    = iBuffer;
    iReadEnd = iBuffer;
    iProcEnd = iBuffer;
    iState   = EComplete;
    iError   = KErrNone;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
inline TInt CMIDBuffer::Size() const
{
    return iBufferEnd - iBuffer;
}
