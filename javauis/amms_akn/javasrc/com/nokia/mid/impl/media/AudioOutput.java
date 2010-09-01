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
* Description: Control for routing the player's audio output.
*
*/
package com.nokia.mid.impl.media;
//AudioOutput Class which has info regarding the Output mode and device type
public class AudioOutput implements com.nokia.mid.media.AudioOutput
{
    //Constructor
    public AudioOutput(int mode)
    {
        outputmode = mode;
    }
    //public
    public int getActiveOutputMode()
    {
        return outputmode;
    }
    public int[] getOutputDevices()
    {
        return null;
    }
    //package access
    void setOutputMode(int mode)
    {
        outputmode = mode;
    }

    //private
    private int outputmode;


}




