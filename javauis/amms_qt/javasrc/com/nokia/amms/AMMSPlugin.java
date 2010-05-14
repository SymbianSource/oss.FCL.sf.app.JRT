/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Adds PriorityControl when a player is created.
*
*/

package com.nokia.amms;

import com.nokia.amms.control.PriorityControl;
import com.nokia.mid.impl.media.AudioOutputControl;
import com.nokia.microedition.media.InternalPlayer;
import com.nokia.microedition.media.PlayerImpl;
import com.nokia.microedition.media.PlugIn;
import com.nokia.microedition.media.ManagerImpl;
import com.nokia.microedition.media.NativeError;
import javax.microedition.media.MediaException;
import javax.microedition.media.protocol.DataSource;
import java.io.IOException;


// CLASS DEFINITION
/**
 * MMA Manager calls AMMSPlugin class when a new player is created.
 * The plugin adds PriorityControl to the created player.
 */
public class AMMSPlugin implements PlugIn
{
    private final String PRIORITY_CONTROL =
        "javax.microedition.amms.control.PriorityControl";
    private final String AUDIO_OUTPUT_CONTROL =
        "com.nokia.mid.media.AudioOutputControl";

    /**
     * Default constructor.
     */
    public AMMSPlugin()
    {
    }

    /**
     * From PlugIn. Empty implementation.
     */
    public String[] getSupportedContentTypes(String aProtocol)
    {
        return new String[ 0 ];
    }

    /**
     * From PlugIn. Empty implementation.
     */
    public String[] getSupportedProtocols(String aContentType)
    {
        return new String[ 0 ];
    }

    /**
     * From PlugIn. Empty implementation.
     */
    public InternalPlayer createPlayer(DataSource aDataSource)
    throws MediaException, IOException
    {
        return null;
    }

    /**
     * From PlugIn.
     */
    public void preparePlayer(InternalPlayer aPlayer) throws MediaException
    {
        // Do not add PriorityControl if the player is not derived from
        // PlayerImpl (because native player handle is needed).
        if (aPlayer instanceof PlayerImpl)
        {
            PlayerImpl player = (PlayerImpl)aPlayer;

            int eventSource = ManagerImpl.getEventSource();
            createAndAddPriorityControlToPlayer(eventSource, player);
            createAndAddAudioOutputControlToPlayer(eventSource, player);
        }

    }
    /**
    * Create and add priority control to player
    */
    public void createAndAddPriorityControlToPlayer(int eventSource, PlayerImpl aPlayer)
    {
        // Create native PriorityControl and add it to the native player.
        int nativePriorityControl = _createNativePriorityControl(
                                        eventSource, aPlayer.getPlayerHandle());

        NativeError.check(nativePriorityControl);


        // Create java side control.
        PriorityControl priorityControl = new PriorityControl(aPlayer);
        priorityControl.initControl(eventSource, nativePriorityControl,
                                    null);   // This control does not belong to any module.

        aPlayer.addControl(priorityControl, PRIORITY_CONTROL);
    }
    /**
    * Create and add AudioOutput control to player
    */
    public void createAndAddAudioOutputControlToPlayer(int eventSource, PlayerImpl aPlayer)
    {
        // Create native AudioOutputControl and add it to the native player.
        int nativeAudioOutputControl = _createNativeAudioOutputControl(
                                           eventSource, aPlayer.getPlayerHandle());
        NativeError.check(nativeAudioOutputControl);


        // Create java side control.
        AudioOutputControl audioOutputControl = new AudioOutputControl(aPlayer);
        audioOutputControl.initControl(eventSource, nativeAudioOutputControl,
                                       null);  // This control does not belong to any module.
        audioOutputControl.SetAudioOutputToNative();
        aPlayer.addControl(audioOutputControl, AUDIO_OUTPUT_CONTROL);
    }

    /**
     * Creates native PriorityControl and adds it
     */
    private static native int _createNativePriorityControl(
        int aEventSource, int aPlayerHandle);
    /**
    * Creates native AudioOutputControl and adds it
    */
    private static native int _createNativeAudioOutputControl(
        int aEventSource, int aPlayerHandle);
}
