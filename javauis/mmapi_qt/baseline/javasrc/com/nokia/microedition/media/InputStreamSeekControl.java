/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class is used to mark and seek the inputstream. At present it is
*                implemented to mark and seek to the front of the stream and not anywhere
*                in the middle as the use case only demands such a beahaviour.
*
*/


package com.nokia.microedition.media;

import java.io.IOException;
import java.io.InputStream;
import com.nokia.mj.impl.utils.Logger;

/**
 * Class InputStreamSeekControl used to mark and seek the inputstream
 */
public class InputStreamSeekControl implements SeekControl
{


    private InputStream iInputStream;

    /**
     * Constructor
     */
    public InputStreamSeekControl()
    {

        iInputStream = null;
    }

    /*
     * Constructor
     * @param aInputStream - inputstream used by the InputStream class
     *                       that needs to be marked and streamed
     */
    public InputStreamSeekControl(InputStream aInputStream)
    {

        iInputStream = aInputStream;

        if (iInputStream.markSupported() == true)
        {
            iInputStream.mark(0);
        }
    }

    /*
     * Method seeks to the start of the inputstream.
     * @param aWhere - is ignored as the seeking takes place to
     *                 the begining always
     */
    public void seek(int aWhere) throws IOException
    {
        Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"MMA::InputStreamSeekControl - seek +");

        if (iInputStream.markSupported() == true)
        {
            iInputStream.reset();
        }
        Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"MMA::InputStreamSeekControl - seek -");
    }

    public void close()
    {

        // intentionally left blank
    }

}
