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

public class M2GSVGLocatableElement
        extends M2GSVGElement
        implements SVGLocatableElement
{
    //------------------------------------------------------------------
    // METHODS
    //------------------------------------------------------------------

    /**
     * Constructor
     * @param aHandle -
     * @param aDocument -
     */
    public M2GSVGLocatableElement(int aHandle, M2GDocument aDocument)
    {
        super(aHandle, aDocument);
    }

    /**
     * @see org.w3c.dom.svg.SVGLocatableElement#getBBox()
     */
    public SVGRect getBBox()
    {
        M2GSVGRect bbox = new M2GSVGRect();
        M2GSVGElement._getBBox(getNativeSVGProxyHandle(), getHandle(),
                               M2GSVGConstants.AT_BBOX, bbox.getComponents());
        // Checks element's type
        if ((iElementTypeId == M2GSVGConstants.EL_G) ||
                (iElementTypeId == M2GSVGConstants.EL_PATH) ||
                (iElementTypeId == M2GSVGConstants.EL_SVG))
        {
            if (bbox.isZero())
            {
                bbox = null;
            }
        }
        else if (iElementTypeId == M2GSVGConstants.EL_TEXT)
        {
            if ((bbox.getHeight()) == 0 && (bbox.getWidth() == 0))
            {
                bbox = null;
            }
        }
        return bbox;
    }

    /**
     * @see org.w3c.dom.svg.SVGLocatableElement#getScreenBBox()
     */
    public SVGRect getScreenBBox()
    {
        // Checks if this element is in DOM
        if (getDocument().isElementInDOM(getHandle()))
        {
            M2GSVGRect bbox = new M2GSVGRect();
            // Initializes M2GSVGRect components
            M2GSVGElement._getScreenBBox(
                getNativeSVGProxyHandle(),
                getHandle(),
                bbox.getComponents());
            return bbox;
        }
        return null;
    }

    /**
     * @see org.w3c.dom.svg.SVGLocatableElement#getScreenCTM()
     */
    public SVGMatrix getScreenCTM()
    {
        if (getDocument().isElementInDOM(getHandle()))
        {
            M2GSVGMatrix matrix = new M2GSVGMatrix();
            _getMatrixTrait(getNativeSVGProxyHandle(),
                            getHandle(),
                            M2GSVGConstants.EL_SVG, matrix.getComponents());
            return matrix;
        }
        return null;
    }
}
