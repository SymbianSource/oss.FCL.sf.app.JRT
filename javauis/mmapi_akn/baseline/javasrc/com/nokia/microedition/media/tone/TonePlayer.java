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
* Description:
*
*/


package com.nokia.microedition.media.tone;

import javax.microedition.media.PlayerListener;
import javax.microedition.media.MediaException;
import javax.microedition.media.control.ToneControl;
import javax.microedition.media.protocol.DataSource;
import javax.microedition.media.protocol.SourceStream;
import javax.microedition.media.Manager;
import javax.microedition.media.Player;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import com.nokia.microedition.media.PlayerBase;
import com.nokia.microedition.media.InternalPlayer;
import com.nokia.microedition.media.tone.ToneSequence;
import com.nokia.microedition.media.control.MIDIControl;
import com.nokia.microedition.media.PlayerListenerImpl;
import java.util.Vector;
import com.nokia.mj.impl.utils.Logger;


/**
 * Player that is created with Manager.TONE_DEVICE_LOCATOR locator or
 * with jts content.
 */
public class TonePlayer extends PlayerBase implements ToneControl, PlayerListener
{
    // This is allways tone's content type
    public static final String TONE_CONTENT_TYPE =
        "audio/x-tone-seq";

    private static final String TONE_LOCATOR_SUFFIX = ".jts";

    // MIDI player used to play tone
    private InternalPlayer iMidiPlayer;

    // Internal MIDI player listener
    private PlayerListenerImpl iPlayerListenerImpl;

    // MIDI control used to reset native midi engine.
    private MIDIControl iMIDIControl;

    // Tone sequence processed to midi sequence
    private byte[] iMidiSequence;

    // Tone sequence duration
    private long iDuration;

    // Granularity for tone sequence reader
    private static final int SEQUENCE_READ_GRANULARITY = 1024; // 1 KB

    // Empty midi sequence for midi player initialization
    private static final byte[] EMPTY_MIDI_SEQUENCE =
    {
        0x4D,0x54,0x68,0x64,0x00,0x00,0x00,0x06,0x00,0x00,0x00,0x01,0x00,0x10,
        0x4D,0x54,0x72,0x6B,0x00,0x00,0x00,0x12,0x00,(byte)0xFF,0x51,0x03,
        0x07,(byte)0xA1,0x20,0x00,(byte)0xC0,0x01,0x00,(byte)0x80,0x40,
        0x7F,0x00,(byte)0xFF,0x2F,0x00
    };

    private static final String[] TONE_FORBIDDEN_CONTROLS =
        { "javax.microedition.media.control.MetaDataControl" };

    // For checking whether a control is allowed for tone player
    static final String CONTROL_DEFAULT_PACKAGE =
        "javax.microedition.media.control.";

    /**
     * Create player without content.
     */
    public TonePlayer() throws MediaException
    {
        initializeMidi();
    }


    /**
     * Create player from DataSource.
     * @param aDataSource DataSource with audio/x-tone-seq content.
     */
    public TonePlayer(DataSource aDataSource) throws MediaException
    {
        initializeMidi();

        // Read .JTS file data from data source
        SourceStream input = (aDataSource.getStreams())[ 0 ];
        byte[] sequenceTemp = new byte[ SEQUENCE_READ_GRANULARITY ];
        ByteArrayOutputStream sequenceStream =
            new ByteArrayOutputStream(SEQUENCE_READ_GRANULARITY);
        byte[] sequence = null; // container for read tone sequence
        int readCount = 0;

        // Read data in SEQUENCE_READ_GRANULARITY sized blocks
        try
        {
            while (readCount >= 0)
            {
                readCount = input.read(sequenceTemp, 0, SEQUENCE_READ_GRANULARITY);
                if (readCount > 0)
                {
                    sequenceStream.write(sequenceTemp, 0, readCount);
                }
            }
            sequenceStream.flush();
            sequence = sequenceStream.toByteArray();
            sequenceStream.close();
        }
        catch (IOException ioe)
        {
            Logger.ELOG(Logger.EJavaMMAPI,
                        "MMA::TonePlayer: TonePlayer( DataSource ): " + ioe.toString());
            throw new IllegalArgumentException();
        }

        setSequence(sequence);
    }


