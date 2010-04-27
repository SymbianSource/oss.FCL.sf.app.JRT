/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  JSR-179 Location API Criteria class
 *
*/


// PACKAGE
package javax.microedition.location;

// CLASS DESCRIPTION
/**
 * J2ME JSR-179 Location API 1.0.1 complying Criteria class
 */
public class Criteria
{
    public static final int NO_REQUIREMENT = 0;

    public static final int POWER_USAGE_LOW = 1;

    public static final int POWER_USAGE_MEDIUM = 2;

    public static final int POWER_USAGE_HIGH = 3;

    private int iHorizontalAccuracy = NO_REQUIREMENT;

    private int iVerticalAccuracy = NO_REQUIREMENT;

    private int iResponseTime = NO_REQUIREMENT;

    private int iPowerConsumption = NO_REQUIREMENT;

    private boolean iCostAllowed = true;

    private boolean iSpeedRequired = false;

    private boolean iAltitudeRequired = false;

    private boolean iAddressInfoRequired = false;

    public Criteria()
    {
    }

    public int getPreferredPowerConsumption()
    {
        return iPowerConsumption;
    }

    public boolean isAllowedToCost()
    {
        return iCostAllowed;
    }

    public int getVerticalAccuracy()
    {
        return iVerticalAccuracy;
    }

    public int getHorizontalAccuracy()
    {
        return iHorizontalAccuracy;
    }

    public int getPreferredResponseTime()
    {
        return iResponseTime;
    }

    public boolean isSpeedAndCourseRequired()
    {
        return iSpeedRequired;
    }

    public boolean isAltitudeRequired()
    {
        return iAltitudeRequired;
    }

    public boolean isAddressInfoRequired()
    {
        return iAddressInfoRequired;
    }

    public void setHorizontalAccuracy(int accuracy)
    {
        this.iHorizontalAccuracy = accuracy;
    }

    public void setVerticalAccuracy(int accuracy)
    {
        this.iVerticalAccuracy = accuracy;
    }

    public void setPreferredResponseTime(int time)
    {
        this.iResponseTime = time;
    }

    public void setPreferredPowerConsumption(int level)
    {
        this.iPowerConsumption = level;
    }

    public void setCostAllowed(boolean costAllowed)
    {
        this.iCostAllowed = costAllowed;
    }

    public void setSpeedAndCourseRequired(boolean speedAndCourseRequired)
    {
        this.iSpeedRequired = speedAndCourseRequired;
    }

    public void setAltitudeRequired(boolean altitudeRequired)
    {
        this.iAltitudeRequired = altitudeRequired;
    }

    public void setAddressInfoRequired(boolean addressInfoRequired)
    {
        this.iAddressInfoRequired = addressInfoRequired;
    }

}

// End of file
