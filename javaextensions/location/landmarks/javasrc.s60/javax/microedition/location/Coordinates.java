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
* Description:  JSR-179 Location API version 1.0.1 Coordinates class
 *
*/


// PACKAGE
package javax.microedition.location;

// CLASS DESCRIPTION
/**
 * JSR-179 Location API version 1.0.1 Coordinates class
 *
 * Please refer JSR-179 spec.
 *
 */
import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.utils.Logger;

public class Coordinates
{
    static
    {
        try
        {
            Jvm.loadSystemLibrary("javalocation");
        }
        catch (Exception e)
        {
            Logger.ELOG(Logger.EJavaLocation,
                        "Unable to load javalocation dll.");
        }
    }

    /**
     * Please refer JSR-179 spec
     */
    public static final int DD_MM_SS = 1;

    /**
     * Please refer JSR-179 spec
     */
    public static final int DD_MM = 2;

    // Private member coordinates. By default all are initialized to NaN
    private double iLatitude = Double.NaN;

    private double iLongitude = Double.NaN;

    private float iAltitude = Float.NaN;

    // Package private constants indicating the correct mininum/maximum values
    static final double MIN_LATITUDE = -90d;

    static final double MAX_LATITUDE = 90d;

    static final double MIN_LONGITUDE = -180d;

    static final double MAX_LONGITUDE = 180d;

    private static final float BEARING_ZERO = 0.0f;

    private static final float BEARING_HALF = 180.0f;

    // Error messages
    // Altitude is out of the supported range
    private static final String LATITUDE_OUT_OF_RANGE
    = "Latitude argument is out of range: ";

    // Longitude is out of the supported range
    private static final String LONGITUDE_OUT_OF_RANGE
    = "Longitude argument is out of range: ";

    private static final String COORDINATES_NULL_ERROR = "Coordinates is null";

    private static final String INVALID_COORDINATES_ERROR
    = "Coordinate is invalid: ";

    private static final String INVALID_OUTPUT_TYPE_ERROR
    = "Output type is invalid: ";

    /**
     * Please refer JSR-179 spec
     */
    public Coordinates(double aLatitude, double aLongitude, float aAltitude)
    {
        setLatitude(aLatitude);
        setLongitude(aLongitude);
        iAltitude = aAltitude;
    }

    /**
     * Returns the altitude of the location
     */
    public float getAltitude()
    {
        return iAltitude;
    }

    /**
     * Returns the latitude of the location
     */
    public double getLatitude()
    {
        return iLatitude;
    }

    /**
     * Returns the longitude of the location
     */
    public double getLongitude()
    {
        return iLongitude;
    }

    /**
     * Sets the altitude of the location
     */
    public void setAltitude(float aAltitude)
    {
        iAltitude = aAltitude;
    }

    /**
     * Sets the latitude of the location
     */
    public void setLatitude(double aLatitude)
    {
        if (Double.isNaN(aLatitude) || aLatitude < MIN_LATITUDE
                || aLatitude > MAX_LATITUDE)
        {
            throw new
            IllegalArgumentException(LATITUDE_OUT_OF_RANGE + aLatitude);
        }

        iLatitude = aLatitude;
    }

    /**
     * Sets the longitude of the location
     */
    public void setLongitude(double aLongitude)
    {
        if (Double.isNaN(aLongitude) || aLongitude < MIN_LONGITUDE
                || aLongitude >= MAX_LONGITUDE)
        {
            throw new
            IllegalArgumentException(LONGITUDE_OUT_OF_RANGE + aLongitude);
        }

        iLongitude = aLongitude;
    }

