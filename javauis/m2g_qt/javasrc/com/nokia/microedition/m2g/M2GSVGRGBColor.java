/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.microedition.m2g;

import org.w3c.dom.svg.*;

public class M2GSVGRGBColor implements SVGRGBColor
{
    //--------------------------------------------------
    // STATIC CONSTANTS
    //--------------------------------------------------
    private static final int R = 0;
    private static final int G = 1;
    private static final int B = 2;
    // Exception text
    private static final String OUT_OF_RANGE_ESTR =
        "Color values out of range.";

    //--------------------------------------------------
    // VARIABLES
    //--------------------------------------------------
    private int[] iComponents;

    //--------------------------------------------------
    // METHODS
    //--------------------------------------------------

    /**
     * Constructor. Defualt color is black.
    v    */
    public M2GSVGRGBColor()
    {
        iComponents = new int[3];
        iComponents[R] = 0;
        iComponents[G] = 0;
        iComponents[B] = 0;
    }

    /**
     * Constructor
     * @param aR -
    v    * @param aG -
    v    * @param aB -
    v    */
    public M2GSVGRGBColor(int aR, int aG, int aB) throws SVGException
    {
        checkValues(aR, aG, aB);
        iComponents = new int[3];
        iComponents[R] = aR;
        iComponents[G] = aG;
        iComponents[B] = aB;
    }

    /**
     * Checks value range
     * @param aR
     * @param aG
     * @param aB
     */
    protected void checkValues(int aR, int aG, int aB) throws SVGException
    {
        if (((aR > 255) || (aR < 0)) ||
                ((aG > 255) || (aG < 0)) ||
                ((aB > 255) || (aB < 0)))
        {
            throw new SVGException(
                SVGException.SVG_INVALID_VALUE_ERR, OUT_OF_RANGE_ESTR);
        }
    }

    /**
     * Gets components
     * @return components
     */
    int[] getComponents()
    {
        return iComponents;
    }

    /**
     * @see org.w3c.dom.svg.SVGRGBColor#getRed()
     */
    public int getRed()
    {
        return iComponents[R];
    }

    /**
     * @see org.w3c.dom.svg.SVGRGBColor#getGreen()
     */
    public int getGreen()
    {
        return iComponents[G];
    }

    /**
     * @see org.w3c.dom.svg.SVGRGBColor#getBlue()
     */
    public int getBlue()
    {
        return iComponents[B];
    }

    public String toString()
    {
        return "SVGRGBColor( r = " + iComponents[R] + ", g = " +
               iComponents[G] + ", b = " + iComponents[B] + " )";
    }
}
