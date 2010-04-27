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
* Description:  Base class for modules.
*
*/

package com.nokia.amms;

import javax.microedition.media.MediaException;
import javax.microedition.media.Player;
import javax.microedition.media.PlayerListener;
import javax.microedition.media.Control;
import com.nokia.microedition.media.PlayerImpl;
import com.nokia.mj.impl.rt.legacy.NativeError;
import java.util.Vector;
import com.nokia.mj.impl.rt.support.Finalizer;

/**
 * Base class for all modules.
 */
public class ModuleBase implements PlayerListener
{
    /**
     * Player is already in some other module.
     */
    private static final int PLAYER_ALREADY_IN_MODULE = -123;
    private static final int MIXING_NOT_SUPPORTED = -1234;

    /**
     * Current implementation does not support midi channels in modules.
     */
    private static final String MIDI_CHANNELS_NOT_SUPPORTED =
        "Adding MIDI channels is not supported.";

    /**
     * Contains all controls in the module. Package private.
     */
    ControlContainer iControls;

    /**
     * Handle to native implemantation. Package private.
     */
    int iModuleHandle;

    /**
     * Handle to native event source. Package private.
     */
    int iEventSourceHandle;

    /**
     * Handle to native global mabager. Package private.
     * Used to dispose native module.
     */
    int iManagerHandle;

    /**
     * Java-side players. Package private.
     */
    Vector iPlayers = new Vector();

    private Finalizer mFinalizer;

    /**
     * Constructs new Module base. Package private.
     * Module is registered for the finalization.
     *
     * @param aEventSourceHandle Handle to event source.
     * @param aModuleHandle Handle to corresponding native object.
     * @param aManagerHandle Handle to native global manager.
     * @param aMana
     */
    ModuleBase(int aEventSourceHandle,
               int aModuleHandle,
               int aManagerHandle)
    {
        // registeredFinalize() method is called when this class is finalized.
        mFinalizer = new Finalizer()
        {
            public void finalizeImpl()
            {
                doFinalize();
            }
        };

        iManagerHandle = aManagerHandle;

        init(aEventSourceHandle,
             aModuleHandle);
    }

    /**
     * Constructs new Module base. Package private.
     * Module is not registered for the finalization.
     */
    ModuleBase()
    {
    }

    /**
     * Initialises module base. This method must be called before module can be
     * used. Package private.
     *
     * @param aEventSourceHandle Handle to event source.
     * @param aModuleHandle Handle to corresponding native object.
     */
    final void init(int aEventSourceHandle,
                    int aModuleHandle)
    {
        iEventSourceHandle = aEventSourceHandle;
        iModuleHandle = aModuleHandle;

        iControls = ControlContainer.populateControls(
                        iEventSourceHandle,
                        iModuleHandle,
                        this);
    }

    private void doFinalize()
    {
        if (mFinalizer != null)
        {
            registeredFinalize();
            mFinalizer = null;
        }
    }

    /**
     * This method is called when garbage collector finalizes this object.
     */
    void registeredFinalize()
    {
        iControls.invalidateControls();

        _dispose(iManagerHandle,
                 iModuleHandle);
    }

    /**
     * <p>Adds an individual MIDI channel of a MIDI <code>Player</code>
     * to the module.</p>
     *
     * <p>If the played MIDI file or MIDI stream contains information
     * that is contradictory
     * to what is specified via this <code>Module</code> the behavior
     * will be implementation specific.</p>
     *
     * @param player The MIDI <code>Player</code> whose channel is to be added.
     * @param channel The channel of the given <code>Player</code> to be added.
     * The range is 0-15.
     *
     * @throws MediaException if adding of channels is not supported.
     * @throws IllegalArgumentException if the <code>player</code> is not a MIDI player or the <code>player</code> is null
     * or if the <code>player</code> is already part of the module.
     * @throws IllegalArgumentException if <code>channel</code> > 15 or <code>channel</code> < 0.
     * @throws IllegalStateException if some <code>Player</code>
     * in the <code>Module</code> tree is not in
     * <code>UNREALIZED</code> or <code>REALIZED</code> state.
     */
    public void addMIDIChannel(Player aPlayer, int aChannel)
    throws MediaException
    {
        // Midi channels are not supported modules.
        throw new MediaException(MIDI_CHANNELS_NOT_SUPPORTED);
    }

    /**
     * Removes a MIDI channel from the module.
     *
     * All channels can be removed at once by {@link #removePlayer removePlayer}
     *
     * @param player The MIDI <code>Player</code> whose channel is to be removed.
     * @param channel The channel of the given MIDI <code>Player</code>
     * to be removed.
     * @throws IllegalArgumentException if the given <code>channel</code> is
     * not part of the <code>Module</code> or if the <code>player</code> is null.
     * @throws IllegalStateException if some <code>Player</code>
     * in the <code>Module</code> tree is not in
     * <code>UNREALIZED</code> or <code>REALIZED</code> state
     */
    public void removeMIDIChannel(Player aPlayer, int aChannel)
    {
        // Midi channels are not supported modules.
        throw new IllegalArgumentException(MIDI_CHANNELS_NOT_SUPPORTED);
    }

