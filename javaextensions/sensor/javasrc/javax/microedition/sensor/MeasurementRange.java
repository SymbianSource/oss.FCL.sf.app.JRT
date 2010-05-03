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
public class MeasurementRange
{

    private final double largest;
    private final double smallest;
    private final double resolution;

    /**
     * Please refer JSR 256 for more details.
     */
    public MeasurementRange(double smallest, double largest, double resolution)
    {
        if (smallest>largest)
        {
            throw new IllegalArgumentException();
        }
        if (resolution<0)
        {
            throw new IllegalArgumentException();
        }
        this.smallest = smallest;
        this.largest = largest;
        this.resolution = resolution;
    }




    /**
     * Please refer JSR 256 for more details.
     */
    public double getLargestValue()
    {
        return largest;
    }

    /**
     * Please refer JSR 256 for more details.
     */
    public double getResolution()
    {
        return resolution;
    }


    /**
     * Please refer JSR 256 for more details.
     */
    public double getSmallestValue()
    {
        return smallest;
    }
}
