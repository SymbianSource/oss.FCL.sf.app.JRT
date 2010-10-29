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
 * This interface represents an "SVGPoint" datatype,
 * identified by its x and y components.
 */
public class M2GSVGPoint implements SVGPoint
{
    //--------------------------------------------------
    // VARIABLES
    //--------------------------------------------------
    private M2GSVGSVGElement iElement;
    private float iX;
    private float iY;

    //--------------------------------------------------
    // METHODS
    //--------------------------------------------------
    /**
    * Constructor
    * @param aRootElement
     */
    protected M2GSVGPoint(M2GSVGSVGElement aElement)
    {
        iElement = aElement;
        iX = 0;
        iY = 0;
    }

    /**
     * Check set validity
     * @param aValue Value
     * @return true if setting can be done
     */
    protected boolean check(float aValue)
    {
        if (Float.isNaN(aValue))
        {
            // throw IllegalArgumentException();
            return false;
        }
        if (iElement != null)
        {
            String zoomAndPan = iElement.getTrait("zoomAndPan");
            if ((zoomAndPan != null) && zoomAndPan.equals("disable"))
            {
                return false;
            }
            return true;
        }
        return false;
    }

    /**
     * @see org.w3c.dom.svg.SVGPath#getX()
     */
    public float getX()
    {
        synchronized (this)
        {
            return iX;
        }
    }

    /**
     * @see org.w3c.dom.svg.SVGPath#getX()
     */
    public float getY()
    {
        synchronized (this)
        {
            return iY;
        }
    }

    /**
     * @see org.w3c.dom.svg.SVGPath#setX()
     */
    public void setX(float value)
    {
        synchronized (this)
        {
            if (check(value))
            {
                iX = value;
                if (iElement != null)
                {
                    iElement.updateTransformMatrix();
                }
            }
        }
    }

    /**
     * @see org.w3c.dom.svg.SVGPath#setY()
     */
    public void setY(float value)
    {
        synchronized (this)
        {
            if (check(value))
            {
                iY = value;
                if (iElement != null)
                {
                    iElement.updateTransformMatrix();
                }
            }
        }
    }
}
