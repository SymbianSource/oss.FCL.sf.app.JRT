/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.mms;

import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.rt.support.Jvm;
/**
 * The singleton MmsRetriever object used to retrieve MaxMMSSize,check for valid
 * encoding and to retrieve the MMSC from the Native MMS engine.
 */

public final class MmsPropertyRetriever
{
    static
    {
        try
        {
            Jvm.loadSystemLibrary("javawmamms");
        }
        catch (Exception e)
        {
            Logger.LOG(Logger.EWMA, Logger.EInfo, e.toString());
        }

    }

    // THE MAX SIZE used for building messages on this connection
    private static int iMMSMaxSize = 0;

    /**
     * Validates an encoding
     * @returns boolean value which indicates whether valid encoding or not
     */
    static boolean isValidEncoding(String encoding)
    {
        return _isValidEncoding(encoding);
    }

    /**
     * Method for reteiving the MMSC
     * @returns the service centre address
     */
    public static String getMMSC()
    {
        String mmsc = null;
        mmsc = _getMMSC();
        if ((mmsc != null) && (0 == mmsc.length()))
        {
            mmsc = null;
        }
        return mmsc;
    }

    /**
     * Get method for the maxSize
     */

    static int getMMSMaxSize()
    {
        if (iMMSMaxSize == 0)
        {
            iMMSMaxSize = _maxMMSSize();
        }
        return iMMSMaxSize;
    }


    private static native String _getMMSC();

    private static native int _maxMMSSize();

    private static native boolean _isValidEncoding(String aEncoding);
}