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

package com.nokia.mj.impl.sms;

import java.io.IOException;
import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.rt.support.Jvm;

/**
 * Provides some utilities to: Retrieve Service Centre Number.
 * @author Nokia Corporation
 * @see
 */
public final class SMSCRetriever
{

    static
    {
        try
        {
            Jvm.loadSystemLibrary("javawma");
        }
        catch (Exception e)
        {
            Logger.LOG(Logger.EWMA, Logger.EInfo, e.toString());
        }
    }

    /**
     * Method to get service centre number from native side
     * @return service centre number
     */
    public static String getSmscNum()
    {
        String smscNum = null;
        smscNum = _getSmscNum();
        if ((smscNum == null) || (0 == smscNum.length()))
        {
            smscNum = null;
        }
        return smscNum;
    }

    /**
     * Private native method that gets the Service Centre Number.
     *
     * @param nativeHandle
     *            handle to native side objects
     * @return Service Centre Number as string
     * @see
     */
    private native static String _getSmscNum();
}