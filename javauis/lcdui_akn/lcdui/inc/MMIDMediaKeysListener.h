/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/


#ifndef MMIDMEDIAKEYSLISTENER_H
#define MMIDMEDIAKEYSLISTENER_H

#include <e32def.h>

// using TMIDKeyEvent as parameter of HandleMediaKeyEvent function
#include <lcdui.h>

/**
 * Interface class for receiving media key events.
 *
 * @since S60 3.2
 */
class MMIDMediaKeysListener
{
public:

    /**
     * Key codes for media keys.
     */
    enum TMediaKey
    {
        EMIDMediaKeyNone        = 0,
        EMIDMediaKeyPlay        = -20,
        EMIDMediaKeyPrevious    = -21,
        EMIDMediaKeyNext        = -22,
        EMIDMediaKeyStop        = -23
    };

    /**
     * This method is called when a media key has been pressed.
     *
     * @param aKeyEvent     The key event
     * @since S60 3.2
     */
    virtual void HandleMediaKeyEvent(TMIDKeyEvent& aKeyEvent) = 0;

};

#endif // MMIDMEDIAKEYSLISTENER_H

// End of File