    /**
     * Please refer JSR-179 spec
     */
    public static double convert(String aCoordinate)
    {
        if (aCoordinate == null)
        {
            throw new NullPointerException(COORDINATES_NULL_ERROR);
        }
        aCoordinate = aCoordinate.trim();
        if (aCoordinate.length() < 4)
        {
            throw new IllegalArgumentException(
                INVALID_COORDINATES_ERROR + aCoordinate);
        }
        char lastChar = aCoordinate.charAt(aCoordinate.length() - 1);
        if (lastChar < '0' || lastChar > '9')
        {
            throw new IllegalArgumentException(
                INVALID_COORDINATES_ERROR + aCoordinate);
        }
        int ddix = aCoordinate.indexOf(':');
        if (ddix < 1)
        {
            throw new IllegalArgumentException(
                INVALID_COORDINATES_ERROR + aCoordinate);
        }
        if (ddix > 1
                && aCoordinate.charAt(0) == '0'
                || (ddix > 2 && aCoordinate.charAt(0) == '-' && aCoordinate
                    .charAt(1) == '0'))
        {
            throw new IllegalArgumentException(
                INVALID_COORDINATES_ERROR + aCoordinate);
        }

        try
        {
            int dd = Integer.parseInt(aCoordinate.substring(0, ddix));
            if (dd >= 180 || dd < -180)
            {
                throw new IllegalArgumentException(
                    INVALID_COORDINATES_ERROR + aCoordinate);
            }

            int sign = 1;
            if (aCoordinate.startsWith("-"))
            {
                sign = -1;
                dd = -dd;
            }

            int inputType = DD_MM_SS;
            String mmstr = null;
            int mmix = aCoordinate.indexOf(':', ddix + 1);
            if (mmix < 1)
            {
                inputType = DD_MM;
                mmix = aCoordinate.length();
            }

            if (inputType == DD_MM)
            {
                mmstr = aCoordinate.substring(ddix + 1, aCoordinate.length());

                if (mmstr.length() < 2)
                {
                    throw new IllegalArgumentException(
                        INVALID_COORDINATES_ERROR + aCoordinate);
                }

                if (mmstr.length() > 2 && mmstr.charAt(2) != '.')
                {
                    throw new IllegalArgumentException(
                        INVALID_COORDINATES_ERROR + aCoordinate);
                }

                char minIntChar1 = mmstr.charAt(0);
                if (minIntChar1 < '0' || minIntChar1 > '9')
                {
                    throw new IllegalArgumentException(
                        INVALID_COORDINATES_ERROR + aCoordinate);
                }

                char minIntChar2 = mmstr.charAt(1);
                if (minIntChar2 < '0' || minIntChar2 > '9')
                {
                    throw new IllegalArgumentException(
                        INVALID_COORDINATES_ERROR + aCoordinate);
                }

                if (mmstr.length() > 8)
                {
                    throw new IllegalArgumentException(
                        INVALID_COORDINATES_ERROR + aCoordinate);
                }
                double mm = Double.parseDouble(mmstr);
                if (mm >= 60.0d || mm < 0)
                {
                    throw new IllegalArgumentException(
                        INVALID_COORDINATES_ERROR + aCoordinate);
                }

                double res = sign * (dd + mm / 60d);
                if (res >= 180.0d || res < -180.0d)
                {
                    throw new IllegalArgumentException(
                        INVALID_COORDINATES_ERROR + aCoordinate);
                }

                return res;
            }
            else if (inputType == DD_MM_SS)
            {

                if (mmix - (ddix + 1) < 2)
                {
                    throw new IllegalArgumentException(
                        INVALID_COORDINATES_ERROR + aCoordinate);
                }

                mmstr = aCoordinate.substring(ddix + 1, mmix);

                if (mmstr.length() > 2 && mmstr.charAt(2) != '.')
                {
                    throw new IllegalArgumentException(
                        INVALID_COORDINATES_ERROR + aCoordinate);
                }

                char minIntChar1 = mmstr.charAt(0);
                if (minIntChar1 < '0' || minIntChar1 > '9')
                {
                    throw new IllegalArgumentException(
                        INVALID_COORDINATES_ERROR + aCoordinate);
                }

                char minIntChar2 = mmstr.charAt(1);
                if (minIntChar2 < '0' || minIntChar2 > '9')
                {
                    throw new IllegalArgumentException(
                        INVALID_COORDINATES_ERROR + aCoordinate);
                }

                int mm = Integer.parseInt(mmstr);
                if (mm > 59 || mm < 0)
                {
                    throw new IllegalArgumentException(
                        INVALID_COORDINATES_ERROR + aCoordinate);
                }

                String ssstr = null;
                double ss = 0d;
                if (mmix != aCoordinate.length())
                {
                    ssstr = aCoordinate.substring(mmix + 1, aCoordinate
                                                  .length());
                    if (ssstr.length() < 2)
                    {
                        throw new IllegalArgumentException(
                            INVALID_COORDINATES_ERROR + aCoordinate);
                    }

                    if (ssstr.length() > 2 && ssstr.charAt(2) != '.')
                    {
                        throw new IllegalArgumentException(
                            INVALID_COORDINATES_ERROR + aCoordinate);
                    }

                    char secIntChar1 = ssstr.charAt(0);
                    if (secIntChar1 < '0' || secIntChar1 > '9')
                    {
                        throw new IllegalArgumentException(
                            INVALID_COORDINATES_ERROR + aCoordinate);
                    }

                    char secIntChar2 = ssstr.charAt(1);
                    if (secIntChar2 < '0' || secIntChar2 > '9')
                    {
                        throw new IllegalArgumentException(
                            INVALID_COORDINATES_ERROR + aCoordinate);
                    }

                    if (ssstr.length() > 6)
                    {
                        throw new IllegalArgumentException(
                            INVALID_COORDINATES_ERROR + aCoordinate);
                    }

                    ss = Double.parseDouble(ssstr);
                    if (ss >= 60.0d || ss < 0)
                    {
                        throw new IllegalArgumentException(
                            INVALID_COORDINATES_ERROR + aCoordinate);
                    }
                }
                double res = sign * (dd + mm / 60d + ss / 3600d);
                if (res >= 180.0d || res < -180.0d)
                {
                    throw new IllegalArgumentException(
                        INVALID_COORDINATES_ERROR + aCoordinate);
                }

                return res;
            }
            else
            {
                throw new IllegalArgumentException(
                    INVALID_COORDINATES_ERROR + aCoordinate);
            }
        }
        catch (NumberFormatException nfe)
        {
            throw new IllegalArgumentException(nfe.getMessage());
        }
    }

