/*******************************************************************************
 * Copyright (c) 2004, 2007 Nokia Corporation.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Yu You (Nokia Corp.)- initial API specification
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/
package org.eclipse.ercp.swt.mobile;

import org.eclipse.ercp.swt.mobile.internal.OS;
import org.eclipse.swt.*;
import org.eclipse.swt.graphics.*;
import org.eclipse.swt.widgets.*;

/**
 *
 * A CaptionedControl is used to display a label (caption) in front of a
 * control. An optional trailing text can be used after the control, for
 * example, to indicate units of measurement.
 * <p>
 * The highlighting is implementation dependent. Typically, a focused
 * CaptionedControl will highlight all its elements, i.e. the leading label, the
 * control, and optional trailing label, when it gets focus. The positioning
 * order for the captions is determined by the <code>SWT.LEFT_TO_RIGHT</code>
 * and <code>SWT.RIGHT_TO_LEFT</code> styles hints.
 * </p>
 *
 * <p>
 * CaptionedControl does not support nested CaptionedControls. An exception will
 * be thrown when an instance of CaptionedControl is given as the constructor's
 * argument. To change the control contained by the CaptionedControl, a new
 * control can be created using the CaptionedControl as its parent control.
 * The old control must be disposed by the application manually. Only one control
 * can be active and visible in a CaptionedControl. Which control is displayed
 * when multiple controls have been added is implementation-dependent.
 * </p>
 * <p>
 * <em>Example code:</em>
 * </p>
 * <p>
 * <code><pre>
 * CaptionedControl control = new CaptionedControl(shell, SWT.NONE);
 * ConstraintedText currency = new ConstraintedText(control, SWT.NONE,
 *      ConstraintedText.NUMERIC);
 * control.setText(&quot;Velocity&quot;);
 * control.getTrailingText(&quot;cm&quot;);
 * </pre></code>
 * </p>
 *
 * <p>
 * <dl>
 * <dt><b>Styles: </b></dt>
 * <dd>SWT.LEFT_TO_RIGHT (default)</dd>
 * <dd>SWT.RIGHT_TO_LEFT</dd>
 * <dt><b>Events: </b></dt>
 * <dd>(none)</dd>
 * </dl>
 * <p>
 * Note: Even this class is a subclass of Composite, it does not make sense to
 * set a layout on it.
 * </p>
 * <p>
 * IMPORTANT: This class is not intended to be subclassed.
 * </p>
 *
 */
public final class CaptionedControl extends Composite
{
    private int captionedControlHandle;
    Image image;

    /**
     * Constructs a new instance of this class given its parent and a style
     * value describing its behavior and appearance.
     * <p>
     * The style value is either one of the style constants defined in class
     * <code>SWT</code> which is applicable to instances of this class, or
     * must be built by <em>bitwise OR</em> 'ing together (that is, using the
     * <code>int</code> "|" operator) two or more of those <code>SWT</code>
     * style constants. The class description lists the style constants that are
     * applicable to the class. Style bits are also inherited from superclasses.
     * </p>
     *
     * @param parent
     *            a widget which will be the parent of the new instance (cannot
     *            be null)
     * @param style
     *            the style of widget to construct
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the parent is null</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                <li>ERROR_INVALID_SUBCLASS - if this class is not an
     *                allowed subclass</li>
     *                </ul>
     *
     * @see SWT#LEFT_TO_RIGHT
     * @see SWT#RIGHT_TO_LEFT
     */
    public CaptionedControl(Composite parent, int style)
    {
        super(parent, style);
    }

    protected void internal_checkParentClass(Widget parent)
    {
        if (parent instanceof CaptionedControl)
        {
            internal_error(SWT.ERROR_INVALID_PARENT);
        }
    }

    final void setCaptionedControlHandle(int captionedControlHandle)
    {
        this.captionedControlHandle = captionedControlHandle;
        internal_setCompositeHandle(OS.CaptionedControl_CompositeHandle(captionedControlHandle));
    }

    protected void internal_createHandle()
    {
        setCaptionedControlHandle(OS.CaptionedControl_New(this, getParent().internal_getCompositeHandle(), internal_getStyle()));
    }

    public Point computeSize(int wHint, int hHint, boolean changed)
    {
        checkWidget();
        if (getLayout() != null)
        {
            return super.computeSize(wHint, hHint, changed);
        }
        else
        {
            // Compute preferred size of child if it is a Composite with a layout
            // so that native CaptionedControl can return correct result
            computeChildSize();
            return org.eclipse.swt.internal.symbian.OS.Control_ComputeSize(handle, wHint, hHint);
        }
    }

