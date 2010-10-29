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

/**
 * This interface represents an "SVGRect" datatype, consisting of a minimum X, minimum Y, width
 * and height values.
 */
public class M2GSVGRect implements SVGRect
{
    //--------------------------------------------------
    // STATIC CONSTANTS
    //--------------------------------------------------
    private static final int X = 0;
    private static final int Y = 1;
    private static final int W = 2;
    private static final int H = 3;

    //--------------------------------------------------
    // VARIABLES
    //--------------------------------------------------
    private float[] iComponents;

    //--------------------------------------------------
    // METHODS
    //--------------------------------------------------
    /**
     * Contructor
     */
    protected M2GSVGRect()
    {
        iComponents = new float[4];
        iComponents[X] = 0;
        iComponents[Y] = 0;
        iComponents[W] = 0;
        iComponents[H] = 0;
    }

    /**
     * Contructor
     * @param aX -
     * @param aY -
     * @param aWidth -
     * @param aHeight -
     */
    protected M2GSVGRect(float aX, float aY, float aWidth, float aHeight)
    {
        iComponents = new float[4];
        iComponents[X] = aX;
        iComponents[Y] = aY;
        iComponents[W] = aWidth;
        iComponents[H] = aHeight;
    }

    /**
     * Returns reference to a components
     * @return components
     */
    float[] getComponents()
    {
        return iComponents;
    }

    /**
     * @see org.w3c.dom.svg.SVGRect#getX()
     */
    public float getX()
    {
        return iComponents[X];
    }

    /**
     * @see org.w3c.dom.svg.SVGRect#getY()
     */
    public float getY()
    {
        return iComponents[Y];
    }

    /**
     * @see org.w3c.dom.svg.SVGRect#getWidth()
     */
    public float getWidth()
    {
        return iComponents[W];
    }

    /**
     * @see org.w3c.dom.svg.SVGRect#getHeight()
     */
    public float getHeight()
    {
        return iComponents[H];
    }

    /**
     * Checks if all components are zero
     * @returns true is every component is zero
     */
    public boolean isZero()
    {
        for (int index = 0; index < iComponents.length; index++)
        {
            if (iComponents[index] != 0)
            {
                return false;
            }
        }
        return true;
    }

    /**
     * @see org.w3c.dom.svg.SVGRect#setX()
     */
    public void setX(float value)
    {
        iComponents[X] = value;
    }

    /**
     * @see org.w3c.dom.svg.SVGRect#setY()
     */
    public void setY(float value)
    {
        iComponents[Y] = value;
    }

    /**
     * @see org.w3c.dom.svg.SVGRect#setWidth()
     */
    public void setWidth(float value)
    {
        iComponents[W] = value;
    }

    /**
     * @see org.w3c.dom.svg.SVGRect#setHeight()
     */
    public void setHeight(float value)
    {
        iComponents[H] = value;
    }
}
