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
* Description:
*
*/

package com.nokia.mj.impl.properties.mobilemedia;

import com.nokia.mj.impl.rt.support.SystemPropertyProvider;
import javax.microedition.media.Manager;

/**
 * A class for handling dynamic system properties of MobileMedia API.
 */
public class DynamicPropertyHandler implements SystemPropertyProvider
{

    /*
     * Constants for identifying the asked property.
     */
    private static String AUDIO_ENCODINGS = "audio.encodings";
    private static String SUPPORTS_VIDEO_CAPTURE = "supports.video.capture";
    private static String VIDEO_ENCODINGS = "video.encodings";

    // start of the audio content types
    private static final String AUDIO = "audio";

    // special audio encoding strings which are defined in the JSR
    private static final String AUDIO_ENCODINGS_JSR =
        " encoding=pcm encoding=ulaw encoding=gsm";

    // start of the video content types
    private static final String VIDEO = "video";


    /* (non-Javadoc)
     * @see com.nokia.mj.impl.rt.DynamicProperty#getProperty(com.nokia.mj.impl.rt.DynamicPropertyInfo)
     */
    public String getProperty(String propertyName)
    {
        String propertyValue = null;

        if (propertyName.equals(AUDIO_ENCODINGS))
        {
            propertyValue = getSupportedCaptureEncodings(AUDIO) +
                            AUDIO_ENCODINGS_JSR;
        }
        else if (propertyName.equals(SUPPORTS_VIDEO_CAPTURE))
        {
            propertyValue = getSupportedCaptureEncodings(VIDEO);
            // video capture is supported only if there is supported encodings
            propertyValue = String.valueOf(propertyValue != null);
        }
        else if (propertyName.equals(VIDEO_ENCODINGS))
        {
            propertyValue = getSupportedCaptureEncodings(VIDEO);
        }
        return propertyValue;
    }

    public boolean isStatic(String key)
    {
        // All property values of MobileMedia API do not change during the
        // lifetime of the application.
        return true;
    }

    // capture protocol
    private static final String CAPTURE = "capture";

    // encoding for media strings
    private static final String ENCODING = "encoding=";

    /**
     * Returns supported capture encodings.
     * @param aPrefix Capture format type.
     * @return the supported capture encodings.
     */
    private String getSupportedCaptureEncodings(String aPrefix)
    {
        // get supported content types from all capture:// protocols
        String[] cTypes = Manager.getSupportedContentTypes(CAPTURE);

        StringBuffer sBuffer = new StringBuffer();

        // go through all types
        for (int i = 0; i < cTypes.length; i++)
        {
            if (cTypes[ i ].startsWith(aPrefix))
            {
                // if video type appent to buffer encoding= + content type
                sBuffer.append(ENCODING + cTypes[ i ] + " ");
            }
        }
        String encodings = null;

        // if no types was found null must be returned
        if (sBuffer.length() > 0)
        {
            // substring is taken to remove last empty space added in previous
            // for loop
            encodings = sBuffer.toString().substring(0,
                        sBuffer.length() - 1);
        }
        return encodings;
    }
}