    /**
     * Private method for initializing internal midi player
     */
    private void initializeMidi() throws MediaException
    {
        // Create concrete PlayerListenerImpl for listeners of
        // this TonePlayer
        iPlayerListenerImpl = new PlayerListenerImpl(this);
        iMidiSequence = EMPTY_MIDI_SEQUENCE;

        try
        {
            ByteArrayInputStream in = new ByteArrayInputStream(iMidiSequence);
            iMidiPlayer = (InternalPlayer)Manager.createPlayer(in, "audio/midi");
        }
        catch (IOException ioe)
        {
            throw new IllegalStateException();
        }

        // we only have controls from MIDI player this far, so ToneControl
        // must be added.

        addControl(this, MidiToneConstants.TONE_CONTROL_FULL_NAME);

        // register this TonePlayer as MIDI player listener
        iMidiPlayer.addPlayerListener(this);
    }


    /**
     * @see ToneControl
     */
    public void setSequence(byte[] aSequence)
    {
        // player must not be prefetched or started
        int state = getState();
        if (state == javax.microedition.media.Player.PREFETCHED)
        {
            throw new IllegalStateException("Player is PREFETCHED");
        }
        if (state == javax.microedition.media.Player.STARTED)
        {
            throw new IllegalStateException("Player is STARTED");
        }

        try
        {
            // process tone sequence into midi sequence
            ToneSequence tone = new ToneSequence(aSequence);
            tone.process();
            iMidiSequence = tone.getByteArray();
            iDuration = tone.getDuration();
        }
        catch (Exception e)
        {
            Logger.ELOG(Logger.EJavaMMAPI,
                        "MMA: TonePlayer: setSequence: !! Exception: " + e);
            throw new IllegalArgumentException("Illegal sequence");
        }
    }


    /**
     * Adds control to the player.
     * @param aControl Player's control
     * @param aControlName Identifies the control. Name must contain
     *                     full package information
     */
    public void addControl(javax.microedition.media.Control aControl,
                           String aControlName) throws MediaException
    {
        iMidiPlayer.addControl(aControl, aControlName);
    }


    /**
     * interface Controllable
     * Return controls
     * @return list of controls
     * @see Controllable
     */
    public javax.microedition.media.Control[] getControls()
    {
        closeCheck();
        unrealizedCheck();

        // Get all controls but discard forbidden ones
        javax.microedition.media.Control[] allControls =
            iMidiPlayer.getControls();
        javax.microedition.media.Control[] controls = null;

        Vector controlVector = new Vector(allControls.length);

        for (int i = 0; i < allControls.length; i++)
        {
            // Add only allowed controls to control vector
            if (isAllowedControl(allControls[ i ].getClass()))
            {
                controlVector.addElement(allControls[ i ]);
            }

            controls =
                new javax.microedition.media.Control[ controlVector.size()];

            controlVector.copyInto(controls);
        }
        return controls;
    }


    /**
     * interface Controllable
     * get control by content type
     * @param aControlType content type of wanted control
     * @return control
     * @see Controllable
     * @see Control
     */
    public javax.microedition.media.Control getControl(String aControlType)
    {
        closeCheck();
        unrealizedCheck();

        if (aControlType == null)
        {
            throw new IllegalArgumentException("argument was null");
        }

        // If not fully qualified name, then supply default package
        String controlType = null;
        if (aControlType.indexOf(".") == -1)
        {
            controlType = CONTROL_DEFAULT_PACKAGE + aControlType;
        }
        else
        {
            controlType = aControlType;
        }

        Class controlClass = null;
        try
        {
            controlClass = Class.forName(controlType);
        }
        catch (ClassNotFoundException cnfe)
        {
            return null;
        }

        if (isAllowedControl(controlClass))
        {
            return iMidiPlayer.getControl(controlType);
        }
        else
        {
            return null;
        }
    }


