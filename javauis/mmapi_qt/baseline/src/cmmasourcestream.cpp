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
* Description:  Class for reading data from Java SourceStream to native side
*
*/

#include <logger.h>

#include <JniEnvWrapper.h>

#include "mmafunctionserver.h"
#include "cmmasourcestream.h"
#include "cmmasourcestreamevent.h"
#include "mmmasourcestreamlistener.h"

const TInt KBufferSize = 4096; // data is read from java in 4k pieces

// CONSTRUCTION
CMMASourceStream* CMMASourceStream::NewL(JNIEnv* aJNIEnv,
        MMMAEventPoster* aEventPoster,
        jobject aJavaSourceStream,
        MMMASourceStreamListener* aListener
                                        )
{
    CMMASourceStream* self = CMMASourceStream::NewLC(aJNIEnv,
                             aEventPoster,
                             aJavaSourceStream,
                             aListener);
    CleanupStack::Pop();
    return self;
}


CMMASourceStream* CMMASourceStream::NewLC(JNIEnv* aJNIEnv,
        MMMAEventPoster* aEventPoster,
        jobject aJavaSourceStream,
        MMMASourceStreamListener* aListener
                                         )
{
    CMMASourceStream* self = new(ELeave) CMMASourceStream(aEventPoster,
            aListener);
    CleanupStack::PushL(self);
    self->ConstructL(aJNIEnv, aJavaSourceStream);
    return self;
}


CMMASourceStream::~CMMASourceStream()
{
    LOG( EJavaMMAPI, EInfo, "MMA::CMMASourceStream::~");
    delete iData;
    delete iReadEvent;
}


// Default constructor
CMMASourceStream::CMMASourceStream(MMMAEventPoster* aEventPoster,
                                   MMMASourceStreamListener* aListener)
        : iEventPoster(aEventPoster), iListener(aListener)
{
}


void CMMASourceStream::ConstructL(JNIEnv* aJNIEnv,
                                  jobject aJavaSourceStream)
{
    // JNI interface pointer can't be passed to different thread, so
    // it is needed to get valid JNI interface pointer for Event Server thread
    aJNIEnv = iEventPoster->getValidJniEnv();
    jmethodID classMethodID =
        aJNIEnv->GetMethodID(aJNIEnv->GetObjectClass(aJavaSourceStream),
                             "read",
                             "(II)V");
    if (!classMethodID)
    {
        User::Leave(KErrNoMemory);
    }
	LOG( EJavaMMAPI, EInfo, "CMMASourceStream::ConstructL , jmethodId found");  
	jobject apeer =  aJNIEnv->NewGlobalRef(aJavaSourceStream);  
    iReadEvent = new(ELeave) CMMASourceStreamEvent(classMethodID,
            apeer);
}


void CMMASourceStream::WriteL(const TUint8* aData,
                              TInt aLength,
                              TInt aState)
{
	  LOG1( EJavaMMAPI, EInfo, "MMA::CMMASourceStream::WriteL() %d",aState);
    if (aState < KErrNone)
    {
    	LOG( EJavaMMAPI, EInfo, "MMA::CMMASourceStream::WriteL  : 1");
        // Inform listener on error case
        iListener->ReadCompletedL(aState, KNullDesC8());
        return;
    }


    if ((iData->Size() + aLength) > iData->Des().MaxSize())
    {
        // if data doesn't fit, reallocate more
        HBufC8* reallocated = iData->ReAlloc(iData->Des().MaxSize() + KBufferSize);
        if (!reallocated)  // realloc failed
        {
            // probably too much data read, so freeing used memory
            delete iData;
            iData = NULL;
            iListener->ReadCompletedL(KErrNoMemory, KNullDesC8());
            return;
        }
        iData = reallocated;
    }

    // java returns length -1 when completed
    if (aLength > 0)
    {
        iData->Des().Append(aData, aLength);
    }

    TInt currentRead = iData->Size() - iBufferPosition;

    if ((aState == ECompleted) ||
            (currentRead == iReadLength))
    {
        if (iReadLength == KMMAReadAllData)
        {
            iListener->ReadCompletedL(aState, *iData);
        }
        else
        {
            TPtrC8 data = iData->Mid(iBufferPosition, currentRead);
            iListener->ReadCompletedL(aState, data);
        }
    }
    else
    {
        // phase ready, inform the Player
        if (iReadLength == KMMAReadAllData)
        {
            iReadEvent->SetLength(KBufferSize);
        }
        else
        {
            iReadEvent->SetLength(iReadLength - currentRead);
        }

    }

    LOG( EJavaMMAPI, EInfo, "MMA::CMMASourceStream::WriteL completed");
}

void CMMASourceStream::ReadL(TInt aLength)
{
    LOG1( EJavaMMAPI, EInfo, "MMA::CMMASourceStream::ReadL(%d)", aLength);

    __ASSERT_DEBUG(iData != NULL, User::Invariant());

    PrepareReadL();

    iBufferPosition = iData->Size();
    iReadLength = aLength;

    iReadEvent->SetLength(aLength);

    // data has been requested, note will be sent
    iEventPoster->PostEvent(iReadEvent, CMMAEvent::ENotifyPriority);
}


void CMMASourceStream::ReadAllL()
{
    // not reading again if iData already has data
    if (iData == NULL)
    {
        LOG( EJavaMMAPI, EInfo, "CMMASourceStream::ReadAllL: Buffer empty, reading from java");
        CreateDataBufferL(KBufferSize);
        ReadL(KMMAReadAllData);
    }
    else
    {
        LOG( EJavaMMAPI, EInfo, "CMMASourceStream::ReadAllL: Buffer not empty, ->ReadCompleteL");
        iListener->ReadCompletedL(ECompleted, *iData);
    }
}

void CMMASourceStream::PrepareReadL()
{
}

void CMMASourceStream::ResetData()
{
    delete iData;
    iData = NULL;
}

void CMMASourceStream::CreateDataBufferL(TInt aBufferSize)
{
    LOG( EJavaMMAPI, EInfo, "CMMASourceStream::CreateDataBufferL +");
    __ASSERT_DEBUG(iData == NULL, User::Invariant());

    iData = HBufC8::NewL(aBufferSize);
}

//  END OF FILE
