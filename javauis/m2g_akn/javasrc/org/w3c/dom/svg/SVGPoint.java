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
 * This interface represents an "SVGPoint" datatype, identifiend by its x and y components.
 */
public interface SVGPoint
{

    /**
     * Sets the x component of the point to the specified float value.
     *
     * @param value the x component value
     *
     */

    public void setX(float value);

    /**
     * Sets the y component of the point to the specified float value.
     *
     * @param value the y component value
     *
     */

    public void setY(float value);


    /**
     * Returns the x component of the point.
     *
     * @return the x component of the point.
     *
     */

    public float getX();

    /**
     * Returns the y component of the point.
     *
     * @return the y component of the point.
     *
     */
    public float getY();
}
