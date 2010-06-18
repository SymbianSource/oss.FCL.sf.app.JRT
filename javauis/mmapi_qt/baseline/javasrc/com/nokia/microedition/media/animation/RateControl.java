/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: RateControl
*
*/
package com.nokia.microedition.media.animation;

import javax.microedition.media.Player;

import com.nokia.microedition.media.control.ControlImpl;


public class RateControl extends ControlImpl implements
        javax.microedition.media.control.RateControl
{

    private final static int DEFUALT_RATE=100000;
    private final static int MIN_RATE=0;
    /**
     *
     * @param aPlayer
     */
    public RateControl(Player aPlayer)
    {
        this.iPlayer=aPlayer;
        ((AnimationPlayer)iPlayer).setiCurrentRate(DEFUALT_RATE);
    }

    /* (non-Javadoc)
     * @see javax.microedition.media.control.RateControl#getMaxRate()
     */
    public int getMaxRate()
    {
        checkState();
        return DEFUALT_RATE;
    }

    /* (non-Javadoc)
     * @see javax.microedition.media.control.RateControl#getMinRate()
     */
    public int getMinRate()
    {
        checkState();
        return MIN_RATE;
    }

    /* (non-Javadoc)
     * @see javax.microedition.media.control.RateControl#getRate()
     */
    public int getRate()
    {
        checkState();
        return ((AnimationPlayer)iPlayer).getiCurrentRate();
    }

    /**
     * Current implementation support only two rate default(100000) or min(0).
     * No other rate is supported
     */
    public int setRate(int aMilliRate)
    {
        checkState();
        int rate =aMilliRate;
        if (rate<=MIN_RATE)
        {
            rate =MIN_RATE;
        }
        else if (rate >= DEFUALT_RATE)
        {
            rate =DEFUALT_RATE;
        }
        return ((AnimationPlayer)iPlayer).setiCurrentRate(rate) ;
    }
}
