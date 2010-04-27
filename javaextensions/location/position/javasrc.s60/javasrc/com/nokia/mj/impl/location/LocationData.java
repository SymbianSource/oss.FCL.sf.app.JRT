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
* Description:  Extends the Location API standard Location class
 *
*/


// PACKAGE
package com.nokia.mj.impl.location;

// IMPORTS
import javax.microedition.location.*;
import javax.microedition.location.QualifiedCoordinates;
import javax.microedition.location.AddressInfo;
import javax.microedition.location.Coordinates;

// CLASS DESCRIPTION
/**
 * Extends the Location API standard Location class to provide
 * QualifiedCoordinates, timestamp and NMEA coordinates of the Location
 */
public final class LocationData extends Location
{
    private int iMethod;

    private QualifiedCoordinates iCoords = null;

    private long iTimestamp;

    private float iSpeed = Float.NaN;

    private float iCourse = Float.NaN;

    private AddressInfo iAddress;

    private String iExtraInfo;

    public LocationData(int aMethod)
    {
        iMethod = aMethod;
    }

    public void setLocationData(QualifiedCoordinates aCoords, long aTimestamp,
                                String aNmeaData)
    {
        iCoords = aCoords;
        iTimestamp = aTimestamp;
        iExtraInfo = aNmeaData;
    }

    public void setSpeedAndCourse(float aSpeed, float aCourse)
    {
        iSpeed = aSpeed;
        iCourse = aCourse;
    }

    public void setAddressInfo(AddressInfo aAddress)
    {
        iAddress = aAddress;
    }

    public boolean isValid()
    {
        return iCoords != null;
    }

    public long getTimestamp()
    {
        return iTimestamp;
    }

    public QualifiedCoordinates getQualifiedCoordinates()
    {
        return iCoords;
    }

    public float getSpeed()
    {
        return iSpeed;
    }

    public float getCourse()
    {
        return iCourse;
    }

    public int getLocationMethod()
    {
        return iMethod;
    }

    public AddressInfo getAddressInfo()
    {
        return iAddress;
    }

    public String getExtraInfo(String mimetype)
    {
        if ("application/X-jsr179-location-nmea".equals(mimetype))
        {
            return iExtraInfo;
        }
        return null;
    }
}

// End of file
