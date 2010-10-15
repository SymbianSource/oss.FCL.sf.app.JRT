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

import org.eclipse.swt.SWT;
import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.internal.qt.WidgetState;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Internal_GfxPackageSupport;
import org.eclipse.swt.graphics.Point;

/**
 * Instances of this class represent a non-selectable user interface object that
 * displays a string or image. When SEPARATOR is specified, displays a single
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
 * </p>
 * <p>
 * IMPORTANT: This class is intended to be subclassed <em>only</em> within the
 * SWT implementation.
 * </p>
 */
public class Label extends Control {
    Image image;

    /**
     * Constructs a new instance of this class given its parent and a style
     * value describing its behavior and appearance.
     * <p>
     * The style value is either one of the style constants defined in class
     * <code>SWT</code> which is applicable to instances of this class, or
     * must be built by <em>bitwise OR</em>'ing together (that is, using the
     * <code>int</code> "|" operator) two or more of those <code>SWT</code>
     * style constants. The class description lists the style constants that are
     * applicable to the class. Style bits are also inherited from superclasses.
     * </p>
     *
     * @param parent
     *            a composite control which will be the parent of the new
     *            instance (cannot be null)
     * @param style
     *            the style of control to construct
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
    public Label(Composite parent, int style) {
        this(parent, checkStyle(style), false);
    }

    /**
     * <p>
     * <b>IMPORTANT:</b> This constructor is <em>not</em> part of the SWT
     * public API. It should never be referenced from application code.
     * </p>
     */
    protected Label(Composite parent, int style, boolean isExtended) {
        super(parent, checkStyle(style), 0, null, isExtended);
    }

    static int checkStyle(int style) {
        style |= SWT.NO_FOCUS;
        if ((style & SWT.SEPARATOR) != 0) {
            style = checkBits(style, SWT.HORIZONTAL, SWT.VERTICAL, 0, 0, 0, 0);
            return checkBits(style, SWT.SHADOW_NONE, SWT.SHADOW_OUT,
                    SWT.SHADOW_IN, 0, 0, 0)
                    | SWT.BORDER;
        }
        return checkBits(style, SWT.LEFT, SWT.CENTER, SWT.RIGHT, 0, 0, 0);
    }

    public Point computeSize(int wHint, int hHint, boolean changed) {

        if (wHint != SWT.DEFAULT && wHint < 0)
            wHint = SWT.DEFAULT;
        if (hHint != SWT.DEFAULT && hHint < 0)
            hHint = SWT.DEFAULT;

        if ((style & SWT.SEPARATOR) != 0 || image != null
                || (style & SWT.SINGLE) != 0
                || (wHint == SWT.DEFAULT && hHint == SWT.DEFAULT)
                || (wHint != SWT.DEFAULT && hHint != SWT.DEFAULT)) {
            return super.computeSize(wHint, hHint, changed);
        }

        checkWidget();
        
        if (changed) {
            OS.QWidget_updateGeometry(handle);
        }
        
        /**
         * ComputeSize in super class is not able to handle the situation that
         * preferred height depends on the width. So ask QLabel for preferred
         * height or width.
         */

        int border = getBorderWidth();
        if (wHint != SWT.DEFAULT)
            wHint += 2 * border;
        if (hHint != SWT.DEFAULT)
            hHint += 2 * border;

        Point minimumSize = OS.QWidget_minimumSize(handle);
        if (minimumSize.x < 2 * border)
            minimumSize.x = 2 * border;
        if (minimumSize.y < 2 * border)
            minimumSize.y = 2 * border;

        int width = minimumSize.x;
        int height = minimumSize.y;
        if (wHint > width)
            width = wHint;
        if (hHint > height)
            height = hHint;
        /**
         * OS.QWidget_heightForWidth is also used for 'width for height'. In
         * that case, the result may not be accurate.
         */
        int givenLength = wHint != SWT.DEFAULT ? width : height;
        int defaultLength = OS.QWidget_heightForWidth(handle, givenLength);
        if (defaultLength >= 0) {
            if (wHint == SWT.DEFAULT)
                width = defaultLength;
            else
                height = defaultLength;
        }
        
        return new Point(width, height);
    }

