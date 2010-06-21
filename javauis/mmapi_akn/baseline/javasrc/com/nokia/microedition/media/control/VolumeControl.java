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
* Description:  Item for VideoControl.
*
*/


package com.nokia.microedition.media.control;

import com.nokia.mj.impl.rt.support.ApplicationInfo;
import com.nokia.mj.impl.utils.Tokenizer;
import javax.microedition.media.Player;

public class VolumeControl extends ControlImpl
        implements javax.microedition.media.control.VolumeControl
{
    private static final int MAX_VOLUME = 100;
    private static final int UNDEFINED_VOLUME = -1;
    private static final String SEPARATOR = ",";
    private static final String ATTRIB_NOKIA_UI_ENHANCEMENT =
        "Nokia-UI-Enhancement";
    private static final String IGNORE_PROFILES_BASED_SOUNDING =
        "IgnoreProfilesBasedSoundMuting";
    // these constants come from native side
    protected static final int NO_ERROR = 0; // native side returns if OK

    private int iLevel;
    private boolean iMuted;

    public VolumeControl()
    {
        iLevel = UNDEFINED_VOLUME;
    }

    public boolean isMuted()
    {
        checkState();
        return iMuted;
    }

    /**
     * Set handles
     * @param aPlayer Player instance
     * @param aEventSource Handle to event source
     * @param aControlHandle Handle Handle to control
     */
    public void setHandles(Player aPlayer,
                           int aEventSource,
                           int aControlHandle)
    {
        super.setHandles(aPlayer, aEventSource, aControlHandle);
        boolean ignoreProfileBasedSoundMuting = false;
        String attrib = ApplicationInfo.getInstance().getAttribute(
                            ATTRIB_NOKIA_UI_ENHANCEMENT);
        String[] parsedAttributes = Tokenizer.split( attrib, SEPARATOR);
        if (parsedAttributes != null)
        {
            for (int i = 0; i < parsedAttributes.length; i++)
            {
                if (IGNORE_PROFILES_BASED_SOUNDING.equalsIgnoreCase(
                            parsedAttributes[i]))
                {
                    ignoreProfileBasedSoundMuting = true;
                    break;
                }
            }
        }
        if (!ignoreProfileBasedSoundMuting)
        {
            int err =_setProfilesBasedSoundMuting(aEventSource, aControlHandle);
            if (err < NO_ERROR)
            {
                throw new Error("Setting profile based sound " +
                                          "muting failed" + err);
            }
        }
    }

    public int getLevel()
    {
        checkState();

        // If the control is muted the cached volume level is returned
        // because the native implemnetation doesn't directly support
        // muting. The mute is applied by setting volume level to zero
        if (!iMuted)
        {
            int level = _getLevel(iEventSource, iControlHandle);
            if (level < NO_ERROR)
            {
                throw new Error("getLevel() failed, Symbian OS error: " + level);
            }
            // Cache the returned volume level due to mute handling
            iLevel = level;
        }

        return iLevel;
    }


    public int setLevel(int aLevel)
    {
        checkState();
        int level = aLevel;

        if (level < 0)
        {
            level = 0;
        }
        else if (level > MAX_VOLUME)
        {
            level = MAX_VOLUME;
        }

        if (!iMuted)
        {
            int error = _setLevel(iEventSource, iControlHandle, level);
            if (error < NO_ERROR)
            {
                throw new Error("setLevel() failed, Symbian OS error: " + error);
            }
        }
        iLevel = level;
        return level;
    }

    public void setMute(boolean aMuted)
    {
        checkState();
        if (aMuted != iMuted)
        {
            if (iLevel == UNDEFINED_VOLUME)
            {
                getLevel();
            }
            iMuted = aMuted;
            int level = 0;
            if (!iMuted)
            {
                level = iLevel;
            }
            // else level is 0

            int error = _setLevel(iEventSource, iControlHandle, level);
            if (error < NO_ERROR)
            {
                throw new Error("setMute() failed, Symbian OS error: " + error);
            }
        }
    }

    private static native int _setLevel(int aEventSource, int aControlHandle,
                                        int aLevel);
    private static native int _getLevel(int aEventSource, int aControlHandle);

    private static native int _setProfilesBasedSoundMuting(int aEventSource,
            int aControlHandle);
}
