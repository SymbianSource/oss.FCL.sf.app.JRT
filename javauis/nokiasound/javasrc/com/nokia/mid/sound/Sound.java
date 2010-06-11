/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides Sound API for playing tones and digitized audio.
*
*/


package com.nokia.mid.sound;

import com.nokia.mj.impl.rt.support.Finalizer;
import java.util.Vector;
import com.nokia.mj.impl.utils.Logger;
import java.lang.Thread;

/**
 * <p>
 * Provides simple Sound API for playing tones and digitized audio.
 * </p><p>
 * Since MIDP doesn't have any Sound API for games there is a need
 * for proprietary sound extension to support devices audio
 * capabilities. Every implementation has capability to produce tone
 * sounds (e.g. ringing tones), this is the minimum support. Currently
 * some products support or will support also digitized audio formats.
 * The Game sound API will support both buzzer and digitized audio.
 * Buzzer must be supported by all implementations. If implementation
 * doesn't have buzzer the buzzer tones are emulated.
 * </p>
 * Since implementations have different audio capabilities,
 * application can query which audio formats are supported by
 * implementation by calling {@link #getSupportedFormats()}.
 * <p>
 * All implementations need to support at least tone based sounds
 * (type FORMAT_TONE) via {@link #Sound(int freq, long duration)} and
 * {@link #init(int freq, long duration)}. In addition all implementations
 * must support Smart messaging ringingtone format (type FORMAT_TONE)
 * via {@link #Sound(byte[] data, int type)} and
 * {@link #init(byte[] data, int type) }.
 * <p>
 * Note that there is also work going on with Multimedia API that
 * is done in JCP as
 * <a href="http://www.jcp.org/jsr/detail/135.jsp">JSR 135</a>.
 * The standard Multimedia API
 * will replace the proprietary Game Sound API when it is ready. However
 * Sound API will be supported also later on but probably it will be
 * stated as deprecated.
 * </p>
 * @version 1.1
 * @see com.nokia.mid.ui.DeviceControl
 * @since 1.0
 */

public class Sound
{

    /**
     * Tone based format is used.
     *
     * init(int freq, int duration) puts sound into this format.
     * @since 1.0
     *
     */
    public static final int FORMAT_TONE = 1;

    /**
     * Content is in WAV format.
     * @since 1.0
     *
     */
    public static final int FORMAT_WAV = 5;

    /**
     * Sound is playing.
     * @since 1.0
     *
     */
    public static final int SOUND_PLAYING = 0;

    /**
     * Sound is stopped.
     * @since 1.0
     *
     */
    public static final int SOUND_STOPPED = 1;

    /**
     * Sound is uninitialized (released).
     * @since 1.0
     */
    public static final int SOUND_UNINITIALIZED = 3;

    /**
     * Sound is reinitialising
     */
    private static final int SOUND_REINITIALISING = 4;

    private static final int FORMAT_BEEP = 2;
    private static final int NOT_SUPPORTED_ERROR = 3;

    private static final int ERR_NOT_READY = -18;
    private static final int ERR_ARGUMENT  = -6;

    private int iHandle;

    private Finalizer iFinalizer;
    private int iCurrentType;
    private int iState;
    private int iGain = -1;

    Vector iSoundListeners = new Vector();

    private static Sound iPlayingSound;

    static
    {
        com.nokia.mj.impl.rt.support.Jvm.loadSystemLibrary("javanokiasound");
    }