    void createHandle_pp(int index) {
        if ((style & SWT.SEPARATOR) != 0) {
            handle = OS.QFrame_new();
        } else {
            handle = OS.QLabel_new();
            /**
             * Ampersands for keyboard shortcuts are not processed at all and
             * there won't be a shortcut unless buddy is set. The parent is
             * used. Shortcut event can then be canceled and focus moved
             * manually. This way buddy doesn't ever need to be updated.
             */
            OS.QLabel_setBuddy(handle, parent.handle);
        }

        setStyle(style);
        frameHandle = topHandle = handle;
        state |= WidgetState.HANDLE;
        if ((style & SWT.SEPARATOR) != 0)
            return;
        setAlignment(style);
    }

    /**
     * Returns a value which describes the position of the text or image in the
     * receiver. The value will be one of <code>LEFT</code>,
     * <code>RIGHT</code> or <code>CENTER</code> unless the receiver is a
     * <code>SEPARATOR</code> label, in which case, <code>NONE</code> is
     * returned.
     *
     * @return the alignment
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public int getAlignment() {
        checkWidget();
        if ((style & SWT.SEPARATOR) != 0)
            return 0;
        if ((style & SWT.LEFT) != 0)
            return SWT.LEFT;
        if ((style & SWT.CENTER) != 0)
            return SWT.CENTER;
        if ((style & SWT.RIGHT) != 0)
            return SWT.RIGHT;
        return SWT.LEFT;
    }

    /**
     * Returns the receiver's image if it has one, or null if it does not.
     *
     * @return the receiver's image
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public Image getImage() {
        checkWidget();
        return image;
    }

    String getNameText() {
        return getText();
    }

    /**
     * Returns the receiver's text, which will be an empty string if it has
     * never been set or if the receiver is a <code>SEPARATOR</code> label.
     *
     * @return the receiver's text
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public String getText() {
        checkWidget();
        if ((style & SWT.SEPARATOR) != 0)
            return "";
        return OS.QLabel_text(handle);
    }

    void releaseWidget_pp() {
        super.releaseWidget_pp();
        image = null;
    }

    /**
     * Controls how text and images will be displayed in the receiver. The
     * argument should be one of <code>LEFT</code>, <code>RIGHT</code> or
     * <code>CENTER</code>. If the receiver is a <code>SEPARATOR</code>
     * label, the argument is ignored and the alignment is not changed.
     *
     * @param alignment
     *            the new alignment
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void setAlignment(int alignment) {
        checkWidget();
        if ((style & SWT.SEPARATOR) != 0)
            return;
        alignment &= SWT.LEFT | SWT.RIGHT | SWT.CENTER;
        if (alignment == 0)
            return;
        alignment = checkBits(alignment, SWT.LEFT, SWT.RIGHT, SWT.CENTER, 0, 0,
                0);
        style &= ~(SWT.LEFT | SWT.RIGHT | SWT.CENTER);
        style |= alignment & (SWT.LEFT | SWT.RIGHT | SWT.CENTER);

        int qAlign = 0;
        if ((style & SWT.LEFT) != 0) {
            qAlign |= OS.QT_ALIGNLEFT;
        } else if ((style & SWT.CENTER) != 0) {
            qAlign |= OS.QT_ALIGNHCENTER;
        } else if ((style & SWT.RIGHT) != 0) {
            qAlign |= OS.QT_ALIGNRIGHT;
        }

        qAlign |= OS.QT_ALIGNVCENTER;

        OS.QLabel_setAlignment(handle, qAlign);
    }

    void setStandardIcon(int iconType, int iconWidth, int iconHeight) {
        checkWidget();
        if ((style & SWT.SEPARATOR) != 0)
            return;
        int styleHandle = OS.QApplication_style();
        int iconHandle = 0;
        int pixmapHandle = 0;
        try {
            iconHandle = OS.QStyle_standardIcon(styleHandle, iconType);
            pixmapHandle = OS.QIcon_pixmap(iconHandle, iconWidth, iconHeight);
            OS.QLabel_setPixmap(topHandle, pixmapHandle);
        } finally {
            OS.QIcon_delete(iconHandle);
            OS.QPixmap_delete(pixmapHandle);
        }
    }
    
    void setThemeIcon(String name) {
        checkWidget();
        if ((style & SWT.SEPARATOR) != 0)
            return;
        int iconHandle = 0;
        int pixmapHandle = 0;
        try {
            iconHandle = OS.HbIcon_new(name);
            if (iconHandle != 0)
                pixmapHandle = OS.HbIcon_pixmap(iconHandle);
            if (pixmapHandle != 0)
                OS.QLabel_setPixmap(topHandle, pixmapHandle);
        } finally {
            OS.HbIcon_delete(iconHandle);
            OS.QPixmap_delete(pixmapHandle);
        }
    }
    

    void setStyle(int style) {
        int qStyle = 0;
        if ((style & SWT.SEPARATOR) != 0) {
            if ((style & SWT.VERTICAL) != 0) {
                qStyle |= OS.QT_FRAMEVLINE;
            } else {
                qStyle |= OS.QT_FRAMEHLINE;
            }

            if ((style & SWT.SHADOW_IN) != 0) {
                qStyle |= OS.QT_FRAMESUNKEN;
            } else if ((style & SWT.SHADOW_OUT) != 0) {
                qStyle |= OS.QT_FRAMERAISED;
            } else {
                qStyle |= OS.QT_FRAMEPLAIN;
            }

        } else if ((style & SWT.BORDER) != 0) {
            qStyle |= OS.QT_FRAMESTYLEDPANEL;
        } else {
            qStyle |= OS.QT_NOFRAME;
        }

        OS.QFrame_setFrameStyle(handle, qStyle);

        if ((style & SWT.SEPARATOR) != 0) {
            return;
        }

        if ((style & SWT.WRAP) != 0) {
            OS.QLabel_setWordWrap(handle, true);
        }
    }

    /**
     * Sets the receiver's image to the argument, which may be null indicating
     * that no image should be displayed.
     *
     * @param image
     *            the image to display on the receiver (may be null)
     *
     * @exception IllegalArgumentException
     *                <ul>
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
     */
    public void setImage(Image image) {
        checkWidget();
        if ((style & SWT.SEPARATOR) != 0)
            return;
        if (image != null) {
            if (image.isDisposed())
                SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        }
        if (this.image == image)
            return;
        OS.QLabel_swt_setPixmap(handle, image == null ? 0 : Internal_GfxPackageSupport.getImageHandle(image));
        this.image = image;
    }

    /**
     * Sets the receiver's text.
     * <p>
     * This method sets the widget label. The label may include the mnemonic
     * character and line delimiters.
     * </p>
     * <p>
     * Mnemonics are indicated by an '&amp;' that causes the next character to
     * be the mnemonic. When the user presses a key sequence that matches the
     * mnemonic, focus is assigned to the control that follows the label. On
     * most platforms, the mnemonic appears underlined but may be emphasised in
     * a platform specific manner. The mnemonic indicator character '&amp;' can
     * be escaped by doubling it in the string, causing a single '&amp;' to be
     * displayed.
     * </p>
     *
     * @param string
     *            the new text
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the text is null</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void setText(String string) {
        checkWidget();
        if (string == null)
            error(SWT.ERROR_NULL_ARGUMENT);
        if ((style & SWT.SEPARATOR) != 0)
            return;
        if (image != null) {
            image = null;
            // empty string does not erase image
            if (string.length() < 1) {
                OS.QLabel_setPixmap(handle, 0);
                return;
            }
        }

        OS.QLabel_setText(handle, string);
    }

    boolean isTabGroup() {
        return false;
    }

    boolean isTabItem() {
        return false;
    }
}
