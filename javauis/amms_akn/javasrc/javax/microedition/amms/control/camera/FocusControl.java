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
public interface FocusControl extends Control
{

    /**
     * Please refer to JSR 234 for more details.
     */
    public final static int AUTO = -1000;

    /**
     * Please refer to JSR 234 for more details.
     */
    public final static int AUTO_LOCK = -1005;

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
    public int setFocus(int distance) throws MediaException;

    /**
     * Please refer to JSR 234 for more details.
     */
    public int getFocus();

    /**
     * Please refer to JSR 234 for more details.
     */
    public int getMinFocus();

    /**
     * Please refer to JSR 234 for more details.
     */
    public int getFocusSteps();

    /**
     * Please refer to JSR 234 for more details.
     */
    public boolean isManualFocusSupported();


    /**
     * Please refer to JSR 234 for more details.
     */
    public boolean isAutoFocusSupported();


    /**
     * Please refer to JSR 234 for more details.
     */
    public boolean isMacroSupported();

    /**
     * Please refer to JSR 234 for more details.
     */
    public void setMacro(boolean enable) throws MediaException;

    /**
     * Please refer to JSR 234 for more details.
     */
    public boolean getMacro();

}