    /**
     * Please refer JSR-179 spec
     */
    public static String convert(double aCoordinate, int aOutputType)
    {
        if (aCoordinate >= 180.0d || aCoordinate < -180.0d)
        {
            throw new IllegalArgumentException(
                INVALID_COORDINATES_ERROR + aCoordinate);
        }
        if (Double.isNaN(aCoordinate))
        {
            throw new IllegalArgumentException(
                INVALID_COORDINATES_ERROR + aCoordinate);
        }

        boolean sign = aCoordinate < 0;
        aCoordinate = Math.abs(aCoordinate);

        if (aOutputType == DD_MM)
        {
            int rmm = (int)(Math.floor(100000d * 60d * aCoordinate + 0.5d));
            int dd = rmm / (60 * 100000);

            int mmi = (rmm / 100000) - (60 * dd);
            int mmd = rmm - 100000 * (60 * dd + mmi);
            String result = (sign ? "-" : "") + dd + ":"
                            + ((mmi < 10) ? "0" : "") + mmi + ".";

            if (mmd != 0)
            {
                if (mmd < 10)
                {
                    result += "0000";
                }
                else if (mmd < 100)
                {
                    result += "000";
                }
                else if (mmd < 1000)
                {
                    result += "00";
                }
                else if (mmd < 10000)
                {
                    result += "0";
                }
                while (mmd % 10 == 0)
                    mmd /= 10;
            }
            return result + mmd;
        }
        else if (aOutputType == DD_MM_SS)
        {
            int rss = (int)(Math.floor(1000d * 3600d * aCoordinate + 0.5d));
            int dd = rss / (3600 * 1000);
            int mm = (rss / (60 * 1000)) - 60 * dd;

            int ssi = (rss / 1000) - (3600 * dd + 60 * mm);
            int ssd = rss - 1000 * (3600 * dd + 60 * mm + ssi);
            String result = (sign ? "-" : "") + dd + ":"
                            + ((mm < 10) ? "0" : "") + mm + ":"
                            + ((ssi < 10) ? "0" : "") + ssi + ".";
            if (ssd != 0)
            {
                if (ssd < 10)
                {
                    result += "00";
                }
                else if (ssd < 100)
                {
                    result += "0";
                }
                while (ssd % 10 == 0)
                    ssd /= 10;
            }
            return result + ssd;
        }
        else
        {
            throw new IllegalArgumentException(
                INVALID_OUTPUT_TYPE_ERROR + aOutputType);
        }
    }

    /**
     * Please refer JSR-179 spec
     */
    public float azimuthTo(Coordinates aTo)
    {
        if (aTo == null)
        {
            throw new NullPointerException(COORDINATES_NULL_ERROR);
        }

        // This is in the north pole
        if (iLatitude == MAX_LATITUDE && aTo.iLatitude != MAX_LATITUDE)
        {
            return BEARING_HALF;
        }
        // This is in South Pole
        if (iLatitude == MIN_LATITUDE)
        {
            return BEARING_ZERO;
        }

        // Native is responsible for calculating the bearing
        return _azimuthTo(iLatitude, iLongitude, aTo.iLatitude, aTo.iLongitude);
    }

    /**
     * Please refer JSR-179 spec
     */
    public float distance(Coordinates aTo)
    {
        if (aTo == null)
        {
            throw new NullPointerException(COORDINATES_NULL_ERROR);
        }
        // Native is responsible for calculating the distance
        return _distance(iLatitude, iLongitude, aTo.iLatitude, aTo.iLongitude);
    }

    /**
     * Calculates a bearing between two positions
     *
     * @param aSrcLat
     *            The latitude of the source coordinates
     * @param aSrcLon
     *            The longitude of the source coordinates
     * @param aDstLat
     *            The latitude of the destination coordinates
     * @param aDstLon
     *            The longitude of the destination coordinates
     */
    private native float _azimuthTo(double aSrcLat, double aSrcLon,
                                    double aDstLat, double aDstLon);

    /**
     * Calculates a distance between two positions
     *
     * @param aSrcLat
     *            The latitude of the source coordinates
     * @param aSrcLon
     *            The longitude of the source coordinates
     * @param aDstLat
     *            The latitude of the destination coordinates
     * @param aDstLon
     *            The longitude of the destination coordinates
     */
    private native float _distance(double aSrcLat, double aSrcLon,
                                   double aDstLat, double aDstLon);
}

// End of file
