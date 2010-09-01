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

#ifndef CMIDBUFFER_H
#define CMIDBUFFER_H

#include <e32def.h>
#include <jni.h>
#include <mevents.h>
#include "lcdui.h"
#include "javax_microedition_lcdui_Buffer.h"


/**
 * Buffer for graphics (and any other) commands.
 *
 */
class CMIDBuffer : public CJavaEventSource<CMIDBuffer>
{
public:
    /**
     * Ctor
     * @param aJni JNI env
     * @param aPeer Java object
     * @param aServer Event server
     * @return Handle
     */
    static TInt New(JNIEnv& aJni, jobject aPeer, TJavaEventServer aServer);

    /**
     * Ctor
     */
    CMIDBuffer();

    /**
     * Dtor
     */
    virtual ~CMIDBuffer();

    /**
     * This method is called from Java threads. The Java peer class guarantees to
     * to write aCount words into the buffer before calling this method, with a
     * monitor lock held in java.
     * @param aEnv JNI env
     * @param aBuffer Buffer
     * @param aCount Buffer size
     */
    void RequestProcess(JNIEnv& aEnv, jintArray aBuffer, jint aCount);

private:
    /**
     * With help of this method the buffer is executed under lcdui event server.
     * @param aBuffer Buffer
     */
#ifdef RD_JAVA_NGA_ENABLED
    static TBool DoProcess(CMIDBuffer* aBuffer);
#else
    static void DoProcessL(CMIDBuffer* aBuffer);
#endif

    /**
     * Ctor
     * @param aJni JNI env
     * @param aPeer Java object
     * @param aServer Event server
     */
    void ConstructJniL(JNIEnv& aJni, jobject aPeer, TJavaEventServer aServer);

    /**
     * Finalizer. Must be called with the Toolkit lock held.
     * @param aJni JNI env
     */
    void FinalizeJni(JNIEnv& aJni);

    /**
     * Do the buffer processing under lcdui event server.
     */
#ifdef RD_JAVA_NGA_ENABLED
    TBool ProcessL();
#else
    void ProcessL();
#endif

    /**
     * Do the buffer processing under lcdui event server.
     */
    TInt Error(TInt aError);

    /**
     * Do the buffer processing under lcdui event server.
     */
    void Cancel();

    /**
     * Do the buffer processing under lcdui event server.
     */
    void Complete();

    /**
     * Buffer size
     * @return Buffer size
     */
    TInt Size() const;

private:
    enum { KTotalCycles = 65536 };

    /**
     * Processor state at start of RunL
     */
    enum TState
    {
        EComplete     = 0,
        ENewRequest   = 1,
        EAsyncPending = 3,
        EDeferred     = 4,
        EClosed       = -1,
        ECancelled    = -2
    };

    enum { KBufferSize = javax_microedition_lcdui_Buffer_BUFFER_SIZE };

private:
    MMIDBufferProcessor*    iProcessor;
    TMIDBufferOp            iBuffer[KBufferSize];
    const TMIDBufferOp*     iBufferEnd;
    const TMIDBufferOp*     iRead;
    const TMIDBufferOp*     iReadEnd;
    const TMIDBufferOp*     iProcEnd;
    TInt                    iCycles;
    TState                  iState;
    TInt                    iError;
#ifdef RD_JAVA_NGA_ENABLED
    java::util::Monitor*    iMonitor;
#endif
};

#endif // CMIDBUFFER_H
