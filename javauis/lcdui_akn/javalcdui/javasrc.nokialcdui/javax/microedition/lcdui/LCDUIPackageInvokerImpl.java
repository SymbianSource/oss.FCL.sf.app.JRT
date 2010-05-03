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
* Description: Concrete class for accessing package private methods in LCDUI.
*
*/

// PACKAGE
package javax.microedition.lcdui;

// IMPORTS
import com.nokia.mid.ui.impl.LCDUIPackageInvoker;

/**
 * Concrete class for accessing package private methods in LCDUI.
 */
class LCDUIPackageInvokerImpl extends LCDUIPackageInvoker
{
    /**
     * Static initializer for this class.
     */
    static
    {
        setInvoker(new LCDUIPackageInvokerImpl());
    }

    /**
     * Gets the fullscreen status of a Canvas.
     *
     * @param aCanvas The canvas from which the status is requested.
     * @return The fullscreen status of the specified canvas.
     */
    public boolean isFullScreen(Canvas aCanvas)
    {
        return aCanvas.getFullScreenMode();
    }

    /**
     * Checks that the given arguments are valid.
     *
     * @param aText The text to be verified.
     * @param aMaxSize The maximum size to be verified.
     */
    public void checkText(String aText, int aMaxSize)
    {
        TextComponent.checkText(aText, aMaxSize);
    }

    /**
     * Checks that the given constraints are valid.
     *
     * @param aConstraints The constraints to be verified.
     */
    public void checkConstraints(int aConstraints)
    {
        TextComponent.checkConstraints(aConstraints);
    }

    /**
     * Checks that the given offset parameters are valid.
     *
     * @param aOffset The offset to be verified.
     * @param aLength The lenght to be verified.
     * @param aSize The size to be verified.
     */
    public void checkOffset(int aOffset, int aLength, int aSize)
    {
        TextComponent.checkOffset(aOffset, aLength, aSize);
    }

    /**
     * Gets the Font handle.
     *
     * @param aFont The font which handle is requested.
     * @return handle.
     */
    public int getFontHandle(Font aFont)
    {
        return aFont.iHandle;
    }

}

// End of file
