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


import java.util.Vector;

import org.eclipse.swt.*;
import org.eclipse.swt.graphics.*;
import org.eclipse.ercp.swt.expanded.internal.OS;


/**
 * Instances of this class represent a selectable user interface object
 * that represents an item in a table.
 * <dl>
 * <dt><b>Styles:</b></dt>
 * <dd>(none)</dd>
 * <dt><b>Events:</b></dt>
 * <dd>(none)</dd>
 * </dl>
 * <p>
 * IMPORTANT: This class is <em>not</em> intended to be subclassed.
 * </p>
 */

public class TableItem extends Item
{
    private int    handle;
    private Table  parent;
    private Vector images = new Vector();
    private Color background;
    private Color foreground;
    private Vector cellBackgrounds = new Vector();
    private Vector cellForegrounds = new Vector();


    /**
     * Constructs a new instance of this class given its parent
     * (which must be a <code>Table</code>) and a style value
     * describing its behavior and appearance. The item is added
     * to the end of the items maintained by its parent.
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
    public TableItem(Table parent, int style)
    {
        super(parent, style);

        internal_init(parent, parent.getItemCount());
    }

    /**
     * Constructs a new instance of this class given its parent
     * (which must be a <code>Table</code>), a style value
     * describing its behavior and appearance, and the index
     * at which to place it in the items maintained by its parent.
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
     * @param index the index to store the receiver in its parent
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
    public TableItem(Table parent, int style, int index)
    {
        super(parent, style, index);

        internal_init(parent, index);
    }

//Initialise the TableItem, and create the native resources.
    private void internal_init(Table parent, int index)
    {
        // ERROR_INVALID_RANGE not in javadoc but we assume it is the expected behaviour
        if (index < 0 || index > parent.getItemCount())
        {
            error(SWT.ERROR_INVALID_RANGE);
        }

        this.parent = parent;
        this.handle = OS.TableItem_New(this, parent.tableHandle, getStyle(), index);

        parent.tableItems.addElement(this);

        images.setSize(imageVectorSize());
        cellBackgrounds.setSize(imageVectorSize());
        cellForegrounds.setSize(imageVectorSize());
    }


    protected void checkSubclass()
    {
        if (!internal_isValidSubclass()) internal_error(SWT.ERROR_INVALID_SUBCLASS);
    }

    /**
     * Returns the receiver's background color.
     *
     * @return the background color
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @since 2.0
     *
     */
    public Color getBackground()
    {
        checkWidget();
        int colorHandle = OS.TableItem_GetBackground(handle);
        Color newColor = Color.internal_new(display, colorHandle);
        if (background == null || background.isDisposed() || !background.equals(newColor))
        {
            if (background != null && !background.isDisposed())
            {
                background.dispose();
            }
            background = newColor;
        }
        else
        {
            newColor.dispose();
        }
        return background;
    }

    /**
     * Returns the background color at the given column index in the receiver.
     *
     * @param index the column index
     * @return the background color
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @since 3.0
     */
    public Color getBackground(int index)
    {
        checkWidget();

        if (internal_checkValidIndex(index))
        {
            int colorHandle = OS.TableItem_GetBackground(handle, index);
            if (cellBackgrounds.elementAt(index) != null &&
                    !((Color)cellBackgrounds.elementAt(index)).isDisposed())
            {
                ((Color)cellBackgrounds.elementAt(index)).dispose();
            }
            cellBackgrounds.setElementAt(Color.internal_new(display, colorHandle), index);
            return (Color)cellBackgrounds.elementAt(index);
        }

        // Index out of bound: do nothing. No error.
        return getBackground();
    }

    /**
     * Returns a rectangle describing the receiver's size and location
     * relative to its parent at a column in the table.
     *
     * @param index the index that specifies the column
     * @return the receiver's bounding column rectangle
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public Rectangle getBounds(int index)
    {
        checkWidget();
        if (internal_checkValidIndex(index))
        {
            return OS.TableItem_GetBounds(handle, index);
        }

        // Index out of bound: do nothing. No error.
        return new Rectangle(0, 0, 0, 0);
    }

    /**
     * Returns <code>true</code> if the receiver is checked,
     * and false otherwise.  When the parent does not have
     * the <code>CHECK</code> style, return false.
     *
     * @return the checked state of the checkbox
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public boolean getChecked()
    {
        checkWidget();
        return OS.TableItem_GetChecked(handle);
    }

    /**
     * Returns the font that the receiver will use to paint textual information for this item.
     *
     * @return the receiver's font
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @since 3.0
     */
    public Font getFont()
    {
        checkWidget();
        int fontHandle = OS.TableItem_GetFont(handle);
        return Font.internal_new(display, fontHandle);
    }

