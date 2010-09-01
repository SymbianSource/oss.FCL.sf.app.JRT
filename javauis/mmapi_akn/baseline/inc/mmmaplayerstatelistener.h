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
* Description:  This interface is used for informing player state changes
*
*/


#ifndef MMMAPLAYERSTATELISTENER_H
#define MMMAPLAYERSTATELISTENER_H

#include <e32def.h>

//  CLASS DEFINITION
/**

  This interface is used for informing player state changes

*/
NONSHARABLE_CLASS(MMMAPlayerStateListener)
{
public:
    /**
     * Called when player state is changed. Used states are defined in
     * the player header file. Enumeration is not used to provide
     * expandability.
     *
     * @param aState New state of the player.
     */
    virtual void StateChanged(TInt aState) = 0;

protected:  // Constructors and destructors
    /**
     * May not delete through this interface,
     * because destructor is protected.
     */
    virtual ~MMMAPlayerStateListener()
    {
    };
};


#endif // MMMAPLAYERSTATELISTENER_H
