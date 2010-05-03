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
public interface ChorusControl extends EffectControl
{

    /**
     * Please refer to JSR 234 for more details.
     */
    public int setWetLevel(int level);

    /**
     * Please refer to JSR 234 for more details.
     *
     */
    public int getWetLevel();

    /**
     * Please refer to JSR 234 for more details.
     */
    void setModulationRate(int rate);

    /**
     * Please refer to JSR 234 for more details.
     */
    int getModulationRate();

    /**
     * Please refer to JSR 234 for more details.
     */
    int getMinModulationRate();

    /**
     * Please refer to JSR 234 for more details.
     */
    int getMaxModulationRate();

    /**
     * Please refer to JSR 234 for more details.
     */
    void setModulationDepth(int percentage);


    /**
     * Please refer to JSR 234 for more details.
     */
    int getModulationDepth();

    /**
     * Please refer to JSR 234 for more details.
     */
    int getMaxModulationDepth();


    /**
     * Please refer to JSR 234 for more details.
     */
    void setAverageDelay(int delay);

    /**
     * Please refer to JSR 234 for more details.
     */
    int getAverageDelay();


    /**
     * Please refer to JSR 234 for more details.
     */
    int getMaxAverageDelay();


}
