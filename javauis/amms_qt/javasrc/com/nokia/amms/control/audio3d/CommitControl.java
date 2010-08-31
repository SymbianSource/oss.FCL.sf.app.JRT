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

package com.nokia.amms.control.audio3d;

import com.nokia.microedition.media.NativeError;

/**
 * Please refer to JSR 234 for more details.
 */
public class CommitControl
        extends com.nokia.amms.control.ControlImpl
        implements javax.microedition.amms.control.audio3d.CommitControl
{
    // data
    private boolean iIsDeferred = false;

    /**
     * Constructor
     */
    public CommitControl()
    {
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public void setDeferred(boolean aDeferred)
    {
        checkValid();

        int err = _setDeferred(iEventSource, iControlHandle, aDeferred);

        // check error
        NativeError.check(err);

        iIsDeferred = aDeferred;
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public boolean isDeferred()
    {
        checkValid();

        return iIsDeferred;
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public void commit()
    {
        checkValid();

        int err = _commit(iEventSource, iControlHandle);

        // check error
        NativeError.check(err);
    }

    // native function
    private static native int _setDeferred(int aEventSource, int aControlHandle, boolean aDeferred);
    private static native int _commit(int aEventSource, int aControlHandle);
}
