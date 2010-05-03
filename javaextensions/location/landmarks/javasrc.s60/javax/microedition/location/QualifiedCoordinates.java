/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  JSR-179 Location API version 1.0.1 QualifiedCoordinates class
 *
*/


// PACKAGE
package javax.microedition.location;

// CLASS DESCRIPTION
/**
 * JSR-179 Location API version 1.0.1 QualifiedCoordinates class
 *
 * Please refer JSR-179 spec
 */
public class QualifiedCoordinates extends Coordinates
{

    // Error message for illegal accuracy values
    private static final String ILLEGAL_ACCURACY_VALUE
    = "Illegal accuracy value: ";

    // Member data
    private float iHorizontalAccuracy = Float.NaN;

    private float iVerticalAccuracy = Float.NaN;

    /**
     * Please refer JSR-179 spec
     */
    public QualifiedCoordinates(double aLatitude, double aLongitude,
                                float aAltitude, float aHorizontalAccuracy, float aVerticalAccuracy)
    {
        super(aLatitude, aLongitude, aAltitude);
        setHorizontalAccuracy(aHorizontalAccuracy);
        setVerticalAccuracy(aVerticalAccuracy);
    }

    /**
     * Returns the horizontal accuracy of the location
     */
    public float getHorizontalAccuracy()
    {
        return iHorizontalAccuracy;
    }

    /**
     * Returns the vertical accuracy of the location
     */
    public float getVerticalAccuracy()
    {
        return iVerticalAccuracy;
    }

    /**
     * Sets the horizontal accuracy of the location
     */
    public void setHorizontalAccuracy(float aHorizontalAccuracy)
    {
        // Validate the argument
        checkValue(aHorizontalAccuracy);
        // The argument was valid so we can store it
        iHorizontalAccuracy = aHorizontalAccuracy;
    }

    /**
     * Sets the vertical accuracy of the location
     */
    public void setVerticalAccuracy(float aVerticalAccuracy)
    {
        // Validate the argument
        checkValue(aVerticalAccuracy);
        // The argument was valid so we can store it
        iVerticalAccuracy = aVerticalAccuracy;
    }

    /**
     * Validates the passed value
     */
    private void checkValue(float aValue)
    {
        if (!Float.isNaN(aValue) && aValue < 0)
        {
            throw new IllegalArgumentException(ILLEGAL_ACCURACY_VALUE + aValue);
        }
    }
}

// End of file
