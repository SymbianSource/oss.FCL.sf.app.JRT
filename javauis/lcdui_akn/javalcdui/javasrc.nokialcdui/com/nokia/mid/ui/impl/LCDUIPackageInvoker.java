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
 * Description: Abstract class for accessing package private methods in LCDUi.
 *
 */

// PACKAGE
package com.nokia.mid.ui.impl;

// IMPORTS
import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Font;

/**
 * Abstract class for accessing package private methods in LCDUi.
 */
public abstract class LCDUIPackageInvoker
{
    // Singleton instance of the invoker.
    private static LCDUIPackageInvoker sInstance;

    // Invoker implementation package and class.
    private static final String INVOKER_IMPLEMENTATION =
        "javax.microedition.lcdui.LCDUIPackageInvokerImpl";

    /**
     * Returns the singleton instance of the text component invoker.
     *
     * @param TextComponent
     *            invoker.
     */
    public static synchronized LCDUIPackageInvoker getInvoker()
    {
        if (sInstance == null)
        {
            try
            {
                // Static initializer is executed when the class is loaded.
                Class.forName(INVOKER_IMPLEMENTATION);
            }
            catch (ClassNotFoundException e)
            {
                throw new RuntimeException();
            }
        }

        return sInstance;
    }

    /**
     * Setter for the invoker. Must be called by the base class.
     *
     * @param aInvoker
     *            The invoker implementation instance.
     */
    protected static synchronized void setInvoker(LCDUIPackageInvoker aInvoker)
    {
        if (sInstance != null)
        {
            throw new RuntimeException();
        }

        sInstance = aInvoker;
    }

    // Abstract methods.

    /**
     * Gets the fullscreen status of a Canvas.
     *
     * @param aCanvas
     *            The canvas from which the status is requested.
     * @return The fullscreen status of the specified canvas.
     */
    public abstract boolean isFullScreen(Canvas aCanvas);

    /**
     * Checks that the given arguments are valid.
     *
     * @param aText
     *            The text to be verified.
     * @param aMaxSize
     *            The maximum size to be verified.
     */
    public abstract void checkText(String aText, int aMaxSize);

    /**
     * Checks that the given constraints are valid.
     *
     * @param aConstraints
     *            The constraints to be verified.
     */
    public abstract void checkConstraints(int aConstraints);

    /**
     * Checks that the given offset parameters are valid.
     *
     * @param aOffset
     *            The offset to be verified.
     * @param aLength
     *            The lenght to be verified.
     * @param aSize
     *            The size to be verified.
     */
    public abstract void checkOffset(int aOffset, int aLength, int aSize);

    /**
     * Gets the Font handle.
     *
     * @param aFont
     *            The font which handle is requested.
     * @return handle.
     */
    public abstract int getFontHandle(Font aFont);

}

// End of file
