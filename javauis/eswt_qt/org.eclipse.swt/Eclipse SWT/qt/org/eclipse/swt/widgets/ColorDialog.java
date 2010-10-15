/*******************************************************************************
 * Copyright (c) 2000, 2007 IBM Corporation and others.
 * Portion Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *     Nokia Corporation - Qt implementation
 *******************************************************************************/
package org.eclipse.swt.widgets;


import org.eclipse.swt.*;
import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.graphics.*;

/**
 * Instances of this class allow the user to select a color
 * from a predefined set of available colors.
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
public class ColorDialog extends Dialog {
    RGB rgb;
    String dialogID;
    static int dialogCount;
/**
 * Constructs a new instance of this class given only its parent.
 *
 * @param parent a composite control which will be the parent of the new instance
 *
 * @exception IllegalArgumentException <ul>
 *    <li>ERROR_NULL_ARGUMENT - if the parent is null</li>
 * </ul>
 * @exception SWTException <ul>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the parent</li>
 *    <li>ERROR_INVALID_SUBCLASS - if this class is not an allowed subclass</li>
 * </ul>
 *
 * @see SWT
 * @see Widget#checkSubclass
 * @see Widget#getStyle
 */
public ColorDialog (Shell parent) {
    this (parent, SWT.NONE);
    dialogCount++;
    dialogID = toString() + String.valueOf(dialogCount);
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
 * @param parent a composite control which will be the parent of the new instance (cannot be null)
 * @param style the style of control to construct
 *
 * @exception IllegalArgumentException <ul>
 *    <li>ERROR_NULL_ARGUMENT - if the parent is null</li>
 * </ul>
 * @exception SWTException <ul>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the parent</li>
 *    <li>ERROR_INVALID_SUBCLASS - if this class is not an allowed subclass</li>
 * </ul>
 *
 * @see SWT
 * @see Widget#checkSubclass
 * @see Widget#getStyle
 */
public ColorDialog (Shell parent, int style) {
    super (parent, parent == null? style : checkStyle (parent, style));
    checkSubclass ();
    dialogCount++;
    dialogID = toString() + String.valueOf(dialogCount);
}

/**
 * Returns the currently selected color in the receiver.
 *
 * @return the RGB value for the selected color, may be null
 *
 * @see PaletteData#getRGBs
 */
public RGB getRGB () {
    return rgb;
}

/**
 * Makes the receiver visible and brings it to the front
 * of the display.
 *
 * @return the selected color, or null if the dialog was
 *         cancelled, no color was selected, or an error
 *         occurred
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public RGB open() {
    int r = 0;
    int g = 0;
    int b = 0;
    if (rgb != null) {
        r = rgb.red;
        g = rgb.green;
        b = rgb.blue;
    }

    DisposeListener listener = new DisposeListener() {
        public void widgetDisposed(DisposeEvent e) {
            if (e.widget == parent) {
                // Close dialogs which are opened on top of parent.
                OS.QDialog_swt_closeDialogs(parent.handle, dialogID);
            }
        }
    };
    parent.addDisposeListener(listener);

    int newColorHandle = 0;
    try {
        // Open the dialog
        newColorHandle = OS.QColorDialog_getColor(r, g, b, parent.handle, dialogID, 
            (style & SWT.RIGHT_TO_LEFT) != 0 ? OS.QT_RIGHTTOLEFT : OS.QT_LEFTTORIGHT);
    }
    finally {
        if (parent != null && !parent.isDisposed()) {
            parent.removeDisposeListener(listener);
        }
    }
    
    if (newColorHandle == 0) {
        return null;
    }
    
    try {
        if (OS.QColor_isValid(newColorHandle)) {
            int red = OS.QColor_red(newColorHandle);
            int blue = OS.QColor_blue(newColorHandle);
            int green = OS.QColor_green(newColorHandle);
            rgb = new RGB(red, green, blue);
        }
    }
    finally {
        OS.QColor_delete(newColorHandle);
    }
    
    return rgb;
}

/**
 * Sets the receiver's selected color to be the argument.
 *
 * @param rgb the new RGB value for the selected color, may be
 *        null to let the platform select a default when
 *        open() is called
 * @see PaletteData#getRGBs
 */
public void setRGB (RGB rgb) {
    this.rgb = rgb;
}
}
