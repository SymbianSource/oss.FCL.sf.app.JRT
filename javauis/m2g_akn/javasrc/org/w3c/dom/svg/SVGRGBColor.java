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
 * This interface represents an "SVGRGBColor" datatype made up of red, green, and blue components.
 * It can be used to read properties that store color values ({@link org.w3c.dom.svg.SVGElement#getRGBColorTrait getRGBColorTrait})
 * such as <code>fill</code>, <code>stroke</code>, and <code>color</code>.
 */
public interface SVGRGBColor
{

    /**
     * Returns the red component of the SVGRGBColor.
     *
     * @return the red component.
     *
     */
    public int getRed();

    /**
     * Returns the green component of the SVGRGBColor.
     *
     * @return the green component.
     *
     */
    public int getGreen();

    /**
     * Returns the blue component of the SVGRGBColor.
     *
     * @return the blue component.
     *
     */
    public int getBlue();

}
