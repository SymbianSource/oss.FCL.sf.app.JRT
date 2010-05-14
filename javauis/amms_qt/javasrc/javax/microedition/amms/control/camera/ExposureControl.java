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
package javax.microedition.amms.control.camera;

import javax.microedition.media.MediaException;
import javax.microedition.media.Control;

/**
 * Please refer to JSR 234 for more details.
 *

 *
 * @see CameraControl#setExposureMode
 */
public interface ExposureControl extends Control
{

    /**
     * Please refer to JSR 234 for more details.
     */
    public int[] getSupportedFStops();

    /**
     * Please refer to JSR 234 for more details.
     */
    public int getFStop();


    /**
     * Please refer to JSR 234 for more details.
     */
    public void setFStop(int aperture) throws MediaException;

    /**
     * Please refer to JSR 234 for more details.
     */
    public int getMinExposureTime();


    /**
     * Please refer to JSR 234 for more details.
     */
    public int getMaxExposureTime();

    /**
     * Please refer to JSR 234 for more details.
     */
    public int getExposureTime();


    /**
     * Please refer to JSR 234 for more details.
     */
    public int setExposureTime(int time) throws MediaException;



    /**
     * Please refer to JSR 234 for more details.
     */
    public int[] getSupportedISOs();


    /**
     * Please refer to JSR 234 for more details.
     */
    public int getISO();

    /**
     * Please refer to JSR 234 for more details.
     */
    public void setISO(int iso) throws MediaException;



    /**
     * Please refer to JSR 234 for more details.
     */
    public int[] getSupportedExposureCompensations();

    /**
     * Please refer to JSR 234 for more details.
     */
    public int getExposureCompensation();

    /**
     * Please refer to JSR 234 for more details.
     */
    public void setExposureCompensation(int ec) throws MediaException;


    /**
     * Please refer to JSR 234 for more details.
     */
    public int getExposureValue();



    /**
     * Please refer to JSR 234 for more details.
     */
    public String[] getSupportedLightMeterings();

    /**
     * Please refer to JSR 234 for more details.
     */
    public void setLightMetering(String metering);

    /**
     * Please refer to JSR 234 for more details.
     */
    public String getLightMetering();


}