    /**
     * Constructors initialize the Sound object so that it is ready for
     * playback. This constructor is used for initializing Sound
     * object based on byte array data. The data should contain the data
     * presented in the data format specified by type parameter. The Sound
     * class defines also generally supported types as constants.
     * <p>
     * All implementations need to support at least Nokia
     * Smart Messaging, Over the Air (OTA) ringingtone format.
     * The type of this format is FORMAT_TONE.
     * <p>
     * Note: some implementations can't throw exceptions about
     * sound data being corrupted or illegal during construction.
     * This will result that IllagalArgumentException is delayed until
     * play(int loop) method is called. Applications thus need to except
     * that IllegalArgumentException is thrown in this method or during
     * play method call.
     * <p>
     * @throws java.lang.IllegalArgumentException if the data can not be
     recognized to
     * given type or the type is unsupported or unknown
     * @throws java.lang.NullPointerException if the data is null
     * @since 1.0
     *
     */
    public Sound(byte[] data, int type)
    {
        Logger.LOG(Logger.EJavaUI, Logger.EInfo, "Sound Constructor");
        iFinalizer = registerForFinalization();

        iHandle = _create();

        iState = SOUND_UNINITIALIZED;

        init(data, type);
    }

    /**
     * Constructors initialize the Sound object so that it is ready for
     * playback. Sound is initialized as a simple tone based sound.
     * <p>
     * See method {@link #init(int freq, long duration)} for
     * freq value descriptions. See also a note on exceptions semantics in
     * {@link #init(int freq, long duration)}.
     *
     * @param freq a frequency value
     * @param duration the duration of the tone in milliseconds
     * @throws java.lang.IllegalArgumentException if parameter values are
     * illegal, freq is not in given range or duration is negative or zero
     * @since 1.0
     */
    public Sound(int freq, long duration)
    {

        iFinalizer = registerForFinalization();
        iHandle = _create();

        iState = SOUND_UNINITIALIZED;

        init(freq, duration);
    }

    /**
      * Called when this object is finalized, frees native resources
      */

    public Finalizer registerForFinalization()
    {

        return new Finalizer()
        {
            public void finalizeImpl()
            {
                doFinalize();
            }
        };
    }

    void doFinalize()
    {

        if (iFinalizer == null)
        {
            return;
        }
        iFinalizer = null;

        if (iHandle > 0)
        {
            _dispose(iHandle);
        }
    }

    /**
     * Releases audio resources reserved by this object. After object
     * is released it goes to uninitialized state. This method should
     * be called when Sound object is not needed anymore.
     * @since 1.0
     */
    public void release()
    {
        if ((iState != SOUND_UNINITIALIZED) &&
                (iState != SOUND_REINITIALISING))
        {
            iState = SOUND_REINITIALISING;
            soundStateChanged(SOUND_UNINITIALIZED);
        }

        _release(iHandle);

        iState = SOUND_UNINITIALIZED;

    }

