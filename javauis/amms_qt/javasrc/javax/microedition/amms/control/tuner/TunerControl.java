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
package javax.microedition.amms.control.tuner;

import javax.microedition.media.MediaException;
import javax.microedition.media.Control;

/**
 * 
 * Please refer to JSR 234 for more details.
 */

public interface TunerControl extends Control
{
    /**
     * Please refer to JSR 234 for more details.
     */
    public static final int MONO = 1;

    /**
     * Please refer to JSR 234 for more details.
     */
    public static final int STEREO = 2;

    /**
     * Please refer to JSR 234 for more details.
     */
    public static final int AUTO = 3;

    /**
     * Please refer to JSR 234 for more details.
     */
    public static final String MODULATION_FM = "fm";

    /**
     * Please refer to JSR 234 for more details.
     */
    public static final String MODULATION_AM = "am";

    /**
     * Please refer to JSR 234 for more details.
     */
    public int getMinFreq(String modulation);

    /**
     * Please refer to JSR 234 for more details.
     */
    public int getMaxFreq(String modulation);

    /**
     * Please refer to JSR 234 for more details.
     */
    public int setFrequency(int freq, String modulation);

    /**
     * Please refer to JSR 234 for more details.
     */
    public int getFrequency();

    /**
     * Please refer to JSR 234 for more details.
     */
    public int seek(int startFreq, String modulation, boolean upwards) throws MediaException;

    /**
     * Please refer to JSR 234 for more details.
     */
    public boolean getSquelch();

    /**
     * Please refer to JSR 234 for more details.
     */
    public void setSquelch(boolean squelch) throws MediaException;

    /**
     * Please refer to JSR 234 for more details.
     */
    public String getModulation();

    /**
     * Please refer to JSR 234 for more details.
     */
    public int getSignalStrength() throws MediaException;

    /**
     * Please refer to JSR 234 for more details.
     */
    public int getStereoMode();

    /**
     * Please refer to JSR 234 for more details.
     */
    public void setStereoMode(int mode);

    /**
     * Please refer to JSR 234 for more details.
     */
    public int getNumberOfPresets();

    /**
     * Please refer to JSR 234 for more details.
     */
    public void usePreset(int preset);

    /**
     * Please refer to JSR 234 for more details.
     */
    public void setPreset(int preset);

    /**
     * Please refer to JSR 234 for more details.
     */
    public void setPreset(int preset, int freq, String mod, int stereoMode);

    /**
     * Please refer to JSR 234 for more details.
     */
    public int getPresetFrequency(int preset);

    /**
     * Please refer to JSR 234 for more details.
     */
    public String getPresetModulation(int preset);

    /**
     * Please refer to JSR 234 for more details.
     */
    public int getPresetStereoMode(int preset) throws MediaException;

    /**
     * Please refer to JSR 234 for more details.
     */
    public String getPresetName(int preset);

    /**
     * Please refer to JSR 234 for more details.
     */
    public void setPresetName(int preset, String name);
}