    /**
     * Adds a <code>Player</code> to the module.
     * @param player The <code>Player</code> to be added.
     * @throws IllegalArgumentException if the <code>player</code> is null or
     * if the <code>player</code> or some channels of
     * the <code>player</code> already belong to the module.
     * @throws MediaException if the <code>player</code> cannot be added.
     * @throws IllegalStateException if some <code>Player</code>
     * in the <code>Module</code> tree is not in
     * <code>UNREALIZED</code> or <code>REALIZED</code> state.
     */
    public void addPlayer(Player aPlayer) throws MediaException
    {
        if (aPlayer == null)
        {
            throw new IllegalArgumentException("Player is null.");
        }
        if (!(aPlayer instanceof PlayerImpl))
        {
            throw new MediaException("Player is not supported.");
        }

        // It is not reasonable to add a player that is in CLOSED state.
        if (aPlayer.getState() == Player.CLOSED)
        {
            throw new IllegalStateException("Cannot add Player while it is in CLOSED state.");
        }

        int playerHandle = ((PlayerImpl)aPlayer).getPlayerHandle();

        int err = _addPlayer(iEventSourceHandle,
                             iModuleHandle,
                             playerHandle);

        // Throw IllegalArgumentException if the player or some channels of
        // the player already belong to the module.
        if (err == NativeError.KErrArgument)
        {
            throw new IllegalArgumentException("Player already belongs to the module.");
        }

        //  Throw IllegalStateException if some Player in the Module
        // (or the player to be added) is in PREFETCHED or STARTED state
        if (err == NativeError.KErrNotReady)
        {
            throw new IllegalStateException("Cannot Add Player while any player in the module is in PREFETCHED or STARTED state");
        }

        // Throw MediaException if the player already exists in some
        // other module.
        if (err == PLAYER_ALREADY_IN_MODULE)
        {
            throw new MediaException("Player already in other module.");
        }

        // Throw MediaException if audio mixing is not supported (emulator).
        if (err == MIXING_NOT_SUPPORTED)
        {
            throw new MediaException("Mixing is not supported in emulator.");
        }

        // Throw MediaException if the player cannot be added.
        if (err < 0)
        {
            throw new MediaException(NativeError.errorMessage(err));
        }

        iPlayers.addElement(aPlayer);
        aPlayer.addPlayerListener(this);
    }

    /**
     * Removes a <code>Player</code> or all channels of a <code>Player</code>
     * from the module.
     * @param player The <code>Player</code> to be removed.
     * @throws IllegalArgumentException if <code>player</code> is not part of
     * the
     * module or if <code>player</code> is null.
     * @throws IllegalStateException if some <code>Player</code>
     * in the <code>Module</code> tree is not in
     * <code>UNREALIZED</code> or <code>REALIZED</code> state.
     */
    public void removePlayer(Player aPlayer)
    {
        // Throw IllegalArgumentException if the player is null, incorrect
        // type, or does not belong to the module.
        if (aPlayer == null)
        {
            throw new IllegalArgumentException("Player is null.");
        }
        if (!(aPlayer instanceof PlayerImpl))
        {
            throw new IllegalArgumentException("Not supported player.");
        }

        // If the player is in CLOSED state, the native side player is
        // already removed and destroyed.
        if (aPlayer.getState() != Player.CLOSED)
        {
            // The player list does not contain CLOSED players, thus do not
            // check the existence for these players.
            if (!iPlayers.contains(aPlayer))
            {
                throw new IllegalArgumentException(
                    "Player does not belong to the module.");
            }

            int playerHandle = ((PlayerImpl)aPlayer).getPlayerHandle();

            int err = _removePlayer(iEventSourceHandle,
                                    iModuleHandle,
                                    playerHandle);

            if (err == NativeError.KErrNotReady)
            {
                throw new IllegalStateException(
                    "Cannot remove Player while any player in the module is in PREFETCHED or STARTED state");
            }

            NativeError.check(err);

            aPlayer.removePlayerListener(this);
        }

        iPlayers.removeElement(aPlayer);
    }

    /**
     * Obtain the object that implements the specified Control interface.
     *
     * @see javax.microedition.media.Controllable
     * @param aControlType  the class name of the Control. The class name should
     * be given either as the fully-qualified name of the class; or if the
     * package of the class is not given, the package
     * javax.microedition.media.control is assumed.
     * @return the object that implements the control, or null.
     */
    public Control getControl(String aControlType)
    {
        // Delegate to ControlContainer
        return iControls.getControl(aControlType);
    }

    /**
     * Obtain the collection of Controls.
     *
     * @see javax.microedition.media.Controllable
     * @return the collection of Control objects.
     */
    public Control[] getControls()
    {
        // Delegate to ControlContainer
        return iControls.getControls();
    }

    /**
     * From PlayerListener.
     */
    public void playerUpdate(Player aPlayer, String aEvent, Object aEventData)
    {
        // Remove the player from the vector in order to allow
        // the garbage collector to dispose the player.
        if (aEvent == PlayerListener.CLOSED)
        {
            iPlayers.removeElement(aPlayer);
        }
    }

    /**
     * Removes a Player from module.
     * (The function is protected to allow inherited modules to remove
     * CLOSED players which is not possible by removePlayer() function).
     *
     * @param aEventSourceHandle Handle to native event source.
     * @param aModuleHandle Handle to native module object.
     * @param aPlayerHandle Handle to native player object.
     * @return Negative value if player does not exist in the module.
     */
    static native protected int _removePlayer(int aEventSourceHandle,
            int aModuleHandle,
            int aPlayerHandle);

    /**
     * Adds a Player to module.
     *
     * @param aEventSourceHandle Handle to native event source.
     * @param aModuleHandle Handle to native module object.
     * @param aPlayerHandle Handle to native player object.
     * @return Error code
     */
    static native private int _addPlayer(int aEventSourceHandle,
                                         int aModuleHandle,
                                         int aPlayerHandle);

    /**
     * Disposes native module.
     *
     * @param aManagerHandle Handle to native global manager.
     * @param aModuleHandle Handle to native module object.
     */
    static native private void _dispose(int aManagerHandle,
                                        int aModuleHandle);
}
