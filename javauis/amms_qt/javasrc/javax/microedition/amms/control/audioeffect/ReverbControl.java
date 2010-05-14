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
package javax.microedition.amms.control.audioeffect;

import javax.microedition.media.*;
import javax.microedition.amms.control.*;

/**
 * Please refer to JSR 234 for more details.
 */
public interface ReverbControl extends EffectControl
{

    /**
     * Please refer to JSR 234 for more details.
     */
    int setReverbLevel(int level) throws IllegalArgumentException;

    /**
     * Please refer to JSR 234 for more details.
     */
    int getReverbLevel();


    /**
     * Please refer to JSR 234 for more details.
     */
    void setReverbTime(int time) throws IllegalArgumentException,
                javax.microedition.media.MediaException;


    /**
     * Please refer to JSR 234 for more details.
     */
    int getReverbTime() throws
                javax.microedition.media.MediaException;

}