    /**
     * Initializes Sound to play a simple beep.
     * <p>
     * Note: some implementations may not support the full frequency
     * scale defined in table below. They will throw
     * IllegalArgumentException instead for unsupported values. The
     * exception may also be delayed
     * until the play(int loop) method is called.
     * <p>
     * Following table describes some freq argument
     * values:
     * <pre>
     * Description            Frequency
     * Freq off               0
     * Ring freq A0           220
     * Ring freq B0b          233
     * Ring freq B0           247
     * Ring freq C0           262
     * Ring freq D0b          277
     * Ring freq D0           294
     * Ring freq E0b          311
     * Ring freq E0           330
     * Ring freq F0           349
     * Ring freq G0b          370
     * Ring freq G0           392
     * Ring freq A1b          416
     * Ring freq A1           440
     * Ring freq B1b          466
     * Ring freq B1           494
     * Ring freq C1           523
     * Ring freq D1b          554
     * Ring freq D1           587
     * Ring freq E1b          622
     * Ring freq E1           659
     * Ring freq F1           698
     * Ring freq G1b          740
     * Ring freq G1           784
     * Ring freq A2b          831
     * Ring freq A2           880
     * Ring freq B2b          932
     * Ring freq B2           988
     * Ring freq C2           1047
     * Ring freq D2b          1109
     * Ring freq D2           1175
     * Ring freq E2b          1245
     * Ring freq E2           1319
     * Ring freq F2           1397
     * Ring freq G2b          1480
     * Ring freq G2           1568
     * Ring freq A3b          1661
     * Ring freq A3           1760
     * Ring freq B3b          1865
     * Ring freq B3           1976
     * Ring freq C3           2093
     * Ring freq D3b          2217
     * Ring freq D3           2349
     * Ring freq E3b          2489
     * Ring freq E3           2637
     * Ring freq F3           2794
     * Ring freq G3b          2960
     * Ring freq G3           3136
     * Ring freq A4b          3322
     * Ring freq A4           3520
     * Ring freq B4b          3729
     * Ring freq B4           3951
     * Ring freq C4           4186
     * Ring freq D4b          4434
     * Ring freq D4           4698
     * Ring freq E4b          4978
     * Ring freq E4           5274
     * Ring freq F4           5588
     * Ring freq G4b          5920
     * Ring freq G4           6272
     * Ring freq A5b          6644
     * Ring freq A5           7040
     * Ring freq B5b          7458
     * Ring freq B5           7902
     * Ring freq C5           8372
     * Ring freq D5b          8870
     * Ring freq D5           9396
     * Ring freq E5b          9956
     * Ring freq E5           10548
     * Ring freq F5           11176
     * Ring freq G5b          11840
     * Ring freq G5           12544
     * Ring freq A6b          13288
     *
     * </pre>
     *
     * @param duration length of the beep in milliseconds
     * @param freq frequency to be played
     * @throws java.lang.IllegalArgumentException if parameter values are
     * illegal, freq is not in given range or duration is negative or zero
     * @since 1.0
     */
    public void init(int freq, long duration)
    {
        if (duration < 1 || duration > 10000000)
        {
            throw(new IllegalArgumentException(
                      "Bad duration value, must be 1-10000000"));
        }
        if (freq < 0 || freq > 15000)
        {
            throw(new IllegalArgumentException(
                      "Bad frequency value, must be 0-15000"));
        }
        // if the uninitialised event is sent from native side, it reaches
        // listener too late in TCK test sound8004, thus we send the event
        // already here
        if ((iState != SOUND_UNINITIALIZED) &&
                (iState != SOUND_REINITIALISING))
        {
            iState = SOUND_REINITIALISING;
            soundStateChanged(SOUND_UNINITIALIZED);
        } // end of if (iState != SOUND_UNINITIALIZED)

        iCurrentType = FORMAT_BEEP;
        int err = _init(iHandle, iCurrentType, null, freq, duration);
        if (err == ERR_NOT_READY)
        {
            throw new RuntimeException(Integer.toString(err));
        }
        else if (err == ERR_ARGUMENT)
        {
            throw new IllegalArgumentException("Data is invalid");            
        }
        iState = SOUND_STOPPED;
    }

    /**
     * Initializes Sound object based on byte
     * array data. The data should contain the data presented in the data
     * format specified by type parameter. The Sound class defines also
     * generally supported types as constants.
     * <p>
     * All implementations need to support at least Nokia
     * Smart Messaging, Over the Air (OTA) ringingtone format.
     * The type of this format is FORMAT_TONE.
     * <p>
     * Note: some implementations can't throw exceptions about
     * sound data being corrupted or illegal during this method call.
     * This will result that IllagalArgumentException is delayed until
     * play(int loop) method is called. Applications thus need to except
     * that IllegalArgumentException is thrown in this method or during
     * play method call.
     * <p>
     * @param data a byte array containing the data to be played
     * @param type type of the audio
     * @throws java.lang.IllegalArgumentException if the data can not be
     recognized to
     * given type or the type is unsupported or unknown
     * @throws java.lang.NullPointerException if the data is null
     * @since 1.0
     */
    public void init(byte[] data, int type)
    {
        if (!(type == FORMAT_WAV || type == FORMAT_TONE))
        {
            throw(new IllegalArgumentException("Type is not supported"));
        }
        if (data == null)
        {
            throw(new NullPointerException("Data is null"));
        }

        if ((iState != SOUND_UNINITIALIZED) &&
                (iState != SOUND_REINITIALISING))
        {
            iState = SOUND_REINITIALISING;
            soundStateChanged(SOUND_UNINITIALIZED);
        } // end of if (iState != SOUND_UNINITIALIZED)

        iCurrentType = type;
        int err = _init(iHandle, iCurrentType, data, 0, 0);
        if (err == ERR_NOT_READY || err == ERR_ARGUMENT )
        {
            throw new IllegalArgumentException("Data is invalid");
        }

        iState = SOUND_STOPPED;
    }


