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

import javax.microedition.amms.EffectModule;
import javax.microedition.media.Control;

/**
 * EffectModuleImpl provides functionality defined in EffectModule.
 * This class is used through EffectModule interface which implements the
 * Module interface and does not define any new methods. Methods defined in
 * Module are implemented in ModuleBase class.
 * Can be accessed only from com.nokia.amms package.
 */
class EffectModuleImpl extends ModuleBase
        implements EffectModule
{
    /**
     * Constructs new EffectModuleImpl instance.
     * Package private.
     * @param aEventSourceHandle Handle to event source.
     * @param aNativeHandle Handle to corresponding native object.
     * @param aManagerHandle Handle to native global manager.
     */
    EffectModuleImpl(int aEventSourceHandle,
                     int aNativeHandle,
                     int aManagerHandle)
    {
        super(aEventSourceHandle,
              aNativeHandle,
              aManagerHandle);
    }

    /**
     * Obtain the object that implements the specified Control interface.
     *
     * @see javax.microedition.media.Controllable
     * @param aControlType  the class name of the Control. The class name should
     * be given either as the fully-qualified name of the class; or if the
     * package of the class is not given, the package
     * javax.microedition.media.control is assumed.
     * @return the object that implements the control, or null if no objects
     * implement the control or if there is no players in the module.
     */
    public Control getControl(String aControlType)
    {
        if (iPlayers.size() > 0)
        {
            // Delegate to ControlContainer
            return iControls.getControl(aControlType);
        }
        // An EffectModule, that has no Players attached to it, does not
        // provide any Controls.
        else
        {
            return null;
        }
    }

    /**
     * Obtain the collection of Controls.
     *
     * @see javax.microedition.media.Controllable
     * @return the collection of Control objects or a zero length array if
     * there is no controls or players in the module.
     */
    public Control[] getControls()
    {
        if (iPlayers.size() > 0)
        {
            // Delegate to ControlContainer
            return iControls.getControls();
        }
        // An EffectModule, that has no Players attached to it, does not
        // provide any Controls.
        else
        {
            return new Control[ 0 ];
        }


    }
}
