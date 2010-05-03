/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class to handle OutputStream operations.
*
*/

#include <jdebug.h>
#include <JniEnvWrapper.h>
#include "cmmaoutputstream.h"
#include "mmmaeventposter.h"
#include "cmmaoutputstreamevent.h"

// CONSTANTS
const TInt KMMACommit = -10000; // indicates native started commit

CMMAOutputStream* CMMAOutputStream::NewL(JNIEnv* aJNIEnv,
        MMMAEventPoster* aEventPoster,
        jobject aJavaOutputStreamWriter
                                        )
{
    CMMAOutputStream* self = CMMAOutputStream::NewLC(aJNIEnv,
                             aEventPoster,
                             aJavaOutputStreamWriter
                                                    );
    CleanupStack::Pop();
    return self;
}


CMMAOutputStream* CMMAOutputStream::NewLC(JNIEnv* aJNIEnv,
        MMMAEventPoster* aEventPoster,
        jobject aJavaOutputStreamWriter
                                         )
{
    CMMAOutputStream* self = new(ELeave) CMMAOutputStream(aEventPoster);
    CleanupStack::PushL(self);
    self->ConstructL(aJNIEnv, aJavaOutputStreamWriter);
    return self;
}


void CMMAOutputStream::CreateL(CMMAOutputStream** aOutputStream,
                               JNIEnv* aJniEnv,
                               MMMAEventPoster* aEventPoster,
                               jobject aJavaOutputStreamWriter)
{

    // JNI interface pointer can't be passed to different thread, so
    // it is needed to get valid JNI interface pointer for Event Server thread
    aJniEnv = JniEnvWrapper::GetValidJniRef();

    *aOutputStream = NewL(aJniEnv, aEventPoster, aJavaOutputStreamWriter);
}


CMMAOutputStream::~CMMAOutputStream()
{
    DEBUG("MMA::CMMAOutputStream::~");

    // If write event is in the event server, it cannot be deleted.
    // Thus set the event to be deleted when event dispatch is called.
    if (iWriteEvent &&
            iWriteEvent->State() == CMMAOutputStreamEvent::EMMAEventActive)
    {
        iWriteEvent->SetState(CMMAOutputStreamEvent::EMMADeleteEvent);
    }
    else
    {
        delete iWriteEvent;
    }

    delete iData;
    DEBUG("MMA::CMMAOutputStream::~ OK");
}


// Default constructor
CMMAOutputStream::CMMAOutputStream(MMMAEventPoster* aEventPoster)
        : iEventSource(aEventPoster),
        iPtr(NULL, 0)
{
    DEBUG("MMA::CMMAOutputStream constructed");
}


void CMMAOutputStream::ConstructL(JNIEnv* aJNIEnv,
                                  jobject aJavaOutputStreamWriter)
{
    DEBUG("CMMAOutputStream::ConstructL()");

    // void write( int aLength, int aStatus ) method in OutputStreamWriter
    jmethodID classMethodID =
        aJNIEnv->GetMethodID(
            aJNIEnv->GetObjectClass(aJavaOutputStreamWriter),
            "write",
            "(II)V");
    if (!classMethodID)
    {
        DEBUG("CMMAOutputStream::ConstructL: Cannot find java method");
        User::Panic(_L("Java method write(II)V not found"), KErrGeneral);
    }

    iWriteEvent = new(ELeave) CMMAOutputStreamEvent(classMethodID,
            aJavaOutputStreamWriter);
}

void CMMAOutputStream::ReadDataL(TUint8* aOutputData,
                                 TInt* aBufferSize,
                                 TInt* aReadStatus)
{
    if (!iData)
    {
        User::Leave(KErrNotReady);
    }

    // Status code to be returned
    TInt status = KErrNone;

    TPtr8 buffer(aOutputData, *aBufferSize);
    DEBUG_INT(" MMA::CMMAOutputStream::ReadDataL SIZE BEFORE READL %d", buffer.Length());
    DEBUG_INT(" MMA::CMMAOutputStream::ReadDataL BYTES AVAILABLE %d", iPtr.Length());

    // Bytes available in native buffer
    TInt bytesAvailable = iPtr.Length();

    // Maximum size that can be written to Java buffer
    TInt outputMaxLength = buffer.MaxLength();

    // Length that can be written
    TInt writeLength = 0;

    if (outputMaxLength < bytesAvailable)
    {
        // Not all bytes can be written Java buffer
        writeLength = outputMaxLength;

        // Java need to read more data
        status = EMoreData;
    }
    else
    {
        // All bytes can be written to Java buffer
        writeLength = bytesAvailable;

        // All data is copied
        status = ECompleted;
    }

    // Copy maximum number of bytes to Java buffer
    buffer.Copy(iPtr.Left(writeLength));
    *aBufferSize = buffer.Length();

    // Move pointer to next read position.
    iPtr = iPtr.Mid(writeLength);

    DEBUG_INT(" MMA::CMMAOutputStream::ReadDataL SIZE AFTER READL %d", buffer.Length());
    DEBUG_INT(" MMA::CMMAOutputStream::ReadDataL RETURN %d", status);
    DEBUG_INT(" MMA::CMMAOutputStream::ReadDataL dataLeft %d", iPtr.Length());

    if (iPtr.Length() == 0)
    {
        // All data is read and buffer is not needed anymore.
        delete iData;
        iData = NULL;
    }
    *aReadStatus = status;
}

void CMMAOutputStream::WriteL(const TDesC8& aData)
{
    DEBUG_INT("CMMAOutputStream::WriteL data size = %d", aData.Size());
    if (iData)
    {
        // Previous data was not readed from the stream.
        User::Leave(KErrNotReady);
    }

    if (aData.Length() > 0)
    {
        // Take a copy of new data.
        HBufC8* data = aData.AllocL();
        delete iData;
        iData = data;
        iPtr.Set(iData->Des());

        // Set java event
        DEBUG_INT("CMMAOutputStream::WriteL: available data: %d", iData->Length());
        iWriteEvent->SetLength(iData->Length());
        iWriteEvent->SetStatus(EMoreData);
    }
    else
    {
        DEBUG("CMMAOutputStream::WriteL Zero length data");
        iWriteEvent->SetLength(0);
        iWriteEvent->SetStatus(ECompleted);
    }
    iEventSource->PostEvent(iWriteEvent, CMMAEvent::ENotifyPriority);
}

void CMMAOutputStream::Commit()
{
    iWriteEvent->SetLength(0);
    iWriteEvent->SetStatus(KMMACommit);
    iEventSource->PostEvent(iWriteEvent, CMMAEvent::ENotifyPriority);
}

//  END OF FILE

