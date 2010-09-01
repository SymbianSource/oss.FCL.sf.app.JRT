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
public interface FlashControl extends Control
{

    /**
     * Please refer to JSR 234 for more details.
     */
    public final static int OFF = 1;

    /**
     * Please refer to JSR 234 for more details.
     */
    public final static int AUTO = 2;

    /**
     * Please refer to JSR 234 for more details.
     */
    public final static int AUTO_WITH_REDEYEREDUCE = 3;

    /**
     * Please refer to JSR 234 for more details.
     */
    public final static int FORCE = 4;

    /**
     * Please refer to JSR 234 for more details.
     */
    public final static int FORCE_WITH_REDEYEREDUCE = 5;

    /**
     * Please refer to JSR 234 for more details.
     */
    public final static int FILLIN = 6;

    /**
     * Please refer to JSR 234 for more details.
     */
    public int[] getSupportedModes();

    /**
     * Please refer to JSR 234 for more details.
     */
    public void setMode(int mode);

    /**
     * Please refer to JSR 234 for more details.
     */
    public int getMode();

    /**
     * Please refer to JSR 234 for more details.
     */
    public boolean isFlashReady();

}
