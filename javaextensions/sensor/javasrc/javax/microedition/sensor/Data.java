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


package javax.microedition.sensor;



/**
 * Please refer JSR 256 for more details.
 */
public interface Data
{


    /**
     * Please refer JSR 256 for more details.
     */
    public ChannelInfo getChannelInfo();


    /**
     * Please refer JSR 256 for more details.
     */
    public double[] getDoubleValues();

    /**
     * Please refer JSR 256 for more details.
     */
    public int[] getIntValues();

    /**
     * Please refer JSR 256 for more details.
     */
    public long getTimestamp(int index);

    /**
     * Please refer JSR 256 for more details.
     */
    public float getUncertainty(int index);

    /**
     * Please refer JSR 256 for more details.
     */
    public Object[] getObjectValues();

    /**
     * Please refer JSR 256 for more details.
     */
    public boolean isValid(int index);
}