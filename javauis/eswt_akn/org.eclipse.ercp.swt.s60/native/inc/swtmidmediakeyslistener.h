/*******************************************************************************
 * Copyright (c) 2007, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/
 

#ifndef SWTMIDMEDIAKEYSLISTENER_H
#define SWTMIDMEDIAKEYSLISTENER_H

#include <e32def.h>
#include <w32std.h>

/**
 * Interface class for receiving media key events.
 */
class MSwtMediaKeysListener
{
public:

    /**
     * Key codes for media keys.
     */
    enum TMediaKey
    {
        ESwtMediaKeyNone        = 0,
        ESwtMediaKeyPlay        = -20,
        ESwtMediaKeyPrevious    = -21,
        ESwtMediaKeyNext        = -22,
        ESwtMediaKeyStop        = -23
    };

    /**
     * This method is called when a media key has been pressed.
     *
     * @param aKeyEvent     The key event
     */
    virtual void HandleMediaKeyEvent(TKeyEvent& aKeyEvent, TInt aEventType) = 0;

};

#endif // SWTMIDMEDIAKEYSLISTENER_H

// End of File
