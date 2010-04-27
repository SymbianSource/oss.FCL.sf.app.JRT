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
* Description:  Streams data from Java to controller
*
*/

#ifndef CMMASTREAMHANDLER_H
#define CMMASTREAMHANDLER_H

//  INCLUDES
#include "cmmastreamrequest.h"
#include "mmmastreamrequestlistener.h"
#include "mmmastreamhandlerlistener.h"
#include <MDUChunkDataObserver.h>
#include <MetaDataUtility.h>

//EMC
#include "StreamControl.h"
#include "MMControlFactory.h"
#include "DataBufferSource.h"
#include "DataBuffer.h"
#include "SinkControl.h"
#include "StreamControlObserver.h"
#include "SourceControlObserver.h"


using namespace multimedia;
using multimedia ::MStreamControl;
using multimedia ::MDataBufferSource;
using multimedia ::MDataBuffer;
using multimedia ::CMultimediaFactory;
using multimedia ::MSinkControl;
using multimedia::MSourceControlObserver;
using multimedia::MStreamControlObserver;
using multimedia::MBufferProcessedEvent;

//End EMC


// CONSTANTS
const TInt KMMAStreamHandlerBufferCount = 2;

class CMMADataSourceStream;

//  CLASS DECLARATION
/**
* This class read data from CMMADataSourceStream and writes it to
* controller.
*/
NONSHARABLE_CLASS(CMMAStreamHandler): public CBase, public MControlObserver,
        public MMMAStreamRequestListener, public MMDUChunkDataObserver
{
private:
    // Streams internal state
    enum TMMAStreamState
    {
        EMMAStreamPrepare = 0,
        EMMAStreamPaused,
        EMMAStreamStart,
        EMMAStreamStarted,
        EMMAStreamEof
    };

    // chunk data processed state
    enum TMMAProcessedState
    {
        EMMANoneProcessed = 0,
        EMMAMetaDataProcessed,
        EMMABufferProcessed,
        EMMABothProcessed
    };

public: // Construction
    /**
    * Creates new player.
    */
    static CMMAStreamHandler* NewL(
        MMMAStreamHandlerListener& aListener,
        MStreamControl& aMStreamControl,
        MDataBufferSource& aMDataBufferSource,
        CMultimediaFactory& aFactory,
        CMetaDataUtility& aMetaDataUtility);

    //   Destructor
    ~CMMAStreamHandler();

protected:
    //   C++ constructor
    CMMAStreamHandler(MMMAStreamHandlerListener& aResolver,
                      MStreamControl& aMStreamControl,
                      MDataBufferSource& aMDataBufferSource,
                      CMultimediaFactory& aFactory,
                      CMetaDataUtility& aMetaDataUtility);
    void ConstructL();

protected: // New methods

public: // New methods
    /**
     * Prepares stream to play.
     */
    void Prepare(const TDesC8& aMimeType);

    /**
     * Stops handling requests. After this call data received from source
     * stream will be buffered until StartL is called.
     */
    void Pause();

    /**
             * Stops handling requests and frees already read data
             * without playing it.
             */
    void Stop();

    /**
     * Starts handling requests
     */
    void Start();

    /**
     * Sets source stream to read
     */
    void SetSourceStream(CMMADataSourceStream* aSourceStream);

public: // From MMMAStreamRequestListener
    void WriteComplete(CMMAStreamRequest* aRequest);
    void ReadComplete(CMMAStreamRequest* aRequest);
    void HandleError(CMMAStreamRequest* aRequest,
                     TInt aError);

public: // from MControlObserver
    void Event(MControl* aControl,
               TUint aEventType,
               TAny* aEventObject);
    TBool LastBufferWritten();

public: // from MMDUChunkDataObserver
    void HandleChunkDataProcessed(TInt aError);
    void HandleChunkDataReadyToBeParsed();
    void HandleChunkDataComplete(TInt aError);

private:
    /**
     * Writes request to controller
     */
    void WriteRequest(CMMAStreamRequest* aRequest);

private: // data
    // Owned write requests
    RPointerArray< CMMAStreamRequest > iRequests;

    CMMAStreamRequest* iCurrentRequest;

    // not owned source stream
    CMMADataSourceStream* iSourceStream;

    // EMC
    // not owned
    MStreamControl& iMStreamControl;
    MDataBufferSource& iMDataBufferSource;
    CMultimediaFactory& iFactory;

    // owned
    MDataBuffer *iBuffer;
    TBool iLastBufferWritten;
    //

    // not owned listener
    MMMAStreamHandlerListener& iListener;

    // not owned
    CMetaDataUtility& iMetaDataUtility;

    // Stream's state
    TMMAStreamState iState;

    // Processed state of current chunk data
    TMMAProcessedState iProcessedState;

    // tells if metadata parsing can be started
    TBool iMetaDataReadyToBeParsed;

    // to be removed once MDU supports all the reqd formats
    TBool iMimeTypeSupportedByMDU;
};

#endif // CMMASTREAMHANDLER_H