    /**
     * interface Player
     * @see Player
     */
    public void addPlayerListener(PlayerListener aPlayerListener)
    {
        closeCheck();
        iPlayerListenerImpl.addPlayerListener(aPlayerListener);
    }


    /**
     * From Player
     * @see Player
     */
    public void removePlayerListener(PlayerListener aPlayerListener)
    {
        closeCheck();
        iPlayerListenerImpl.removePlayerListener(aPlayerListener);
    }


    /**
     * From Player
     * @see Player
     */
    public int getState()
    {
        return iMidiPlayer.getState();
    }


    /**
     * From PlayerBase
     * @see PlayerBase
     */
    public void doRealize() throws MediaException
    {
        iMidiPlayer.realize();
    }


    /**
     * From PlayerBase
     * @see PlayerBase
     */
    public void doPrefetch() throws MediaException
    {

        // we get javax.microedition control which is cast to com.nokia control
        iMIDIControl = (MIDIControl)
                       iMidiPlayer.getControl(MidiToneConstants.MIDI_CONTROL_FULL_NAME);

        // reinitialize midi engine with the sequence currently
        // in iMidiSequence

        iMIDIControl.reInitializeMidi(
            iMidiSequence, 0, iMidiSequence.length);
    }


    /**
     * From PlayerBase
     * @see PlayerBase
     */
    public void doDeallocate()
    {
        iMidiPlayer.deallocate();
    }


    /**
     * From Player.
     * @see Player
     */
    public void start() throws MediaException
    {
        prefetch();
        iMidiPlayer.start();
    }


    /**
     * From PlayerBase
     * @see PlayerBase
     */
    public void doStop() throws MediaException
    {
        iMidiPlayer.stop();
    }


    /**
     * From PlayerBase
     * @see PlayerBase
     */
    public void doClose()
    {
        iDuration = 0;
        iMidiPlayer.close();
    }


    /**
     * interface Player
     * @see Player
     */
    public void setLoopCount(int aCount)
    {
        iMidiPlayer.setLoopCount(aCount);
    }


    /**
     * interface Player
     * @see Player
     */
    public long getDuration()
    {
        closeCheck();

        int state = iMidiPlayer.getState();

        // Return TIME_UNKNOWN if player is realized and a midi
        // sequence has been explicitely set.
        if (state == Player.REALIZED &&
                !(iMidiSequence.equals(EMPTY_MIDI_SEQUENCE)))
        {
            iDuration = Player.TIME_UNKNOWN;
        }
        else if (state == Player.PREFETCHED ||
                 state == Player.STARTED)
        {
            iDuration = iMidiPlayer.getDuration();
        }
        return iDuration;
    }


    /**
     * From PlayerBase
     * @see PlayerBase
     */
    public long setMediaTime(long aNow) throws MediaException
    {
        return iMidiPlayer.setMediaTime(aNow);
    }


    /**
     * interface Player
     * @see Player
     */
    public long getMediaTime()
    {
        closeCheck();
        return iMidiPlayer.getMediaTime();
    }


    /**
     * interface Player
     * @see Player
     */
    public String getContentType()
    {
        closeCheck();
        unrealizedCheck();
        return TONE_CONTENT_TYPE;
    }

    /**
     * from PlayerListener
     * @see PlayerListener
     */
    public void playerUpdate(Player player,
                             java.lang.String event,
                             java.lang.Object eventData)
    {
        iPlayerListenerImpl.postEvent(event, eventData);
    }

    /**
     * Check whether a control is allowed for tone player.
     * @param aControl instance of control to check
     * @return boolean allowed or not
     */
    private boolean isAllowedControl(Class aControlClass)
    {
        for (int i = 0; i < TONE_FORBIDDEN_CONTROLS.length; i++)
        {
            Class forbidden = null;
            try
            {
                forbidden = Class.forName(TONE_FORBIDDEN_CONTROLS[ i ]);
            }
            catch (ClassNotFoundException cnfe)
            {
                // This should never happen
                throw new Error("Control class not found");
            }

            if (forbidden.isAssignableFrom(aControlClass))
            {
                // Not allowed
                return false;
            }
        }
        // else
        return true;
    }
}
