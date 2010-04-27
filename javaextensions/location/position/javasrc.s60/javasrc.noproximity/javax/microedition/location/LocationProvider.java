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
* Description:  JSR-179 Location API LocationProvider class
 *
*/


// PACKAGE
package javax.microedition.location;

// IMPORTS
import com.nokia.mj.impl.location.LocationProviderImpl;

// CLASS DESCRIPTION
/**
 * J2ME JSR-179 Location API 1.0.1 complying LocationProvider class
 */
public abstract class LocationProvider
{
    public static final int AVAILABLE = 1;

    public static final int TEMPORARILY_UNAVAILABLE = 2;

    public static final int OUT_OF_SERVICE = 3;

    protected LocationProvider()
    {
    }

    public abstract int getState();

    public static LocationProvider getInstance(Criteria criteria)
    throws LocationException
    {
        LocationProviderImpl lp = new LocationProviderImpl();
        if (lp.selectModule(criteria))
        {
            return lp;
        }
        else
        {
            return null;
        }
    }

    public abstract Location getLocation(int timeout) throws LocationException,
                InterruptedException;

    public abstract void setLocationListener(LocationListener listener,
            int interval, int timeout, int maxAge);

    public abstract void reset();

    public static Location getLastKnownLocation()
{
        return LocationProviderImpl.getLastKnownLocation();
    }

    public static void addProximityListener(ProximityListener listener,
                                            Coordinates coordinates, float proximityRadius)
    throws LocationException
    {

        throw new LocationException();
    }

    public static void removeProximityListener(ProximityListener listener)
    {
    }
}

// End of file
