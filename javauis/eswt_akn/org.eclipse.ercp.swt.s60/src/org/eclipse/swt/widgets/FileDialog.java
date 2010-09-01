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
import org.eclipse.swt.internal.symbian.*;


/**
 * Instances of this class allow the user to navigate
 * the file system and select or enter a file name.
 * <dl>
 * <dt><b>Styles:</b></dt>
 * <dd>SAVE, OPEN, MULTI</dd>
 * <dt><b>Events:</b></dt>
 * <dd>(none)</dd>
 * </dl>
 * <p>
 * IMPORTANT: This class is intended to be subclassed <em>only</em>
 * within the SWT implementation.
 * </p>
 */
public class FileDialog extends Dialog
{
    private String iFileName = "";
    private String iPath = "";

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
    public FileDialog(Shell parent)
    {
        super(parent);
        checkSubclass();
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
    public FileDialog(Shell parent, int style)
    {
        super(parent, style);
        checkSubclass();
    }

    /**
     * Returns the path of the first file that was
     * selected in the dialog relative to the filter path, or an
     * empty string if no such file has been selected.
     *
     * @return the relative path of the file
     */
    public String getFileName()
    {
        return iFileName;
    }

    /**
     * Returns a (possibly empty) array with the paths of all files
     * that were selected in the dialog relative to the filter path.
     *
     * @return the relative paths of the files
     */
    public String [] getFileNames()
    {
        // Not supported in Symbian platform.
        return new String[0];
    }

    /**
     * Returns the file extensions which the dialog will
     * use to filter the files it shows.
     *
     * @return the file extensions filter
     */
    public String [] getFilterExtensions()
    {
        // Not supported in Symbian platform.
        return new String[0];
    }

    /**
     * Returns the names that describe the filter extensions
     * which the dialog will use to filter the files it shows.
     *
     * @return the list of filter names
     */
    public String [] getFilterNames()
    {
        // Not supported in Symbian platform.
        return new String[0];
    }

    /**
     * Returns the directory path that the dialog will use, or an empty
     * string if this is not set.  File names in this path will appear
     * in the dialog, filtered according to the filter extensions.
     *
     * @return the directory path string
     *
     * @see #setFilterExtensions
     */
    public String getFilterPath()
    {
        return iPath;
    }

    /**
     * Makes the dialog visible and brings it to the front
     * of the display.
     *
     * @return a string describing the absolute path of the first selected file,
     *         or null if the dialog was cancelled or an error occurred
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the dialog has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the dialog</li>
     * </ul>
     */
    public String open()
    {
        internal_checkThread();
        String resultPath = "";
        boolean useSlash = false;
        if (iPath != null)
        {
            resultPath += iPath;
            if (resultPath.length() > 0)
            {
                useSlash = (resultPath.indexOf('/') >= 0);
                resultPath = resultPath.replace('/', '\\');
            }
            if (!(resultPath.endsWith("\\")))
            {
                resultPath += "\\";
            }
        }
        if (iFileName != null)
        {
            resultPath += iFileName;
        }

        OS.FileDialog_Open(this, resultPath, getText(), style);

        iIsOpen = true;
        //looping until native dialog broker sets the flag (iIsOpen) to false
        internal_waitForClose();
        // get dialog result
        resultPath = iResultString;
        iResultString = null;

        iFileName = "";
        iPath = "";
        if (resultPath != null && resultPath.length() > 0)
        {
            int separatorIndex = resultPath.lastIndexOf('\\');
            iFileName = resultPath.substring(separatorIndex + 1);
            iPath = resultPath.substring(0, separatorIndex + 1);
            if (useSlash)
            {
                iPath = iPath.replace('\\', '/');
                resultPath = resultPath.replace('\\', '/');
            }
            return resultPath;
        }
        else
        {
            return null;
        }
    }

    /**
     * Set the initial filename which the dialog will
     * select by default when opened to the argument,
     * which may be null.  The name will be prefixed with
     * the filter path when one is supplied.
     *
     * @param string the file name
     */
    public void setFileName(String string)
    {
        iFileName = string;
    }

    /**
     * Set the file extensions which the dialog will
     * use to filter the files it shows to the argument,
     * which may be null.
     * <p>
     * The strings are platform specific. For example, on
     * Windows, an extension filter string is typically of
     * the form "*.extension", where "*.*" matches all files.
     * </p>
     *
     * @param extensions the file extension filter
     */
    public void setFilterExtensions(String [] extensions)
    {
        // Not supported in Symbian platform.
    }

    /**
     * Sets the the names that describe the filter extensions
     * which the dialog will use to filter the files it shows
     * to the argument, which may be null.
     *
     * @param names the list of filter names
     */
    public void setFilterNames(String [] names)
    {
        // Not supported in Symbian platform.
    }

    /**
     * Sets the directory path that the dialog will use
     * to the argument, which may be null. File names in this
     * path will appear in the dialog, filtered according
     * to the filter extensions. If the string is null,
     * then the operating system's default filter path
     * will be used.
     * <p>
     * Note that the path string is platform dependent.
     * For convenience, either '/' or '\' can be used
     * as a path separator.
     * </p>
     *
     * @param string the directory path
     *
     * @see #setFilterExtensions
     */
    public void setFilterPath(String string)
    {
        iPath = string;
    }


    protected void internal_checkThread()
    {
        if (!parent.display.internal_isValidThread())
        {
            SWT.error(SWT.ERROR_THREAD_INVALID_ACCESS);
        }
    }

}
