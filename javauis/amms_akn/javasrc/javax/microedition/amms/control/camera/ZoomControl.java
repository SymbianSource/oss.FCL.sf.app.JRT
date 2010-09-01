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
public interface ZoomControl extends Control
{

    /**
     * Please refer to JSR 234 for more details.
     */
    public final static int NEXT = -1001;

    /**
     * Please refer to JSR 234 for more details.
     */
    public final static int PREVIOUS = -1002;

    /**
     * Please refer to JSR 234 for more details.
     */
    public final static int UNKNOWN = -1004;

    /**
     * Please refer to JSR 234 for more details.
     */
    public int setOpticalZoom(int level);

    /**
     * Please refer to JSR 234 for more details.
     */
    public int getOpticalZoom();


    /**
     * Please refer to JSR 234 for more details.
     */
    public int getMaxOpticalZoom();

    /**
     * Please refer to JSR 234 for more details.
     */
    public int getOpticalZoomLevels();


    /**
     * Please refer to JSR 234 for more details.
     */
    public int getMinFocalLength();

    /**
     * Please refer to JSR 234 for more details.
     */
    public int setDigitalZoom(int level);

    /**
     * Please refer to JSR 234 for more details.
     */
    public int getDigitalZoom();


    /**
     * Please refer to JSR 234 for more details.
     */
    public int getMaxDigitalZoom();


    /**
     * Please refer to JSR 234 for more details.
     */
    public int getDigitalZoomLevels();

}
