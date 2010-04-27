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
#include <mmf/common/mmfcontroller.h>
#include "CMMAStreamRequest.h"
#include "MMMAStreamRequestListener.h"
#include "MMMAStreamHandlerListener.h"


// CONSTANTS
const TInt KMMAStreamHandlerBufferCount = 2;

class CMMADataSourceStream;

//  CLASS DECLARATION
/**
* This class read data from CMMADataSourceStream and writes it to
* controller.
*/
NONSHARABLE_CLASS(CMMAStreamHandler): public CBase,
        public MMMAStreamRequestListener
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

public: // Construction
    /**
     * Creates new player.
     */
    static CMMAStreamHandler* NewL(
        MMMAStreamHandlerListener& aListener,
        RMMFController& aController);

    //   Destructor
    ~CMMAStreamHandler();

protected:
    //   C++ constructor
    CMMAStreamHandler(MMMAStreamHandlerListener& aResolver,
                      RMMFController& aController);
    void ConstructL();

protected: // New methods

public: // New methods
    /**
     * Prepares stream to play.
     */
    void PrepareL();

    /**
     * Stops handling requests. After this call data received from source
     * stream will be buffered until StartL is called.
     */
    void Pause();

    /**
     * Starts handling requests
     */
    void StartL();

    /**
     * Data sink destination must be written to this reference
     * before stream can be started.
     * @return data sink destination reference
     */
    TMMFMessageDestination& MessageDestination();

    /**
     * Sets source stream to read
     */
    void SetSourceStream(CMMADataSourceStream* aSourceStream);

public: // From MMMAStreamRequestListener
    void WriteComplete(CMMAStreamRequest* aRequest);
    void ReadComplete(CMMAStreamRequest* aRequest);
    void HandleError(CMMAStreamRequest* aRequest,
                     TInt aError);
private:
    /**
     * Writes request to controller
     */
    void WriteRequest(CMMAStreamRequest* aRequest);

private: // data
    // Owned write requests
    RPointerArray< CMMAStreamRequest > iRequests;

    // not owned source stream
    CMMADataSourceStream* iSourceStream;

    // Controller to write stream
    RMMFController& iController;

    // added data source
    TMMFMessageDestination iDataSourceHandle;

    // not owned listener
    MMMAStreamHandlerListener& iListener;

    // Stream's state
    TMMAStreamState iState;
};

#endif // CMMASTREAMHANDLER_H
