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
package javax.microedition.amms.control.imageeffect;

import javax.microedition.media.MediaException;
import javax.microedition.media.Control;
import javax.microedition.amms.control.EffectControl;

/**
 * Please refer to JSR 234 for more details.
 */
public interface WhiteBalanceControl extends EffectControl
{

    /**
     * Please refer to JSR 234 for more details.
     */
    public final static int AUTO = -1000;

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
    int setColorTemp(int temp);

    /**
     * Please refer to JSR 234 for more details.
     */
    int getColorTemp();

    /**
     * Please refer to JSR 234 for more details.
     */
    int getMinColorTemp();


    /**
     * Please refer to JSR 234 for more details.
     */
    int getMaxColorTemp();

    /**
     * Please refer to JSR 234 for more details.
     */
    public int getNumberOfSteps();

}
