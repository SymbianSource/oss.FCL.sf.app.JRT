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

// PACKAGE
package com.nokia.mid.ui;

// EXTERNAL IMPORTS
import com.nokia.mj.impl.rt.legacy.NativeError;
import com.nokia.mj.impl.rt.legacy.ToolkitInvoker;

/**
 * Class to provide extra functionality that is related to text-editing
 *
 * @since 1.4
 */
public abstract class Clipboard
{

    // Private constructor.
    private Clipboard()
    {
    }

    /**
     * Copies characters into the system clipboard.
     * <P>
     * If <code>text</code> parameter is <code>null</code> or empty string,
     * it clears the system clipboard.
     *
     * @param text the text to be copied
     */
    public static void copyToClipboard(String text)
    {
        // Toolkit invoker is needed for accessing javax.microedition.lcdui
        // package
        ToolkitInvoker toolkitInvoker = ToolkitInvoker.getToolkitInvoker();

        Object toolkit = toolkitInvoker.getToolkit();

        synchronized (toolkit)
        {
            if ((text != null) && (text.length() == 0))
            {
                text = null;
            }
            NativeError.check(_copyToClipboard(
                                  toolkitInvoker.toolkitGetHandle(toolkit), text));
        }
    }

    /**
     * Copies characters from the system clipboard.
     * <P>
     * Returns empty string when there is nothing in the system clipboard.
     * <P>
     * This method is not supported on S40 platform, returns <code>null</code>.
     * <P>
     * @return the content in clipboard
     */
    public static String copyFromClipboard()
    {
        String text = null;

        int[] error = new int[1];
        // Toolkit invoker is needed for accessing javax.microedition.lcdui
        // package
        ToolkitInvoker toolkitInvoker = ToolkitInvoker.getToolkitInvoker();

        Object toolkit = toolkitInvoker.getToolkit();

        synchronized (toolkit)
        {
            text = _copyFromClipboard(
                       toolkitInvoker.toolkitGetHandle(toolkit),
                       error);
        }

        NativeError.check(error[0]);
        if (text == null)
        {
            text = "";
        }
        return text;
    }

    /*
     * Stores the text into the clipboard.
     *
     * @param toolkitHandle A handle to the LCDUI toolkit.
     *
     * @param text A text to be copied into the system-clipboard.
     *
     * @return NativeError.KErrNone if the operation was successful. Otherwise,
     * a system-wide error code is returned.
     */
    private static native int _copyToClipboard(int toolkit, String text);

    /*
     * Copies characters from the system-clipboard.
     *
     * @param toolkit A handle to the LCDUI toolkit.
     *
     * @param error On return contains the error code for the operation.
     *
     * @return The content of clipboard.
     */
    private static native String _copyFromClipboard(
        int toolkit,
        int[] error);

}

// END OF FILE