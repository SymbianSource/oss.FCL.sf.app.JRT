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
import org.eclipse.swt.graphics.*;
import org.eclipse.swt.internal.symbian.OS;

/**
 * Instances of this class represent a non-selectable
 * user interface object that displays a string or image.
 * When SEPARATOR is specified, displays a single
 * vertical or horizontal line.
 * <dl>
 * <dt><b>Styles:</b></dt>
 * <dd>SEPARATOR, HORIZONTAL, VERTICAL</dd>
 * <dd>SHADOW_IN, SHADOW_OUT, SHADOW_NONE</dd>
 * <dd>CENTER, LEFT, RIGHT, WRAP</dd>
 * <dt><b>Events:</b></dt>
 * <dd>(none)</dd>
 * </dl>
 * <p>
 * Note: Only one of SHADOW_IN, SHADOW_OUT and SHADOW_NONE may be specified.
 * SHADOW_NONE is a HINT. Only one of HORIZONTAL and VERTICAL may be specified.
 * Only one of CENTER, LEFT and RIGHT may be specified.
 * </p><p>
 * IMPORTANT: This class is intended to be subclassed <em>only</em>
 * within the SWT implementation.
 * </p>
 */
public class Label extends Control
{
    private int    labelHandle;
    private Image  image;
    private String text = "";

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
     * @see SWT#SEPARATOR
     * @see SWT#HORIZONTAL
     * @see SWT#VERTICAL
     * @see SWT#SHADOW_IN
     * @see SWT#SHADOW_OUT
     * @see SWT#SHADOW_NONE
     * @see SWT#CENTER
     * @see SWT#LEFT
     * @see SWT#RIGHT
     * @see SWT#WRAP
     * @see Widget#checkSubclass
     * @see Widget#getStyle
     */
    public Label(Composite parent, int style)
    {
        super(parent, internal_checkStyle(style));
    }

    protected final void internal_setLabelHandle(int labelHandle)
    {
        this.labelHandle = labelHandle;
        handle = OS.Label_ControlHandle(labelHandle);
    }

    protected void internal_createHandle()
    {
        internal_setLabelHandle(OS.Label_New(this, parent.compositeHandle, style));
    }

    protected static int internal_checkStyle(int style)
    {
        style |= SWT.NO_FOCUS;
        if ((style & SWT.SEPARATOR) != 0)
        {
            style = internal_checkBits(style, SWT.HORIZONTAL, SWT.VERTICAL, 0, 0, 0, 0);
            return internal_checkBits(style, SWT.SHADOW_OUT, SWT.SHADOW_NONE, SWT.SHADOW_IN, 0, 0, 0);
        }
        return internal_checkBits(style, SWT.LEAD, SWT.CENTER, SWT.TRAIL, 0, 0, 0);
    }

    public Point computeSize(int wHint, int hHint, boolean changed)
    {
        checkWidget();
        return OS.Control_ComputeSize(handle, wHint, hHint);
    }

    /**
     * Disable SetMenu method because label is a non-focusable widget.
     *
     */
    /*
    public void setMenu (Menu menu) {
        checkWidget ();
    }
    */



    /**
     * Returns a value which describes the position of the
     * text or image in the receiver. The value will be one of
     * <code>LEFT</code>, <code>RIGHT</code> or <code>CENTER</code>
     * unless the receiver is a <code>SEPARATOR</code> label, in
     * which case, <code>NONE</code> is returned.
     *
     * @return the alignment
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public int getAlignment()
    {
        checkWidget();
        if ((style & SWT.SEPARATOR) != 0) return SWT.NONE;
        return OS.Label_GetAlignment(labelHandle);
    }

    /**
     * Returns the receiver's image if it has one, or null
     * if it does not.
     *
     * @return the receiver's image
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public Image getImage()
    {
        checkWidget();
        return image;
    }

    /**
     * Returns the receiver's text, which will be an empty
     * string if it has never been set or if the receiver is
     * a <code>SEPARATOR</code> label.
     *
     * @return the receiver's text
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public String getText()
    {
        checkWidget();
        return text;
    }

    /**
     * Controls how text and images will be displayed in the receiver.
     * The argument should be one of <code>LEFT</code>, <code>RIGHT</code>
     * or <code>CENTER</code>.  If the receiver is a <code>SEPARATOR</code>
     * label, the argument is ignored and the alignment is not changed.
     *
     * @param alignment the new alignment
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setAlignment(int alignment)
    {
        checkWidget();
        if ((style & SWT.SEPARATOR) == 0) OS.Label_SetAlignment(labelHandle, alignment);
    }

    /**
     * Sets the receiver's image to the argument, which may be
     * null indicating that no image should be displayed.
     *
     * @param image the image to display on the receiver (may be null)
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_ARGUMENT - if the image has been disposed</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setImage(Image image)
    {
        checkWidget();
        if ((style & SWT.SEPARATOR) != 0) return;

        this.image = image;
        OS.Label_SetImage(labelHandle, (this.image == null) ? 0 : this.image.handle);
    }

    String checkMnemonic(String string)
    {
        int i = 0;

        String result = new String(string);
        while (i < result.length())
        {
            if (result.charAt(i) == '&')
            {
                result = result.substring(0, i) + result.substring(i+1, result.length());
                if (i < result.length())
                {
                    if (result.charAt(i) == '&')
                    {
                        i++;
                    }
                }
            }
            else
            {
                i ++;
            }
        }

        return result;
    }

    /**
     * Sets the receiver's text.
     * <p>
     * This method sets the widget label.  The label may include
     * the mnemonic character and line delimiters.
     * </p>
     * <p>
     * Mnemonics are indicated by an '&amp' that causes the next
     * character to be the mnemonic.  When the user presses a
     * key sequence that matches the mnemonic, focus is assigned
     * to the control that follows the label. On most platforms,
     * the mnemonic appears underlined but may be emphasised in a
     * platform specific manner.  The mnemonic indicator character
     *'&amp' can be escaped by doubling it in the string, causing
     * a single '&amp' to be displayed.
     * </p>
     *
     * @param string the new text
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the text is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setText(String string)
    {
        checkWidget();
        if (string == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        if ((style & SWT.SEPARATOR) != 0) return;

        text = checkMnemonic(string);
        OS.Label_SetText(labelHandle, text);
    }

}
