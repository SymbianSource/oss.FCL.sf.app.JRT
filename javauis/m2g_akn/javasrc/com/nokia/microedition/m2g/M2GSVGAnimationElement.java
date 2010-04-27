/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
import com.nokia.mj.impl.utils.Logger;


/**
 * This interface represents an Animation element,
 * which contains methods to control the timing of
 * animations.
 */
public class M2GSVGAnimationElement
        extends M2GSVGElement
        implements SVGAnimationElement
{
    //--------------------------------------------------
    // METHODS
    //--------------------------------------------------
    /**
     * Constructor
     * @param aElemnetHandle -
     * @param aDocument -
     */
    public M2GSVGAnimationElement(int aElementHandle, M2GDocument aDocument)
    {
        super(aElementHandle, aDocument);
    }

    /**
     * @see org.w3c.dom.svg.SVGAnimationElement#beginElementAt()
     */
    public void beginElementAt(float offset)
    {
        _beginElementAt(
            getNativeEventSourceHandle(), getNativeSVGProxyHandle(),
            getDocument().getHandle(), getHandle(), offset,
            getDocument().getRootElement().getCurrentTime(),
            iUIToolkit);
    }

    /**
     * Same as the beginElementAt with offset 0.0f
     * @see org.w3c.dom.svg.SVGAnimationElement#beginElementAt()
     */
    public void beginElement()
    {
        beginElementAt(0.0f);
    }

    /**
     * @see org.w3c.dom.svg.SVGAnimationElement#endElementAt()
     */
    public void endElementAt(float offset)
    {
        short endAttribute = _getEnumTrait(
                                 getNativeEventSourceHandle(), getNativeSVGProxyHandle(),
                                 getHandle(), M2GSVGConstants.AT_END, iUIToolkit);
        // NOTE Native SVG engine checks that element is active so
        // java side doesn't have to do that
        if (endAttribute == M2GSVGConstants.ANIM_INDEFINITE)
        {
            Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                       "endElementAt() - Cannot stop this animation, indefinite attribute");
        }
        else
        {
            _endElementAt(
                getNativeEventSourceHandle(), getNativeSVGProxyHandle(),
                getDocument().getHandle(), getHandle(),
                (offset + getDocument().getRootElement().getCurrentTime()),
                iUIToolkit);
        }
    }


    //--------------------------------------------------
    // NATIVE METHODS
    //--------------------------------------------------
    private native void _beginElementAt(
        int aEventSourceHandle, int aSvgProxyHandle, int aDocumentHandle,
        int aElementHandle, float aOffset, float aCurrentTime,boolean iUIToolkit);

    private native void _endElementAt(
        int aEventSourceHandle, int aSvgProxyHandle, int aDocumentHandle,
        int aElementHandle, float aOffset,boolean iUIToolkit);

    private native boolean _isActive(
        int aEventSourceHandle, int aSvgProxyHandle, int aElementHandle, boolean iUIToolkit);
}
