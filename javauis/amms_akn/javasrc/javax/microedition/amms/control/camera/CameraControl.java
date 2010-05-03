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
 */
public interface CameraControl extends Control
{

    /**
     * Please refer to JSR 234 for more details.
     */
    public final static int ROTATE_LEFT = 2;

    /**
     * Please refer to JSR 234 for more details.
     */
    public final static int ROTATE_RIGHT = 3;

    /**
     * Please refer to JSR 234 for more details.
     */
    public final static int ROTATE_NONE = 1;

    /**
     * Please refer to JSR 234 for more details.
     */
    public final static int UNKNOWN = -1004;

    /**
     * Please refer to JSR 234 for more details.
     */
    public int getCameraRotation();


    /**
     * Please refer to JSR 234 for more details.
     */
    public void enableShutterFeedback(boolean enable) throws MediaException;

    /**
     * Please refer to JSR 234 for more details.
     */
    public boolean isShutterFeedbackEnabled();

    /**
     * Please refer to JSR 234 for more details.
     */
    public String[] getSupportedExposureModes();

    /**
     * Please refer to JSR 234 for more details.
     */
    public void setExposureMode(String mode);

    /**
     * Please refer to JSR 234 for more details.
     */
    public String getExposureMode();


    /**
     * Please refer to JSR 234 for more details.
     */
    public int[] getSupportedVideoResolutions();

    /**
     * Please refer to JSR 234 for more details.
     */
    public int[] getSupportedStillResolutions();

    /**
     * Please refer to JSR 234 for more details.
     */
    public void setVideoResolution(int index);

    /**
     * Please refer to JSR 234 for more details.
     */
    public void setStillResolution(int index);


    /**
     * Please refer to JSR 234 for more details.
     */
    public int getVideoResolution();

    /**
     * Please refer to JSR 234 for more details.
     */
    public int getStillResolution();
}