    /**
     * Sets the caption label
     *
     * @param string
     *            the new caption label
     *
     * @throws java.lang.IllegalArgumentException
     *             <code>ERROR_NULL_ARGUMENT</code> if the text is null
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                </ul>
     * @see #getText
     */
    public void setText(java.lang.String string)
    {
        checkWidget();
        if (string == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        // Compute preferred size of child if it is a Composite with a layout
        // so that native CaptionedControl can resize and relocate its elements properly
        computeChildSize();
        OS.CaptionedControl_SetText(captionedControlHandle, string);
    }

    /**
     * Sets the trailing label
     *
     * @param string
     *            the new trailing label
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the string is null</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                </ul>
     *
     * @see #getTrailingText
     */
    public void setTrailingText(java.lang.String string)
    {
        checkWidget();
        if (string == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        // Compute preferred size of child if it is a Composite with a layout
        // so that native CaptionedControl can resize and relocate its elements properly
        computeChildSize();
        OS.CaptionedControl_SetTrailingText(captionedControlHandle, string);
    }

    /**
     * Gets the caption text, which will be an empty string if it has never been
     * set.
     *
     * @return The label text.
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                </ul>
     *
     * @see #setText(java.lang.String)
     */
    public String getText()
    {
        checkWidget();
        String ret = OS.CaptionedControl_GetText(captionedControlHandle);
        if (ret == null)
        {
            return "";
        }
        else
        {
            return ret;
        }
    }

    /**
     * Gets the trailing text, which will be an empty string if it has never
     * been set.
     *
     * @return The trailing text.
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                </ul>
     *
     * @see #setTrailingText(java.lang.String)
     */
    public String getTrailingText()
    {
        checkWidget();
        String ret = OS.CaptionedControl_GetTrailingText(captionedControlHandle);
        if (ret == null)
        {
            return "";
        }
        else
        {
            return ret;
        }
    }

    /**
     * Sets the image as an icon to the CaptionedControl. The icon can co-exist
     * with caption text. The icon position is platform-dependent.
     *
     * <p>
     * The parameter can be null indicating that no image should be displayed.
     * The implementation will adjust the image size to make it best fit the
     * CaptionedControl.
     * </p>
     *
     * @param image
     *            the image to display on the receiver
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the image is null</li>
     *                <li>ERROR_INVALID_ARGUMENT - if the image has been
     *                disposed</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     *
     */
    public void setImage(Image image)
    {
        checkWidget();
        if (image != null && image.isDisposed()) internal_error(SWT.ERROR_INVALID_ARGUMENT);
        this.image = image;
        // Compute preferred size of child if it is a Composite with a layout
        // so that native CaptionedControl can resize and relocate its elements properly
        computeChildSize();
        OS.CaptionedControl_SetImage(captionedControlHandle, (image == null)? 0 : this.image.handle);
    }

    /**
     * Returns the CaptionedControl's icon image, or null if it has never been
     * set.
     *
     * @return the icon image or null.
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     * @see #setImage(Image)
     */
    public Image getImage()
    {
        checkWidget();
        return image;
    }

    /**
     * Computes the preferred size of child and stores the result on native side.
     */
    void computeChildSize()
    {
        Point size;
        Control[] children = getChildren();
        if (children.length != 0)
        {
            Control child = children[0];
            size = child.computeSize(SWT.DEFAULT, SWT.DEFAULT);
        }
        else
        {
            size = new Point(0, 0);
        }

        OS.CaptionedControl_SetChildPreferredSize(captionedControlHandle, size.x, size.y);
    }

    public void setBounds(int x, int y, int width, int height)
    {
        checkWidget();
        // Compute preferred size of child if it is a Composite with a layout
        // so that native CaptionedControl can resize and relocate its elements properly
        computeChildSize();
        super.setBounds(x, y, width, height);
    }

    public void setSize(int width, int height)
    {
        checkWidget();
        // Compute preferred size of child if it is a Composite with a layout
        // so that native CaptionedControl can resize and relocate its elements properly
        computeChildSize();
        super.setSize(width, height);
    }

    public void setLayout(Layout layout)
    {
        // Even though this class is a subclass of Composite, setting a layout on it is not supported.
        checkWidget();
    }

    public Layout getLayout()
    {
        // Even though this class is a subclass of Composite, setting a layout on it is not supported.
        checkWidget();
        return null;
    }

    public void layout()
    {
        // Even though this class is a subclass of Composite, setting a layout on it is not supported.
        checkWidget();
    }

    public void layout(boolean changed)
    {
        // Even though this class is a subclass of Composite, setting a layout on it is not supported.
        checkWidget();
    }
}