    /**
     * Get the current state of the Sound object.
     *
     * @return current state, SOUND_PLAYING, SOUND_STOPPED or
     SOUND_UNINITIALIZED
     * @since 1.0
     *
     */
    public int getState()
    {
        if (iState == SOUND_REINITIALISING)
        {
            return SOUND_UNINITIALIZED;
        }

        iState = _getState(iHandle);
        switch (iState)
        {
        case(0):  // ENotReady
        case(4):  // EInitialising
            iState = SOUND_UNINITIALIZED;
            break;
        case(1):  // EReadyToPlay
            iState = SOUND_STOPPED;
            break;
        case(2):  // EPlaying
            iState = SOUND_PLAYING;
            break;
        default:
        }
        return iState;
    }

    /**
     * This method is used for starting the playback from the beginning of a
     * sound object. The loop parameter defined the loop count for playback.
     * Argument zero (0) means continuos looping. For uninitialized sound the
     * play method doesn't do anything and silently returns. For stopped and
     * playing sounds the playback starts from beginning of the sound with new
     * looping information.
     * <p>
     * This method will throw IllegalStateException if playback cannot be
     * started since all channels are in use, or playback is not possible
     * because there is more higher priority system sounds being played.
     * <p>
     * If Sound playback is possible this method will return immediately and
     * thus will not block the calling thread during the playback. If any error
     * that prevents the playback is encountered during the playback, the
     * playback is silently stopped as if called to the stop method.
     *
     * @param number number of times audio is played. Value 0 plays audio in
     * continous loop.
     * @throws java.lang.IllegalStateException if the sound object cannot be
     * played because all the channels are already in use.
     * @throws java.lang.IllegalArgumentException if the loop value is negative,
     * or if sound values/date is illegal or corrupted.
     * @since 1.0
     *
     */
    public void play(int loop) throws IllegalArgumentException
    {
        if (loop < 0)
        {
            throw(new IllegalArgumentException("Negative loop value"));
        }
        if (iState == SOUND_REINITIALISING)
        {
            return;
        } // end of if (iState == SOUND_REINITIALISING)

        if (iPlayingSound != null)
        {
            if (iPlayingSound.getState() == SOUND_PLAYING)
            {
                iPlayingSound.stop();
            }
        } // end of if (iPlayingSound != null)

        int error = _play(iHandle, loop);
        if ((error == NOT_SUPPORTED_ERROR))
        {
            throw(new IllegalArgumentException("Sound is not supported"));
        }
        iPlayingSound = this;
    }

    /**
     *  The method will stop the sound playback, storing the current position.
     *  For sound that has never been started (may be uninitialized), or is
     *  currently being stopped the method call doesn't do anything and returns
     *  silently.
     *
     *  Note that for tone based sounds it is not possible to resume from
     *  position the sound was stopped at, to be specific, stop will reset
     *  the position to the beginning of the sound.
     *  @since 1.0
     */
    public void stop()
    {
        if (iState == SOUND_REINITIALISING)
        {
            return;
        } // end of if (iState == SOUND_REINITIALISING)
        _stop(iHandle);
    }

    /**
     *  The method will continue the stopped sound object from the position it
     *  was stopped to. For sound that has never been started (may be
     *  uninitialized), or is currently being played the method call doesn't
     *  do anything.
     *  <p>
     *  Note: For tone based sounds the resume starts the sound from the
     *  beginning of the sound clip.
     *  @since 1.0
     *
     */
    public void resume()
    {
        if (iState == SOUND_REINITIALISING)
        {
            return;
        } // end of if (iState == SOUND_REINITIALISING)
        _resume(iHandle);
    }

