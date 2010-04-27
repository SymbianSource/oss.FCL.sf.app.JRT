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
import org.eclipse.swt.events.*;

/**
 * Instances of this class represent a selectable user interface object that
 * issues notification when pressed and released.
 * <dl>
 * <dt><b>Styles:</b></dt>
 * <dd>CHECK, PUSH, RADIO, TOGGLE, FLAT</dd>
 * <dd>LEFT, RIGHT, CENTER</dd>
 * <dt><b>Events:</b></dt>
 * <dd>Selection</dd>
 * </dl>
 * <p>
 * Note: Only one of the styles CHECK, PUSH, RADIO, and TOGGLE
 * may be specified.
 * </p><p>
 * Note: Only one of the styles LEFT, RIGHT, and CENTER may be specified.
 * </p><p>
 * IMPORTANT: This class is intended to be subclassed <em>only</em>
 * within the SWT implementation.
 * </p>
 */

public class Button extends Control
{
    private int    buttonHandle;
    private Image  image = null;
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
     * @see SWT#CHECK
     * @see SWT#PUSH
     * @see SWT#RADIO
     * @see SWT#TOGGLE
     * @see SWT#FLAT
     * @see SWT#LEFT
     * @see SWT#RIGHT
     * @see SWT#CENTER
     * @see Widget#checkSubclass
     * @see Widget#getStyle
     */
    public Button(Composite parent, int style)
    {
        super(parent,checkStyle(style));
    }

    /**
     * Adds the listener to the collection of listeners who will
     * be notified when the control is selected, by sending
     * it one of the messages defined in the <code>SelectionListener</code>
     * interface.
     * <p>
     * <code>widgetSelected</code> is called when the control is selected.
     * <code>widgetDefaultSelected</code> is not called.
     * </p>
     *
     * @param listener the listener which should be notified
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see SelectionListener
     * @see #removeSelectionListener
     * @see SelectionEvent
     */
    public void addSelectionListener(SelectionListener listener)
    {
        checkWidget();
        if (listener == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        TypedListener typedListener = new TypedListener(listener);
        addListener(SWT.Selection,typedListener);
        addListener(SWT.DefaultSelection,typedListener);
    }
    /**
     * Returns a value which describes the position of the
     * text or image in the receiver. The value will be one of
     * <code>LEFT</code>, <code>RIGHT</code> or <code>CENTER</code>.
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
        return OS.Button_GetAlignment(buttonHandle);
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
     * Returns <code>true</code> if the receiver is selected,
     * and false otherwise.
     * <p>
     * When the receiver is of type <code>CHECK</code> or <code>RADIO</code>,
     * it is selected when it is checked. When it is of type <code>TOGGLE</code>,
     * it is selected when it is pushed in. If the receiver is of any other type,
     * this method returns false.
     *
     * @return the selection state
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public boolean getSelection()
    {
        checkWidget();
        return OS.Button_GetSelection(buttonHandle);
    }

    /**
     * Returns the receiver's text, which will be an empty
     * string if it has never been set.
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
     * Removes the listener from the collection of listeners who will
     * be notified when the control is selected.
     *
     * @param listener the listener which should be notified
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see SelectionListener
     * @see #addSelectionListener
     */
    public void removeSelectionListener(SelectionListener listener)
    {
        removeListener(SWT.Selection, listener);
        removeListener(SWT.DefaultSelection, listener);
    }

    /**
     * Controls how text and images will be displayed
     * in the receiver. The argument should be one of
     * <code>LEFT</code>, <code>RIGHT</code> or <code>CENTER</code>.
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

        if ((alignment == SWT.LEFT) || (alignment == SWT.CENTER) || (alignment == SWT.RIGHT))
        {
            OS.Button_SetAlignment(buttonHandle, alignment);
        }
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
        int sendValue = 0;
        if (image == null)
        {
            sendValue = 0;
        }
        else
        {
            if (image.isDisposed())
            {
                internal_error(SWT.ERROR_INVALID_ARGUMENT);
            }
            else
            {
                this.image = image;
                sendValue = this.image.handle;
            }
        }

        OS.Button_SetImage(buttonHandle, sendValue);
    }

    /**
     * Sets the selection state of the receiver, if it is of type <code>CHECK</code>,
     * <code>RADIO</code>, or <code>TOGGLE</code>.
     *
     * <p>
     * When the receiver is of type <code>CHECK</code> or <code>RADIO</code>,
     * it is selected when it is checked. When it is of type <code>TOGGLE</code>,
     * it is selected when it is pushed in.
     *
     * @param selected the new selection state
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setSelection(boolean selected)
    {
        checkWidget();

        // Set the selection if button is type CHECK, RADIO or TOGGLE
        // i.e. the button is not type PUSH
        if ((style & SWT.PUSH) == 0)
        {
            OS.Button_SetSelection(buttonHandle, selected);
        }
    }

    /**
     * Sets the receiver's text.
     * <p>
     * This method sets the button label.  The label may include
     * the mnemonic character but must not contain line delimiters.
     * </p>
     * <p>
     * Mnemonics are indicated by an '&amp' that causes the next
     * character to be the mnemonic.  When the user presses a
     * key sequence that matches the mnemonic, a selection
     * event occurs. On most platforms, the mnemonic appears
     * underlined but may be emphasised in a platform specific
     * manner.  The mnemonic indicator character '&amp' can be
     * escaped by doubling it in the string, causing a single
     *'&amp' to be displayed.
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
        if (string == null)
        {
            internal_error(SWT.ERROR_NULL_ARGUMENT);
        }
        // Mnemonics are not supported on Symbian platform.
        text = checkMnemonic(string);

        OS.Button_SetText(buttonHandle, text);
    }

//Internals methods.
    private String checkMnemonic(String string)
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

    private static int checkStyle(int style)
    {
        int styleLocal = internal_checkBits(style, SWT.PUSH, SWT.CHECK, SWT.RADIO, SWT.TOGGLE, 0, 0);

        if (((styleLocal & SWT.PUSH) | (styleLocal & SWT.TOGGLE)) != 0)
        {
            styleLocal = internal_checkBits(styleLocal, SWT.CENTER, SWT.LEFT, SWT.RIGHT, 0, 0, 0);
        }
        else if (((styleLocal & SWT.CHECK) | (styleLocal & SWT.RADIO)) != 0)
        {
            styleLocal = internal_checkBits(styleLocal, SWT.LEFT, SWT.CENTER, SWT.RIGHT, 0, 0, 0);
        }

        return styleLocal;
    }

    private final void internal_setButtonHandle(int buttonHandle)
    {
        this.buttonHandle = buttonHandle;
        handle = OS.Button_ControlHandle(buttonHandle);
    }
// inherited from control :
    protected void internal_createHandle()
    {
        internal_setButtonHandle(OS.Button_New(this, parent.compositeHandle, style));
    }

// Used by Decoration.
    int GetButtonHandle()
    {
        return buttonHandle;
    }

}