    /**
     * Returns the font that the receiver will use to paint textual information
     * for the specified cell in this item.
     *
     * @param index the column index
     * @return the receiver's font
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @since 3.0
     */
    public Font getFont(int index)
    {
        checkWidget();

        if (internal_checkValidIndex(index))
        {
            int fontHandle = OS.TableItem_GetFont(handle, index);
            return Font.internal_new(display, fontHandle);
        }

        return getFont();
    }

    /**
     * Returns the foreground color that the receiver will use to draw.
     *
     * @return the receiver's foreground color
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @since 2.0
     *
     */
    public Color getForeground()
    {
        checkWidget();
        int colorHandle = OS.TableItem_GetForeground(handle);
        Color newColor = Color.internal_new(display, colorHandle);
        if (foreground == null || foreground.isDisposed() || !foreground.equals(newColor))
        {
            if (foreground != null && !foreground.isDisposed())
            {
                foreground.dispose();
            }
            foreground = newColor;
        }
        else
        {
            newColor.dispose();
        }
        return foreground;
    }

    /**
     *
     * Returns the foreground color at the given column index in the receiver.
     *
     * @param index the column index
     * @return the foreground color
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @since 3.0
     */
    public Color getForeground(int index)
    {
        checkWidget();

        if (internal_checkValidIndex(index))
        {
            int colorHandle = OS.TableItem_GetForeground(handle, index);
            if (cellForegrounds.elementAt(index) != null &&
                    !((Color)cellForegrounds.elementAt(index)).isDisposed())
            {
                ((Color)cellForegrounds.elementAt(index)).dispose();
            }
            cellForegrounds.setElementAt(Color.internal_new(display, colorHandle), index);
            return (Color)cellForegrounds.elementAt(index);
        }

        // Index out of bound: do nothing. No error.
        return getForeground();
    }

    /**
     * Returns <code>true</code> if the receiver is grayed,
     * and false otherwise. When the parent does not have
     * the <code>CHECK</code> style, return false.
     *
     * @return the grayed state of the checkbox
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public boolean getGrayed()
    {
        checkWidget();
        return OS.TableItem_GetGrayed(handle);
    }

    /**
     * Returns the image stored at the given column index in the receiver,
     * or null if the image has not been set or if the column does not exist.
     *
     * @param index the column index
     * @return the image stored at the given column index in the receiver
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public Image getImage(int index)
    {
        checkWidget();

        if (index >= 0 && index < images.size())
        {
            return (Image)images.elementAt(index);
        }

        // Index out of bound: do nothing. No error.
        return null;
    }


    public Image getImage()
    {
        return getImage(0);
    }


    /**
     * Returns a rectangle describing the size and location
     * relative to its parent of an image at a column in the
     * table.
     *
     * @param index the index that specifies the column
     * @return the receiver's bounding image rectangle
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public Rectangle getImageBounds(int index)
    {
        checkWidget();

        if (internal_checkValidIndex(index))
        {
            // The index is valid.
            return OS.TableItem_GetImageBounds(handle, index);
        }

        return new Rectangle(0, 0, 0, 0);
    }

    /**
     * Returns the receiver's parent, which must be a <code>Table</code>.
     *
     * @return the receiver's parent
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public Table getParent()
    {
        checkWidget();
        return parent;
    }

    /**
     * Returns the text stored at the given column index in the receiver,
     * or empty string if the text has not been set.
     *
     * @param index the column index
     * @return the text stored at the given column index in the receiver
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public String getText(int index)
    {
        checkWidget();
        if (internal_checkValidIndex(index))
        {
            return OS.TableItem_GetText(handle, index);
        }

        // Index out of bound: do nothing. No error.
        return "";
    }


// From Item
    public String getText()
    {
        return getText(0);
    }

    /**
     * Sets the receiver's background color to the color specified
     * by the argument, or to the default system color for the item
     * if the argument is null.
     *
     * @param color the new color (or null)
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_ARGUMENT - if the argument has been disposed</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @since 2.0
     *
     */
    public void setBackground(Color color)
    {
        checkWidget();
        int colorHandle = 0;
        if (color!=null)
        {
            if (color.isDisposed())
            {
                SWT.error(SWT.ERROR_INVALID_ARGUMENT);
            }
            colorHandle = color.handle;
        }

        OS.TableItem_SetBackground(handle, colorHandle);
    }

