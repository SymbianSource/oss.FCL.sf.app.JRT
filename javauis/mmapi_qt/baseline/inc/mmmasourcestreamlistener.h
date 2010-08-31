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
* Description:  This interface is used for informing player that
*                CMMASourceStream is ready
*
*/


#ifndef MMMASOURCESTREAMLISTENER_H
#define MMMASOURCESTREAMLISTENER_H

//  CLASS DEFINITION
/**

    This interface is used for informing player that CMMASourceStream is ready

*/
NONSHARABLE_CLASS(MMMASourceStreamListener)   // usually not derived
{
public: // New methods
    /**
     * This method will be called when source stream read operation
     * is ready or error occures.
     *
     * @param aStatus Status of the reading operation or one of the system
     *                error codes.
     * @param aData Reference to read data. If an error occured reference
     *              to zero length descriptor.
     */
    virtual void ReadCompletedL(TInt aStatus, const TDesC8& aData) = 0;

protected:  // Destructor
    /**
     * Do not allow delete trough this interface.
     */
    virtual ~MMMASourceStreamListener()
    {
    };
};

#endif // MMMASOURCESTREAMLISTENER_H
