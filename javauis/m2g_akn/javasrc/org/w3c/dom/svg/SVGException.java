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
 * An exception thrown for SVG-specific errors, such as noninvertable matrix in {@link org.w3c.dom.svg.SVGMatrix#inverse inverse}.
 */
public class SVGException extends RuntimeException
{
    /**
     * An integer indicating the type of error generated.
     */
    public short code;

    /**
     * Constructs a SVGException with a detailed message.
     *
     * @param code the exception's error code.
     * @param message the exception's descriptive message.
     */
    public SVGException(short code, String message)
    {
        super(message);
        this.code = code;
    }


    /**
     * Value passed to an SVG-specific method is invalid, such as out of range color component in {@link org.w3c.dom.svg.SVGSVGElement#createSVGRGBColor createSVGRGBColor}.
     */
    public static final short SVG_INVALID_VALUE_ERR = 1;

    /**
     * Matrix that has a determinant equal to zero, and therefore not invertable.
     */
    public static final short SVG_MATRIX_NOT_INVERTABLE = 2;

}
