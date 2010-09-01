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
* Description:  Request used to write or read data
*
*/

#ifndef CMMASTREAMREQUEST_H
#define CMMASTREAMREQUEST_H

//  INCLUDES
#include <e32base.h>
#include "MMMAStreamRequestListener.h"

// CONSTANTS
const TInt KMMAStreamRequestBufferSize = 5120;

/**
 * - If CActive::IsActive returns ETrue, data is procedded in the server.
 * - If CActive::IsActive returns EFalse and DataPtr is empty request is
 *   ready for reuse
 * - If CActive::IsActive returns EFalse and DataPtr is not empty request
 *   is waiting to be written.
 */
class CMMAStreamRequest: public CActive
{
public:// Constructor and destructor
    /**
     * @param aListener will be informed when request completes
     */
    static CMMAStreamRequest* NewLC(MMMAStreamRequestListener* aListener);

    /**
     * Destructor
     */
    ~CMMAStreamRequest();

public: // new methods
    /**
     * @return TPtr to request's data
     */
    TPtr8& DataPtr();

    /**
     * Completes read request.
     * @param aError system error code
     */
    void CompleteRead(TInt aError);

    /**
     * MMMAStreamRequestListener will be informed when request is ready.
     */
    void SetActive();

    /**
     * 1 if this is stream's last buffer
     * @return reference to request buffer
     */
    TPckgBuf< TInt >& RequestBuffer();
public: // From CActive
    void RunL();
    TInt RunError(TInt aError);
    void DoCancel();

private: // Constructor
    CMMAStreamRequest(MMMAStreamRequestListener* aListener);
    void ConstructL();
private: // Data
    // will be informed when request is complete
    MMMAStreamRequestListener* iListener;

    // owned data
    HBufC8* iData;

    // ptr to iData or NULL if data is processed.
    TPtr8 iDataPtr;

    // Request buffer
    TPckgBuf< TInt > iRequestBuffer;
};

#endif // CMMASTREAMREQUEST_H
