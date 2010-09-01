/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The class derived from this class handles the volume keys
*                update events
*
*
*/



#ifndef MMMAVOLUMEKEYSLISTENER_H
#define MMMAVOLUMEKEYSLISTENER_H

#include <e32def.h>

// CLASS DECLARATION
/**
*  The class derived from this class handles the volume keys
*  update events
*/
NONSHARABLE_CLASS(MMMAVolumeKeysListener)
{
public:  // Constructors and destructor

    //Virtual destructor, public allows delete
    virtual ~MMMAVolumeKeysListener() {};

public:  // Abstract methods
    /**
     * Called when volume key is clicked/pressed up
     */
    virtual void VolumeUp() = 0;

    /**
     * Called when volume key is clicked/pressed down
     */
    virtual void VolumeDown() = 0;
};

#endif // MMMAVOLUMEKEYSLISTENER_H

// End of File