    /**
     * Sets the background color at the given column index in the receiver
     * to the color specified by the argument, or to the default system color for the item
     * if the argument is null.
     *
     * @param index the column index
     * @param color the new color (or null)
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_ARGUMENT - if the argument has been disposed</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @since 3.0
     *
     */
    public void setBackground(int index, Color color)
    {
        checkWidget();
        int colorHandle = 0;
        if (color!=null)
        {
            if (color.isDisposed())
            {
                SWT.error(SWT.ERROR_INVALID_ARGUMENT);
            }
            colorHandle = color.handle;
        }
        if (internal_checkValidIndex(index))
        {
            OS.TableItem_SetBackground(handle, colorHandle, index);
        }
    }


    /**
     * Sets the checked state of the checkbox for this item.  This state change
     * only applies if the Table was created with the SWT.CHECK style.
     *
     * @param checked the new checked state of the checkbox
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setChecked(boolean checked)
    {
        checkWidget();
        if ((parent.getStyle() & SWT.CHECK) == 0)
        {
            return;
        }
        OS.TableItem_SetChecked(handle, checked);
    }

    /**
     * Sets the font that the receiver will use to paint textual information
     * for this item to the font specified by the argument, or to the default font
     * for that kind of control if the argument is null.
     *
     * @param font the new font (or null)
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_ARGUMENT - if the argument has been disposed</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @since 3.0
     */
    public void setFont(Font font)
    {
        checkWidget();
        int fontHandle = 0;
        if (font!=null)
        {
            if (font.isDisposed())
            {
                SWT.error(SWT.ERROR_INVALID_ARGUMENT);
            }
            fontHandle = font.handle;
        }

        OS.TableItem_SetFont(handle, fontHandle);
    }

    /**
     * Sets the font that the receiver will use to paint textual information
     * for the specified cell in this item to the font specified by the
     * argument, or to the default font for that kind of control if the
     * argument is null.
     *
     * @param index the column index
     * @param font the new font (or null)
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_ARGUMENT - if the argument has been disposed</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @since 3.0
     */
    public void setFont(int index, Font font)
    {
        checkWidget();
        int fontHandle = 0;
        if (font!=null)
        {
            if (font.isDisposed())
            {
                SWT.error(SWT.ERROR_INVALID_ARGUMENT);
            }
            fontHandle = font.handle;
        }

        if (internal_checkValidIndex(index))
        {
            OS.TableItem_SetFont(handle, fontHandle, index);
        }
    }


    /**
     * Sets the receiver's foreground color to the color specified
     * by the argument, or to the default system color for the item
     * if the argument is null.
     *
     * @param color the new color (or null)
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_ARGUMENT - if the argument has been disposed</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @since 2.0
     *
     */
    public void setForeground(Color color)
    {
        checkWidget();
        int colorHandle = 0;
        if (color!=null)
        {
            if (color.isDisposed())
            {
                SWT.error(SWT.ERROR_INVALID_ARGUMENT);
            }
            colorHandle = color.handle;
        }

        OS.TableItem_SetForeground(handle, colorHandle);
    }

    /**
     * Sets the foreground color at the given column index in the receiver
     * to the color specified by the argument, or to the default system color for the item
     * if the argument is null.
     *
     * @param index the column index
     * @param color the new color (or null)
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_ARGUMENT - if the argument has been disposed</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @since 3.0
     *
     */
    public void setForeground(int index, Color color)
    {
        checkWidget();
        int colorHandle = 0;
        if (color!=null)
        {
            if (color.isDisposed())
            {
                SWT.error(SWT.ERROR_INVALID_ARGUMENT);
            }
            colorHandle = color.handle;
        }
        if (internal_checkValidIndex(index))
        {
            OS.TableItem_SetForeground(handle, colorHandle, index);
        }
    }


