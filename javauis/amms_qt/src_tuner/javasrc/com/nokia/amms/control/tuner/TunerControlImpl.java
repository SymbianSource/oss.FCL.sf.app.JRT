/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class implements
 *                javax.microedition.amms.control.tuner.TunerControl
 *
*/


package com.nokia.amms.control.tuner;

import javax.microedition.media.MediaException;
import javax.microedition.media.Control;
import com.nokia.microedition.media.InternalPlayer;
import com.nokia.microedition.media.PlayerImpl;
import com.nokia.microedition.media.Locator;
import com.nokia.microedition.media.ManagerImpl;
import com.nokia.microedition.media.control.ControlImpl;
import com.nokia.mj.impl.rt.legacy.NativeError;

public class TunerControlImpl
        extends ControlImpl
        implements javax.microedition.amms.control.tuner.TunerControl
{
    //preset name max length
    private static final int MAX_PRESET_NAME_LENGTH = 100;
    //max number of presets
    private static final int MAX_NUMBER_OF_PRESETS = 20;
    //all frequencys are in 100 Hertzs
    private static final int HERTZ_MULTIPLIER = 100;

    /**
     * Constructor
     */
    public TunerControlImpl()
    {
        //JDEBUG( "AMMS TunerControlImpl Constructor" );
    }

    /**
     * Gets the minimum frequency supported by this tuner
     * with the given modulation.
     *
     * @param modulation The modulation whose supported minimum frequency
     * is asked.
     * @return The minimum frequency in 100 Hertzs.
     * @throws IllegalArgumentException if the <code>modulation</code> is not supported or it is null.
     */
    public int getMinFreq(String aModulation)
    {
        if (aModulation != MODULATION_FM)
        {
            throw new IllegalArgumentException("Modulation is not valid.");
        }

        int minFreq = _getMinFreq(iEventSource,
                                  iControlHandle);

        NativeError.check(minFreq);

        return minFreq / HERTZ_MULTIPLIER;
    }

    /**
     * Gets the maximum frequency supported by this tuner
     * with the given modulation.
     *
     * @param modulation The modulation whose supported maximum frequency
     * is asked.
     * @return The maximum frequency in 100 Hertzs.
     * @throws IllegalArgumentException if the <code>modulation</code> is not supported or it is null.
     */
    public int getMaxFreq(String aModulation)
    {
        if (aModulation != MODULATION_FM)
        {
            throw new IllegalArgumentException("Modulation is not valid.");
        }

        int maxFreq = _getMaxFreq(iEventSource,
                                  iControlHandle);

        NativeError.check(maxFreq);

        return maxFreq / HERTZ_MULTIPLIER;
    }

    /**
     * Tunes to the given frequency or to the closest supported frequency.
     *
     * @param freq The frequency in 100 Hertzs that will be taken into use.
     * If that frequency is not supported, the closest supported
     * frequency will be taken into use.
     *
     * @param modulation The modulation to be used. <code>TunerControl</code> specifies
     * predefined constants <code>MODULATION_FM</code> and <code>MODULATION_AM</code>
     * but other modulations can be supported as well. Supported modulations can be queried
     * by <code>System</code> property <code>tuner.modulations</code>.
     *
     * @throws IllegalArgumentException if <code>freq</code> is not inside the frequency band supported
     * by the device or if the <code>modulation</code> is not supported or the <code>modulation</code> is null.
     *
     * @return the frequency in 100 Hertzs that was taken into use.
     */
    public int setFrequency(int aFreq, String aModulation)
    {
        if (aModulation != MODULATION_FM)
        {
            throw new IllegalArgumentException("Modulation is not valid.");
        }

        int freqInUse = _setFrequency(iEventSource,
                                      iControlHandle,
                                      aFreq * HERTZ_MULTIPLIER);

        NativeError.check(freqInUse);

        return freqInUse / HERTZ_MULTIPLIER;
    }

    /**
     * Gets the frequency which the tuner has been tuned to.
     *
     * @return The frequency to which the device has been tuned, in 100 Hertzs.
     */
    public int getFrequency()
    {
        int freq = _getFrequency(iEventSource, iControlHandle);

        NativeError.check(freq);

        return freq / HERTZ_MULTIPLIER;
    }

    /**
     * <p>Seeks for the next broadcast signal. If the end of the Player's
     * frequency band is reached before a signal was found, the scan
     * continues from the other end until a signal is found or the
     * starting frequency is reached.</p>
     *
     * <p>After seeking, the frequency of the Player is the one that
     * was returned or if nothing was found, the original frequency.</p>
     *
     * @param startFreq the frequency in 100 Hertzs wherefrom the scan starts (inclusive)
     *
     * @param modulation The modulation to be used. <code>TunerControl</code> specifies
     * predefined constants <code>MODULATION_FM</code> and <code>MODULATION_AM</code>
     * but other modulations can be supported as well. Supported modulations can be queried
     * by <code>System</code> property <code>tuner.modulations</code>.
     * @param upwards if <code>true</code>, the scan proceeds towards higher frequencies,
     * otherwise towards lower frequencies
     *
     * @return The found frequency in 100 Hertzs or, if no signal was found, 0.
     *
     * @throws IllegalArgumentException if <code>startFreq</code> is not between the supported minimum
     * and maximum frequencies or if the <code>modulation</code> is null.
     * @throws MediaException if the seek functionality is not available for the given modulation.
     */
    public int seek(int aStartFreq, String aModulation, boolean aUpwards)
    throws MediaException
    {
        if (aModulation != MODULATION_FM)
        {
            throw new IllegalArgumentException("Modulation is not valid.");
        }

        int foundFreq = _seek(iEventSource,
                              iControlHandle,
                              aStartFreq * HERTZ_MULTIPLIER,
                              aUpwards);

        NativeError.check(foundFreq);

        return foundFreq / HERTZ_MULTIPLIER;
    }

    /**
     * Gets the current squelching (muting in frequencies without broadcast)
     * setting.
     *
     * @return <code>true</code> if squelch is on or <code>false</code> if squelch is off.
     */
    public boolean getSquelch()
    {
        int retValue = _getSquelch(iEventSource, iControlHandle);

        NativeError.check(retValue);

        if (retValue == NativeError.KErrNone)
        {
            return true;
        }

        return false;
    }

    /**
     * Sets squelching on or off. Squelching means muting the frequencies
     * that do not contain radio broadcast.
     *
     * @param squelch <code>true</code> to turn the squelch on or <code>false</code> to turn the squelch off.
     * @throws MediaException if the given squelch setting is not supported.
     */
    public void setSquelch(boolean aSquelch) throws MediaException
    {
        int err = _setSquelch(iEventSource, iControlHandle, aSquelch);

        NativeError.check(err);
    }

    /**
     * Gets the modulation in use.
     *
     * @return The modulation currently in use.
     */
    public String getModulation()
    {
        //only FM modulation is supported
        return MODULATION_FM;
    }

    /**
     * Gets the strength of the recepted signal.
     *
     * @return A value between 0 and 100 where 0 means the faintest and 100 the strongest possible signal strength.
     * @throws MediaException if querying the signal strength is not supported.
     */
    public int getSignalStrength() throws MediaException
    {
        int signalStrength = _getSignalStrength(iEventSource,
                                                iControlHandle);

        if (signalStrength != NativeError.KErrNone)
        {
            throw new MediaException("Signal strength is not supported.");
        }

        return signalStrength;
    }

    /**
     * Gets the stereo mode in use.
     *
     * @return The stereo mode in use. Stereo mode is one of <code>MONO</code>,
     * <code>STEREO</code> or <code>AUTO</code>.
     */
    public int getStereoMode()
    {
        int stereoMode = _getStereoMode(iEventSource, iControlHandle);

        NativeError.check(stereoMode);

        return stereoMode;
    }

    /**
     * Sets the stereo mode.
     *
     * @param mode The stereo mode to be used. Stereo mode is one of <code>MONO</code>,
     * <code>STEREO</code> or <code>AUTO</code>.
     * @throws IllegalArgumentException if the given mode is not supported.
     */
    public void setStereoMode(int aStereoMode)
    {
        if (aStereoMode != MONO && aStereoMode != STEREO && aStereoMode != AUTO)
        {
            throw new IllegalArgumentException();
        }

        int err = _setStereoMode(iEventSource, iControlHandle, aStereoMode);

        if (err != NativeError.KErrNone)
        {
            throw new IllegalArgumentException("Stereo mode is not supported.");
        }
    }

    /**
     * Gets the number of presets. The numbering of presets starts from one and the largest
     * preset number equals the value returned from this method.
     *
     * @return The number of presets, or zero if the presets are not supported.
     */
    public int getNumberOfPresets()
    {

        return 0;
    }

    /**
     * Tunes the tuner by using settings specified in the preset. Changes to
     * presets following a <code>usePreset</code> call do not tune the tuner automatically.
     *
     * @param preset the preset to be used.
     * @throws IllegalArgumentException if <code>preset</code> &lt 1 or <code>preset</code> &gt number of presets.
     */
    public void usePreset(int aPreset)
    {
        if (aPreset < 0 || aPreset > MAX_NUMBER_OF_PRESETS)
        {
            throw new IllegalArgumentException("Preset out of preset range.");
        }
        int err = _usePreset(iEventSource, iControlHandle, aPreset);

        NativeError.check(err);
    }

    /**
     * Configures the preset using current frequency and modulation
     * (and stereo mode if native presets support storing it).
     *
     * @param preset the preset to be set.
     * @throws IllegalArgumentException if <code>preset</code> &lt 1 or <code>preset</code> &gt number of preset range.
     * @throws SecurityException if setting presets has been prohibited.
     */
    public void setPreset(int aPreset)
    {
        if (aPreset < 0 || aPreset > MAX_NUMBER_OF_PRESETS)
        {
            throw new IllegalArgumentException("Preset out of preset range.");
        }
        int err = _setPreset(iEventSource, iControlHandle, aPreset);

        NativeError.check(err);
    }

    /**
     * Configures the preset using given settings.
     * The stereo mode might not be stored if it is not supported by the presets.
     * (In that case, <code>IllegalArgumentException</code> is not thrown.)
     *
     * @param preset the preset to be configured.
     * @param freq the frequency of the preset in 100 Hertzs.
     * @param mod the modulation of the preset.
     * @param stereoMode the stereo mode of the preset.
     * @throws IllegalArgumentException if <code>preset</code> &lt 1 or <code>preset</code> &gt number of presets or
     * <code>freq</code> or <code>modulation</code> are not available or if the <code>modulation</code> is null or if <code>stereoMode</code> is not a supported stereo mode.
     * @throws SecurityException if setting presets has been prohibited.
     */
    public void setPreset(int aPreset, int aFreq, String aModulation, int aStereoMode)
    {
        if (aPreset < 0 || aPreset > MAX_NUMBER_OF_PRESETS)
        {
            throw new IllegalArgumentException("Preset out of preset range.");
        }
        if (aModulation != MODULATION_FM)
        {
            throw new IllegalArgumentException("Modulation is not valid.");
        }
        if (aStereoMode != MONO && aStereoMode != STEREO && aStereoMode != AUTO)
        {
            throw new IllegalArgumentException("Stereo mode is not supported.");
        }

        int err = _setPreset(iEventSource, iControlHandle, aPreset, aFreq * HERTZ_MULTIPLIER, aStereoMode);

        NativeError.check(err);
    }

    /**
     * Gets the preset's frequency.
     *
     * @param preset the preset whose frequency is to be returned.
     * @return The frequency of the preset in 100 Hertzs.
     * @throws IllegalArgumentException if <code>preset</code> &lt 1 or <code>preset</code> &gt number of presets.
     */
    public int getPresetFrequency(int aPreset)
    {
        if (aPreset < 0 || aPreset > MAX_NUMBER_OF_PRESETS)
        {
            throw new IllegalArgumentException("Preset out of preset range.");
        }

        int presetFreq = _getPresetFrequency(iEventSource, iControlHandle, aPreset);

        NativeError.check(presetFreq);

        return presetFreq / HERTZ_MULTIPLIER;
    }

    /**
     * Gets the preset's modulation.
     *
     * @param preset the preset whose modulation is to be returned.
     * @return The modulation of the preset.
     * @throws IllegalArgumentException if <code>preset</code> &lt 1 or <code>preset</code> &gt number of presets.
     */
    public String getPresetModulation(int aPreset)
    {
        if (aPreset < 0 || aPreset > MAX_NUMBER_OF_PRESETS)
        {
            throw new IllegalArgumentException("Preset out of preset range.");
        }
        return MODULATION_FM;
    }

    /**
     * Gets the preset's stereo mode.
     *
     * @param preset the preset whose stereo mode is to be returned.
     * @return The stereo mode of the preset. Stereo mode is one of
     * <code>MONO</code>, <code>STEREO</code> or <code>AUTO</code>.
     * @throws IllegalArgumentException if <code>preset</code> &lt 1 or <code>preset</code> &gt number of presets.
     * @throws MediaException if the presets do not support storing of the stereo mode.
     */
    public int getPresetStereoMode(int aPreset) throws MediaException
    {
        if (aPreset < 0 || aPreset > MAX_NUMBER_OF_PRESETS)
        {
            throw new IllegalArgumentException("Preset out of preset range.");
        }

        int presetStereoMode = _getPresetStereoMode(iEventSource, iControlHandle, aPreset);

        NativeError.check(presetStereoMode);

        return presetStereoMode;
    }

    /**
     * Gets the preset name.
     *
     * @param preset the preset whose name is to be returned.
     * @return A <code>String</code> containing the preset name.
     * @throws IllegalArgumentException if <code>preset</code> &lt 1 or <code>preset</code> &gt number of presets.
     */
    public String getPresetName(int aPreset)
    {
        if (aPreset < 0 || aPreset > MAX_NUMBER_OF_PRESETS)
        {
            throw new IllegalArgumentException("Preset out of preset range.");
        }

        int[] error = new int[ 1 ];

        String presetName = _getPresetName(iEventSource, iControlHandle, aPreset, error);

        NativeError.check(error[ 0 ]);

        return presetName;
    }

    /**
     * Sets the preset name.
     *
     * @param preset the preset whose name is to be set.
     * @param name the name of the preset.
     * @throws IllegalArgumentException if <code>preset</code> &lt 1 or <code>preset</code> &gt number of presets or
     * if the <code>name</code> is null.
     * @throws SecurityException if setting presets has been prohibited.
     */
    public void setPresetName(int aPreset, String aName)
    {
        if (aPreset < 0 || aPreset > MAX_NUMBER_OF_PRESETS)
        {
            throw new IllegalArgumentException("Preset out of preset range.");
        }
        if (aName.length() > MAX_PRESET_NAME_LENGTH)
        {
            throw new IllegalArgumentException("Preset name too long");
        }

        int err = _setPresetName(iEventSource, iControlHandle, aPreset, aName);

        NativeError.check(err);
    }


    private native int _getMinFreq(int aEventSourceHandle,
                                   int aTunerControlHandle);

    private native int _getMaxFreq(int aEventSourceHandle,
                                   int aTunerControlHandle);

    private native int _setFrequency(int aEventSourceHandle,
                                     int aTunerControlHandle,
                                     int aFreq);

    private native int _getFrequency(int aEventSourceHandle,
                                     int aTunerControlHandle);


    private native int _seek(int aEventSourceHandle,
                             int aTunerControlHandle,
                             int aStartFreq,
                             boolean aUpwards);

    private native int _getSquelch(int aEventSourceHandle,
                                   int aTunerControlHandle);

    private native int _setSquelch(int aEventSourceHandle,
                                   int aTunerControlHandle,
                                   boolean aSquelch);

    private native int _getSignalStrength(int aEventSourceHandle,
                                          int aTunerControlHandle);

    private native int _getStereoMode(int aEventSourceHandle,
                                      int aTunerControlHandle);

    private native int _setStereoMode(int aEventSourceHandle,
                                      int aTunerControlHandle,
                                      int aStereoMode);

    private native int _usePreset(int aEventSourceHandle,
                                  int aTunerControlHandle,
                                  int aPreset);

    private native int _setPreset(int aEventSourceHandle,
                                  int aTunerControlHandle,
                                  int aPreset);

    private native int _setPreset(int aEventSourceHandle,
                                  int aTunerControlHandle,
                                  int aPreset,
                                  int aFreq,
                                  int aStereoMode);

    private native int _getPresetFrequency(int aEventSourceHandle,
                                           int aTunerControlHandle,
                                           int aPreset);

    private native int _getPresetStereoMode(int aEventSourceHandle,
                                            int aTunerControlHandle,
                                            int aPreset);

    private native int _setPresetName(int aEventSourceHandle,
                                      int aTunerControlHandle,
                                      int aPreset,
                                      String aName);

    private native String _getPresetName(int aEventSourceHandle,
                                         int aTunerControlHandle,
                                         int aPreset,
                                         int[] aError);


}
