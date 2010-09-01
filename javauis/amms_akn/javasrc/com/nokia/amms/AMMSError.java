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

import javax.microedition.media.MediaException;
import com.nokia.mj.impl.rt.legacy.NativeError;

/**
 * This class contains general helper methods for error conversion
 * between native side and java side.
 */
public final class AMMSError
{
    /**
     * Private, because this class is not intended to be constructed.
     */
    private AMMSError()
    {
    }

    /**
     * This method throws IllegalStateException if error code is
     * KErrNotReady (-18)
     *
     * @param aNativeErrorCode Native error code.
     */
    static public void checkIllegalState(int aNativeErrorCode)
    {
        if (aNativeErrorCode == NativeError.KErrNotReady)
        {
            throw new IllegalStateException();
        }
    }

    /**
     * This method throws MediaException if checked native error
     * code is below KErrNone.
     * @param aNativeErrorCode Native error code.
     */
    static public void checkMediaException(int aNativeErrorCode)
    throws MediaException
    {
        NativeError.checkOOMOnly(aNativeErrorCode);
        if (aNativeErrorCode < NativeError.KErrNone)
        {
            throw new MediaException();
        }
    }
    /**
     * @param aObject Object to be checked.
     */
    static public void checkArgument(Object aObject)
    {
        if (aObject == null)
        {
            throw new IllegalArgumentException();
        }
    }

    /**
     * Checks for basic native error codes that map to standard Java
     * exceptions and throws the exception if the error code matches.
     * Otherwise throws basic Error class.
     * @param aNativeErrorCode Native error code.
     */
    static public void check(int aNativeErrorCode)
    {
        NativeError.check(aNativeErrorCode);
    }
}