    /**
     * Sets the gain for the sound object. The gain is a value between
     * 0 and 255. Implementation scales the gain value to the limits it
     * supports. Notice that any gain value > 0 should result a gain
     * value > 0. If the gain is smaller than this minimum value then
     * gain is set to 0, if the gain greater than this maximum value
     * then the gain is set to maximum value (255).
     *
     * @param gain gain value: 0 - 255
     * @throws java.lang.IllegalArgumentException if the gain not 0 - 255
     * @since 1.0
     */
    public void setGain(int gain)
    {
        if (iState == SOUND_REINITIALISING)
        {
            return;
        } // end of if (iState == SOUND_REINITIALISING)
        if (gain < 0)
        {
            gain = 0;
        }
        else if (gain > 255)
        {
            gain = 255;
        }
        iGain = gain;
        _setVolume(iHandle, iGain);
    }

    /**
     * Get the gain (or volume) of Sound object. The gain is a value
     * between 0 and 255. System returns a scaled value based on the
     * limits it supports. Notice that any system gain value > 0 should
     * return a gain value > 0.
     *
     * @return gain value 0 - 255
     * @since 1.0
     *
     */
    public int getGain()
    {
        if (iGain == -1)
        {
            return _volume(iHandle);
        }
        // we have previously set gain
        return iGain;
    }

    /**
     * Returns number of concurrent sounds the device can play for
     * specific audio type. Returns 1 if only one sound can be played
     * at a time. Notice that most types use same channel resources.
     * @return total number of available channels.
     * @param type the media type
     * @throws java.lang.IllegalArgumentException if the type is unsupported
     * or unknown
     * @since 1.0
     */
    public static int getConcurrentSoundCount(int type)
    {
        if ((type != FORMAT_TONE) && (type != FORMAT_WAV))
        {
            throw(new IllegalArgumentException("Type is not supported"));
        }

        return 1;
    }

    /**
     *  Returns the supported audio formats as an int array.
     *
     *  @return an array containing supported audio formats as
     *  int values (e.g. FORMAT_TONE, FORMAT_WAV),
     *  or an empty array if no audio formats are supported.
     *  @since 1.0
     */
    static public int[] getSupportedFormats()
    {
        return(new int[] { FORMAT_TONE, FORMAT_WAV });
    }

    /**
     *  Registeres a listener for playback state notifications.
     *  @see com.nokia.mid.sound.SoundListener
     *  @param listener a listener that is notified when state
     *  changes occur or null if listener is to be
     *  removed.
     *  @since 1.0
     *
     */
    public void setSoundListener(SoundListener listener)
    {
        iSoundListeners.addElement(listener);
    }

    /**
     * Callback method when sound state changes
     *
     */
    public void soundStateChanged(final int event)
    {
        /*
        for(int i = 0; i < iSoundListeners.size(); i++)
        {
          ((SoundListener)iSoundListeners.elementAt(i)).soundStateChanged(this, event);
        }
        */
        //Notify SoundState Listeners in a separate thread, so that application doesn't
        //block main thread
        new Thread(new Runnable()
        {
            public void run()
            {
                notifySoundStateListeners(event);
            }
        }).start();
    }

    /**
     * Notify Sound State Listeners
     */
    public synchronized void notifySoundStateListeners(int event)
    {
        for (int i = 0; i < iSoundListeners.size(); i++)
        {
            ((SoundListener)iSoundListeners.elementAt(i)).soundStateChanged(this, event);
        }
    }

    private native void _dispose(int aHandle);
    private native int _create();
    private native int _init(int aHandle, int aType,
                             byte[] aData,
                             int aFrequency, long aDuration);
    private native void _release(int aHandle);
    private native int _play(int aHandle, int aLoop);
    private native void _stop(int aHandle);
    private native void _resume(int aHandle);
    private native void _setVolume(int aHandle, int aVolume);
    private native int _volume(int aHandle);
    private native int _getState(int aHandle);

} //End of Sound class

