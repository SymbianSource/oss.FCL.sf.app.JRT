/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

package com.nokia.amms;

import javax.microedition.media.Controllable;

/**
 * SpectatorImpl provides functionality defined in Spectator class.
 * This class is used through Controllable interface which methods are
 * implemented in ModuleBase.
 */
class SpectatorImpl extends ModuleBase implements Controllable
{
    /**
     * Constructs new SpectatorImpl instance.
     * @param aEventSourceHandle Handle to event source.
     * @param aNativeHandle Handle to corresponding native object.
     */
    SpectatorImpl(int aEventSourceHandle,
                  int aNativeHandle)
    {
        // use empty constructor which don't register this object for
        // finalization. Finalization is not needed because spectator's
        // is singleton class in the global manager.
        init(aEventSourceHandle,
             aNativeHandle);
    }


}
