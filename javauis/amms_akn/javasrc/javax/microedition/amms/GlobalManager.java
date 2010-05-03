/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
package javax.microedition.amms;

import javax.microedition.amms.control.audio3d.LocationControl;
import javax.microedition.amms.control.FormatControl;
import javax.microedition.media.*;

// Implementation
import com.nokia.amms.GlobalManagerImpl;

/**
 * Please refer JSR 234 for more details.
 */
public class GlobalManager
{
    /**
     * Static spectator instance. Returned from getSpectator method.
     */
    private static Spectator iSpectator;

    /**
     * Implementation. All method calls in this class will be delegated
     * GlobalManagerImpl.
     */
    private static GlobalManagerImpl iManagerImpl;

    static
    {
        // Get static instance.
        iManagerImpl = GlobalManagerImpl.getInstance();

        // Spectator delegates all method calls to spectator got from
        // GlobalManagerImpl
        iSpectator = new Spectator(iManagerImpl.getSpectator());
    }

    /**
     * Private not allow construction
     */
    private GlobalManager()
    {
    }


    /**
     * Please refer JSR 234 for more details.
     */
    public static Control[] getControls()
    {
        return iManagerImpl.getControls();
    }

    /**
     * Please refer JSR 234 for more details.
     */
    public static Control getControl(String aControlType)
    {
        return iManagerImpl.getControl(aControlType);
    }

    /**
     * Please refer JSR 234 for more details.
     */
    public static EffectModule createEffectModule() throws MediaException
    {
        return iManagerImpl.createEffectModule();
    }

    /**
     * Please refer JSR 234 for more details.
     */
    public static SoundSource3D createSoundSource3D() throws MediaException
    {
        return iManagerImpl.createSoundSource3D();
    }

    /**
     * Please refer JSR 234 for more details.
     */
    public static String[] getSupportedSoundSource3DPlayerTypes()
    {
        return iManagerImpl.getSupportedSoundSource3DPlayerTypes();
    }

    /**
     * Please refer JSR 234 for more details.
     */
    public static Spectator getSpectator() throws MediaException
    {
        return iSpectator;
    }

    /**
     * Please refer JSR 234 for more details.
     */
    public static MediaProcessor createMediaProcessor(String aContentType)
    throws MediaException
    {
        return iManagerImpl.createMediaProcessor(aContentType);
    }

    /**
     * Please refer JSR 234 for more details.
     */
    public static String[] getSupportedMediaProcessorInputTypes()
    {
        return iManagerImpl.getSupportedMediaProcessorInputTypes();
    }

}