    /**
     * Sets the grayed state of the checkbox for this item.  This state change
     * only applies if the Table was created with the SWT.CHECK style.
     *
     * @param grayed the new grayed state of the checkbox;
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setGrayed(boolean grayed)
    {
        checkWidget();
        OS.TableItem_SetGrayed(handle, grayed);
    }

    /**
     * Sets the image for multiple columns in the Table.
     *
     * @param images the array of new images
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the array of images is null</li>
     *    <li>ERROR_INVALID_ARGUMENT - if one of the images has been disposed</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setImage(Image [] images)
    {
        checkWidget();
        if (images == null)
        {
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        }

        int vectorSize = imageVectorSize();
        this.images.removeAllElements();
        this.images.setSize(vectorSize);

        int count = Math.min(images.length, vectorSize);
        int[] imageHandles = new int[count];
        for (int i=0; i<count; ++i)
        {
            Image img = images[i];
            if (img != null)
            {
                if (img.isDisposed())
                {
                    SWT.error(SWT.ERROR_INVALID_ARGUMENT);
                }
                imageHandles[i] = img.handle;
            }
            else
            {
                imageHandles[i] = 0;
            }
            this.images.setElementAt(img, i);
        }

        OS.TableItem_SetImage(handle, imageHandles);
    }

    /**
     * Sets the receiver's image at a column.
     *
     * @param index the column index
     * @param image the new image
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_INVALID_ARGUMENT - if the image has been disposed</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setImage(int index, Image image)
    {
        checkWidget();
        int imageHandle = 0;

        if (image != null)
        {
            if (image.isDisposed())
            {
                SWT.error(SWT.ERROR_INVALID_ARGUMENT);
            }
            imageHandle = image.handle;
        }

        if (internal_checkValidIndex(index))
        {
            images.setElementAt(image, index);
            OS.TableItem_SetImage(handle, index, imageHandle);
        }
    }

//From Item
    public void setImage(Image image)
    {
        setImage(0, image);
    }

    /**
     * Sets the text for multiple columns in the table.
     *
     * @param strings the array of new strings
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the text is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setText(String [] strings)
    {
        checkWidget();
        if (strings == null)
        {
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        }

        OS.TableItem_SetText(handle, strings);
    }

    /**
     * Sets the receiver's text at a column
     *
     * @param index the column index
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
    public void setText(int index, String string)
    {
        checkWidget();
        if (string == null)
        {
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        }
        if (internal_checkValidIndex(index))
        {
            OS.TableItem_SetText(handle, string, index);
        }
    }

//From Item
    public void setText(String string)
    {
        setText(0, string);
    }


    protected void internal_prepareForRelease()
    {
        int index = parent.indexOf(this);
        parent.internal_removeItem(index);
        super.internal_prepareForRelease();
    }

    protected void internal_releaseResources()
    {
        images.removeAllElements();
        images = null;

        for (int i = 0; i < cellBackgrounds.size(); ++i)
        {
            if (cellBackgrounds.elementAt(i) != null &&
                    !((Color)cellBackgrounds.elementAt(i)).isDisposed())
            {
                ((Color)cellBackgrounds.elementAt(i)).dispose();
            }
        }
        for (int i = 0; i < cellForegrounds.size(); ++i)
        {
            if (cellForegrounds.elementAt(i) != null &&
                    !((Color)cellForegrounds.elementAt(i)).isDisposed())
            {
                ((Color)cellForegrounds.elementAt(i)).dispose();
            }
        }
        cellBackgrounds.removeAllElements();
        cellBackgrounds = null;
        cellForegrounds.removeAllElements();
        cellForegrounds = null;
        if (background != null && !background.isDisposed())
        {
            background.dispose();
        }
        background = null;
        if (foreground != null && !foreground.isDisposed())
        {
            foreground.dispose();
        }
        foreground = null;

        parent.tableItems.removeElement(this);

        super.internal_releaseResources();
    }

    protected void internal_releaseHandle()
    {
        if (handle != 0)
        {
            OS.TableItem_Dispose(handle);
            handle = 0;
        }
        super.internal_releaseHandle();
    }

    int internal_getHandle()
    {
        return this.handle;
    }

    private boolean internal_checkValidIndex(int index)
    {
        // Return false if index is invalid
        int count = Math.max(1, parent.getColumnCount());
        if (0 > index || index > count - 1)
        {
            return false;
        }

        return true;
    }

    void columnAdded(int index)
    {
        // The vectors always contain at least one slot
        if (parent.getColumnCount() > 1)
        {
            images.insertElementAt(null, index);
            cellForegrounds.insertElementAt(null, index);
            cellBackgrounds.insertElementAt(null, index);
        }
        else
        {
            images.setElementAt(null, 0);
        }
    }

    void columnRemoved(int index)
    {
        // Always keep at least one slot in the vectors
        if (images.size() != 1)
        {
            images.removeElementAt(index);
            if (cellForegrounds.elementAt(index) != null &&
                    !((Color)cellForegrounds.elementAt(index)).isDisposed())
            {
                ((Color)cellForegrounds.elementAt(index)).dispose();
            }
            cellForegrounds.removeElementAt(index);
            if (cellBackgrounds.elementAt(index) != null &&
                    !((Color)cellBackgrounds.elementAt(index)).isDisposed())
            {
                ((Color)cellBackgrounds.elementAt(index)).dispose();
            }
            cellBackgrounds.removeElementAt(index);
        }
        else
        {
            images.setElementAt(null, 0);
        }
    }

    int imageVectorSize()
    {
        return Math.max(1, parent.getColumnCount());
    }

}
