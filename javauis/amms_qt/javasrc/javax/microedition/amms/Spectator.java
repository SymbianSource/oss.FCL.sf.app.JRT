/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
package javax.microedition.amms;

import javax.microedition.media.Controllable;
import javax.microedition.media.Control;
import javax.microedition.amms.control.audio3d.LocationControl;


/**
 * Please refer JSR 234 for more details.
 *
 */
public class Spectator implements Controllable
{
    /**
     * All method calls are delegated to this Controllable object
     */
    private Controllable iControllable;

    /**
     * Package private constructor. Can be used only from GlobalManager.
     * @param aControllable Used to delegate all operations.
     */
    Spectator(Controllable aControllable)
    {
        iControllable = aControllable;
    }

    /**
     * From JSR. This is never called. Added to state that constructor
     * is private.
     */
    private Spectator()
    {
    }

    /**
     * From JSR
     */
    public Control getControl(java.lang.String aControlType)
    {
        return iControllable.getControl(aControlType);
    }

    /**
     * From JSR
     */
    public Control[] getControls()
    {
        return iControllable.getControls();
    }

}
