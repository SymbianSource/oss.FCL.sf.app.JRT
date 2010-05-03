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

package com.nokia.amms.control;

import com.nokia.mj.impl.rt.legacy.NativeError;

public class PanControl
        extends com.nokia.amms.control.ControlImpl
        implements javax.microedition.amms.control.PanControl
{
    private static final int MIN_PAN = -100;
    private static final int MAX_PAN = 100;

    /**
     * Constructor
     */
    public PanControl()
    {
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public int setPan(int aPan)
    {
        checkValid();

        int pan = aPan;
        // Panning value that is out of range is set to the valid range.
        if (pan < MIN_PAN)
        {
            pan = MIN_PAN;
        }
        if (pan > MAX_PAN)
        {
            pan = MAX_PAN;
        }

        // SetBalance method in native side returns a system wide error code.
        int[] error = new int[ 1 ];
        int value = _setPan(iEventSource, iControlHandle, pan, error);
        NativeError.check(error[ 0 ]);

        return value;
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public int getPan()
    {
        checkValid();

        int value = _getPan(iEventSource, iControlHandle);
        // GetBalance method in native would return a system wide error code,
        // but the panning value is just instead queried from the control group.

        return value;
    }

    // native methods
    private static native int _setPan(
        int aEventSource,
        int aControlHandle,
        int aPan,
        int[] aError);
    private static native int _getPan(
        int aEventSource,
        int aControlHandle);
}
