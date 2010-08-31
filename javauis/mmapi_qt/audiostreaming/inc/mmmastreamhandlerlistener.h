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
* Description:  Listener for CMMAStreamHandler
*
*/

#ifndef MMMASTREAMHANDLERLISTENER_H
#define MMMASTREAMHANDLERLISTENER_H

/**
 * Listener for CMMAStreamHandler.
 */
NONSHARABLE_CLASS(MMMAStreamHandlerListener)
{
public:
    /**
     * Called when stream is prepared.
     * @param aError system error code
     */
    virtual void PrepareComplete(TInt aError) = 0;

    /**
     * Called when stream is started.
     * @param aError system error code
     */
    virtual void StartComplete(TInt aError) = 0;

    /**
     * Called when error need to be handled.
     * If stream is starting or preparing PrepareComplete or
     * StartComplete will be called instead of this method.
     * @param aError system error code
     */
    virtual void HandleError(TInt aError) = 0;
};


#endif // MMMASTREAMHANDLERLISTENER_H
