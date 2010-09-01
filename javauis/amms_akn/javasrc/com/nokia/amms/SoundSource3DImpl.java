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

import javax.microedition.amms.SoundSource3D;

/**
 * SoundSource3DImpl provides functionality defined in SoundSource3D.
 * This class is used through SoundSource3D interface which implements the
 * Module interface and does not define any new methods. Methods defined in
 * Module are implemented in ModuleBase class.
 * Can be accessed only from com.nokia.amms package.
 */
class SoundSource3DImpl extends ModuleBase
        implements SoundSource3D
{
    /**
     * Constructs new EffectModuleImpl instance.
     * Package private.
     * @param aEventSourceHandle Handle to event source.
     * @param aNativeHandle Handle to corresponding native object.
     * @param aManagerHandle Handle to native global manager.
     */
    SoundSource3DImpl(int aEventSourceHandle,
                      int aNativeHandle,
                      int aManagerHandle)
    {
        super(aEventSourceHandle,
              aNativeHandle,
              aManagerHandle);
    }
}
