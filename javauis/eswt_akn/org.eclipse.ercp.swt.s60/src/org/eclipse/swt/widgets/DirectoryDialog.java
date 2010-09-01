/*******************************************************************************
 * Copyright (c) 2000, 2004 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *     Lynne Kues (IBM Corp) - modified to reflect eSWT API subset
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/
package org.eclipse.swt.widgets;


import org.eclipse.swt.*;
import org.eclipse.ercp.swt.expanded.internal.OS;


/**
 * Instances of this class allow the user to navigate
 * the file system and select a directory.
 * <dl>
 * <dt><b>Styles:</b></dt>
 * <dd>(none)</dd>
 * <dt><b>Events:</b></dt>
 * <dd>(none)</dd>
 * </dl>
 * <p>
 * IMPORTANT: This class is intended to be subclassed <em>only</em>
 * within the SWT implementation.
 * </p>
 */

public class DirectoryDialog extends Dialog
{
    String iMessage = "";
    String iPath = "";

    /**
     * Constructs a new instance of this class given only its parent.
     *
     * @param parent a shell which will be the parent of the new instance
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the parent is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the parent</li>
     *    <li>ERROR_INVALID_SUBCLASS - if this class is not an allowed subclass</li>
     * </ul>
     */
    public DirectoryDialog(Shell parent)
    {
        super(parent);
    }

    /**
     * Constructs a new instance of this class given its parent
     * and a style value describing its behavior and appearance.
     * <p>
     * The style value is either one of the style constants defined in
     * class <code>SWT</code> which is applicable to instances of this
     * class, or must be built by <em>bitwise OR</em>'ing together
     * (that is, using the <code>int</code> "|" operator) two or more
     * of those <code>SWT</code> style constants. The class description
     * lists the style constants that are applicable to the class.
     * Style bits are also inherited from superclasses.
     * </p>
     *
     * @param parent a shell which will be the parent of the new instance
     * @param style the style of dialog to construct
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the parent is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the parent</li>
     *    <li>ERROR_INVALID_SUBCLASS - if this class is not an allowed subclass</li>
     * </ul>
     */
    public DirectoryDialog(Shell parent, int style)
    {
        super(parent, style);
    }

    /**
     * Returns the path which the dialog will use to filter
     * the directories it shows.
     *
     * @return the filter path
     *
     * @see #setFilterPath
     */
    public String getFilterPath()
    {
        return iPath;
    }

    /**
     * Returns the dialog's message, which is a description of
     * the purpose for which it was opened. This message will be
     * visible on the dialog while it is open.
     *
     * @return the message
     */
    public String getMessage()
    {
        return iMessage;
    }

    /**
     * Makes the dialog visible and brings it to the front
     * of the display.
     *
     * @return a string describing the absolute path of the selected directory,
     *         or null if the dialog was cancelled or an error occurred
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the dialog has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the dialog</li>
     * </ul>
     */
    public String open()
    {
        if (!parent.getDisplay().isValidThread()) error(SWT.ERROR_THREAD_INVALID_ACCESS);
        boolean useSlash = false;
        if (iPath != null)
        {
            if (iPath.length() > 0)
            {
                useSlash = (iPath.indexOf('/') >= 0);
                iPath = iPath.replace('/', '\\');
            }
            if (!(iPath.endsWith("\\")))
            {
                iPath += "\\";
            }
        }
        OS.DirectoryDialog_Open(
            this,
            iPath,
            title == "" ? null : title);

        // Looping until native dialog broker sets the flag (iIsOpen) to false
        iIsOpen = true;
        internal_waitForClose();

        // Get dialog result
        iPath = iResultString;
        iResultString = null;

        if (iPath.length() == 0)
        {
            iPath = null;
        }

        if (iPath != null && iPath.length() > 0)
        {
            if (useSlash)
            {
                iPath = iPath.replace('\\', '/');
            }
        }
        return iPath;
    }

    /**
     * Sets the path that the dialog will use to filter
     * the directories it shows to the argument, which may
     * be null. If the string is null, then the operating
     * system's default filter path will be used.
     * <p>
     * Note that the path string is platform dependent.
     * For convenience, either '/' or '\' can be used
     * as a path separator.
     * </p>
     *
     * @param string the filter path
     */
    public void setFilterPath(String filterPath)
    {
        this.iPath = filterPath;
    }

    /**
     * Sets the dialog's message, which is a description of
     * the purpose for which it was opened. This message will be
     * visible on the dialog while it is open.
     *
     * @param string the message
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the string is null</li>
     * </ul>
     */
    public void setMessage(String message)
    {
        if (message == null)
        {
            error(SWT.ERROR_NULL_ARGUMENT);
        }
        this.iMessage = message;
    }

}
