/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Thread that calls SeekControl's seek method.
*
*/

package com.nokia.microedition.media.protocol;

import com.nokia.microedition.media.SeekControl;
import com.nokia.mj.impl.utils.Logger;

/**
 * Thread that calls SeekControl's seek method.
 */
class SeekThread extends Thread
{
    // control to call in run method
    final private SeekControl iSeekControl;

    // object to notify when seek is ready
    final private Object iWaitObject;

    /**
     * Public constructor.
     * @param aWwaitObject Object to notify when ready
     * @param aSeekControl Control to seek.
     */
    public SeekThread(Object aWaitObject,
                      SeekControl aSeekControl)
    {
        iWaitObject = aWaitObject;
        iSeekControl = aSeekControl;
    }

    /**
     * From Thread class.
     */
    public void run()
    {
        Logger.ELOG(Logger.EJavaMMAPI,
                    "MMA::seekThread run +");
        try
        {
            iSeekControl.seek(0);
            Logger.ELOG(Logger.EJavaMMAPI,
                        "MMA::seekThread run after iSeekControl.seek");
        }
        catch (Exception e)
        {
            // If seek fails, stream cannot be read and SeekControl's target
            // read must return -1 or throw an exception.
            Logger.ELOG(Logger.EJavaMMAPI,
                        "MMA::SourceStreamReader::read seek exception ", e);
        }

        // notify that seek is ready
        synchronized (iWaitObject)
        {
            Logger.ELOG(Logger.EJavaMMAPI,
                        "MMA::seekThread run before iWaitObject.notify()");
            iWaitObject.notify();
            Logger.ELOG(Logger.EJavaMMAPI,
                        "MMA::seekThread run -");
        }
    }
}
