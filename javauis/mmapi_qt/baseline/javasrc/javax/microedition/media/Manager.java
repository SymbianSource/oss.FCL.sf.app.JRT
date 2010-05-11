/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Please refer JSR 135 for more details. 
*/


package javax.microedition.media;

import java.io.IOException;
import com.nokia.microedition.media.ManagerImpl;
import javax.microedition.media.protocol.DataSource;
import java.io.InputStream;

public final class Manager
{
    private static ManagerImpl sInstance;

    static
    {
        sInstance = ManagerImpl.getInstance();
    }

    final public static String TONE_DEVICE_LOCATOR = "device://tone";

    final public static String MIDI_DEVICE_LOCATOR = "device://midi";


    private Manager()
    {
    }

    public static String[] getSupportedContentTypes(String aProtocol)
    {
        return sInstance.getAllSupportedContentTypes(aProtocol);
    }

    public static String[] getSupportedProtocols(String aContentType)
    {
        return sInstance.getAllSupportedProtocols(aContentType);
    }

    public static Player createPlayer(String aLocator)
    throws IOException, MediaException
    {
        return sInstance.createPlayer(aLocator);
    }

    public static Player createPlayer(DataSource aSource)
    throws IOException, MediaException
    {
        return sInstance.createInternalPlayer(aSource);
    }

    public static Player createPlayer(InputStream aStream, String aType)
    throws IOException, MediaException
    {
        return sInstance.createPlayer(aStream, aType);
    }

    static public void playTone(int aNote, int aDuration, int aVolume)
    throws MediaException
    {
        sInstance.playTone(aNote, aDuration, aVolume);
    }

    public static TimeBase getSystemTimeBase()
    {
        return sInstance.getSystemTimeBase();
    }
}
