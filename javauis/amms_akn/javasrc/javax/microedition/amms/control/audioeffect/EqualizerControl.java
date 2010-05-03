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

import javax.microedition.amms.control.*;

/**
 * Please refer to JSR 234 for more details.
 */

public interface EqualizerControl
            extends EffectControl
{

    public final int UNDEFINED = -1004;


    /**
     * Please refer to JSR 234 for more details.
     */
    int getMinBandLevel();

    /**
     * Please refer to JSR 234 for more details.
     */
    int getMaxBandLevel();

    /**
     * Please refer to JSR 234 for more details.
     */
    void setBandLevel(int level, int band) throws IllegalArgumentException;

    /**
     * Please refer to JSR 234 for more details.
     */
    int getBandLevel(int band) throws IllegalArgumentException;

    /**
     * Please refer to JSR 234 for more details.
     */
    int getNumberOfBands();

    /**
     * Please refer to JSR 234 for more details.
     */
    int getCenterFreq(int band) throws IllegalArgumentException;

    /**
     * Please refer to JSR 234 for more details.
     */
    int getBand(int frequency);

    /**
     * Please refer to JSR 234 for more details.
    */
    int setBass(int level) throws IllegalArgumentException;

    /**
     * Please refer to JSR 234 for more details.
     */
    int setTreble(int level) throws IllegalArgumentException;

    /**
     * Please refer to JSR 234 for more details.
     */
    int getBass();

    /**
     * Please refer to JSR 234 for more details.
     */
    int getTreble();

}
