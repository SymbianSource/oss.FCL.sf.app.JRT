/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


package javax.microedition.sensor.control;



/**
 * Please refer JSR 256 for more details.
 */
public interface SampleRateControl extends Control
{


    /**
     * Please refer JSR 256 for more details. z
     */
    public float getSampleRate();


    /**
     * Please refer JSR 256 for more details.
     */
    public float[] getSampleRates();


    /**
     * Please refer JSR 256 for more details.
     */
    public void setSampleRate(float sampleRate);

}
