/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
package org.w3c.dom.svg;


/**
 * This interface represents an "SVGRect" datatype, consisting of a minimum X, minimum Y, width
 * and height values.
 */
public interface SVGRect
{

    /**
     * Sets the minimum X value of this SVGRect to the specified value.
     *
     * @param value the minimum X value.
     *
     */
    public void setX(float value);

    /**
     * Sets the minimum Y value of this SVGRect to the specified value.
     *
     * @param value the minimum Y value.
     *
     */
    public void setY(float value);

    /**
     * Sets the width of this SVGRect to the specified value.
     *
     * @param value the rectangle width value.
     *
     */
    public void setWidth(float value);

    /**
     * Sets the height of this SVGRect to the specified value.
     *
     * @param value the rectangle height value.
     *
     */
    public void setHeight(float value);

    /**
     * Returns the minimum X value for this SVGRect.
     *
     * @return the minimum X value.
     *
     */
    public float getX();

    /**
     * Returns the minimum Y value for this SVGRect.
     *
     * @return the minimum Y value.
     *
     */
    public float getY();


    /**
     * Returns the width for this SVGRect.
     *
     * @return the rectangle width.
     *
     */
    public float getWidth();

    /**
     * Returns the height for this SVGRect.
     *
     * @return the rectangle height.
     *
     */
    public float getHeight();

}
