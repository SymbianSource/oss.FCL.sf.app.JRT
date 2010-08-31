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
package javax.microedition.amms.control;

import javax.microedition.media.*;

/**
 * Please refer JSR 234 for more details.
 */
public interface FormatControl extends Control
{

    /**
     * Please refer JSR 234 for more details.
     */
    public final static int METADATA_NOT_SUPPORTED = 0;

    /**
     * Please refer JSR 234 for more details.
     */
    public final static int METADATA_SUPPORTED_FIXED_KEYS = 1;

    /**
     * Please refer JSR 234 for more details.
     */
    public final static int METADATA_SUPPORTED_FREE_KEYS = 2;

    /**
     * Please refer JSR 234 for more details.
     */
    public final static String PARAM_BITRATE = "bitrate";

    /**
     * Please refer JSR 234 for more details.
     */
    public final static String PARAM_BITRATE_TYPE = "bitrate type";

    /**
     * Please refer JSR 234 for more details.
     */
    public final static String PARAM_SAMPLERATE = "sample rate";

    /**
     * Please refer JSR 234 for more details.
     */
    public final static String PARAM_FRAMERATE = "frame rate";


    /**
     * Please refer JSR 234 for more details.
     */
    public final static String PARAM_QUALITY = "quality";

    /**
     * Please refer JSR 234 for more details.
     */
    public final static String PARAM_VERSION_TYPE = "version type";



    /**
     * Please refer JSR 234 for more details.
     */
    String[] getSupportedFormats();

    /**
     * Please refer JSR 234 for more details.
     */
    String[] getSupportedStrParameters();

    /**
     * Please refer JSR 234 for more details.
     */
    String[] getSupportedIntParameters();


    /**
     * Please refer JSR 234 for more details.
     */
    String[] getSupportedStrParameterValues(String parameter);


    /**
     * Please refer JSR 234 for more details.
     */
    int[] getSupportedIntParameterRange(String parameter);

    /**
     * Please refer JSR 234 for more details.
     */
    void setFormat(String format);

    /**
     * Please refer JSR 234 for more details.
     */
    String getFormat();

    /**
     * Please refer JSR 234 for more details.
     */
    int setParameter(String parameter, int value);

    /**
     * Please refer JSR 234 for more details.
     */
    void setParameter(String parameter, String value);


    /**
     * Please refer JSR 234 for more details.
     */
    String getStrParameterValue(String parameter);

    /**
     *Please refer JSR 234 for more details.
     */
    int getIntParameterValue(String parameter);

    /**
     * Please refer JSR 234 for more details.
     */
    int getEstimatedBitRate() throws MediaException;


    /**
     * Please refer JSR 234 for more details.
     */
    void setMetadata(String key, String value) throws MediaException;

    /**
     * Please refer JSR 234 for more details.
     */
    String[] getSupportedMetadataKeys();

    /**
     * Please refer JSR 234 for more details.
     */
    int getMetadataSupportMode();

    /**
     * Please refer JSR 234 for more details.
     */
    void setMetadataOverride(boolean override);

    /**
     * Please refer JSR 234 for more details.
     */
    boolean getMetadataOverride();

}
