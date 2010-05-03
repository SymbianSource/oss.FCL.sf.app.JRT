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
* Description:  Used to inform when CMMAStreamRequest completes
*
*/

#ifndef MMMASTREAMREQUESTLISTENER_H
#define MMMASTREAMREQUESTLISTENER_H


//  INCLUDES
class CMMAStreamRequest;

/**
 * MMMAStreamRequestListener will be informed when CMMAStreamRequest is
 * completed.
 */
NONSHARABLE_CLASS(MMMAStreamRequestListener)
{
public:
    /**
     * Called when when stream write request is completed.
     * @param aRequest request completed
     */
    virtual void WriteComplete(CMMAStreamRequest* aRequest) = 0;

    /**
     * Called when when stream read request is completed.
     * @param aRequest request completed
     */
    virtual void ReadComplete(CMMAStreamRequest* aRequest) = 0;

    /**
     * Called when when stream request is completed.
     * @param aError system error code
     */
    virtual void HandleError(CMMAStreamRequest* aRequest,
                             TInt aError) = 0;

};

#endif // MMMASTREAMREQUESTLISTENER_H
