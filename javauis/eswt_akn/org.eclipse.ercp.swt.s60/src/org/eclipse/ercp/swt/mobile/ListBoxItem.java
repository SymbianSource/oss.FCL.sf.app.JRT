/*******************************************************************************
 * Copyright (c) 2004 IBM Corp.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Mark Rogalski (IBM Corp.) - initial API specification
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/

package org.eclipse.ercp.swt.mobile;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Image;

/**
 * Instances of this class represent an item in a ListBox widget. Heading and
 * detail icons, if provided, are displayed at the size provided, or stretched
 * to fit the ListBox styles. For consistancy, all heading icons should be the
 * same size and all detail icons should be the same size for any one list.
 * Elements of a ListBoxItem which are <code>null</code> are not displayed in
 * the ListBox layout.
 *
 * @see ListBox
 */
public class ListBoxItem
{
    String detailText;
    Image[] detailIcons;
    String headingText;
    Image[] headingIcons;

    /**
     * Constructs a new instance of this class.
     */

    public ListBoxItem()
    {
        detailText = "";
    }

    /**
     * Constructs a new instance of this class and initializes it using provided
     * data. Arguments may be <code>null</code> except for
     * <code>detailText</code>.
     *
     * @param detailText
     *            string providing detail text
     * @param detailIcon
     *            Image providing single detail icon
     * @param headingText
     *            detail string providing detail text
     * @param headingIcon
     *            Image providing single heading icon
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if detail text is null</li>
     *                </ul>
     */
    public ListBoxItem(String detailText, Image detailIcon, String headingText, Image headingIcon)
    {
        if (detailText == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        this.detailText = detailText;
        if (detailIcon != null)
        {
            if (detailIcon.isDisposed()) SWT.error(SWT.ERROR_INVALID_ARGUMENT);
            detailIcons = new Image[1];
            detailIcons[0] = detailIcon;
        }
        this.headingText = headingText;
        if (headingIcon != null)
        {
            if (headingIcon.isDisposed()) SWT.error(SWT.ERROR_INVALID_ARGUMENT);
            headingIcons = new Image[1];
            headingIcons[0] = headingIcon;
        }
    }

    /**
     * Gets the detail icons of the item. The return value may be <code>null</code>.
     *
     * @return array of detail icons
     */
    public Image[] getDetailIcons()
    {
        return detailIcons;
    }

    /**
     * Gets the detail text of the item.
     *
     * @return string providing detail text
     */
    public String getDetailText()
    {
        return detailText;
    }

    /**
     * Gets the heading icons for this item. The return value may be <code>null</code>.
     *
     * @return array of heading icons
     */
    public Image[] getHeadingIcons()
    {
        return headingIcons;
    }

    /**
     * Gets the heading text of the item. The return value may be <code>null</code>.
     *
     * @return string providing heading text
     */
    public String getHeadingText()
    {
        return headingText;
    }

    /**
     * Sets the detail icons of the item. If null is provided for either the
     * array or an individual element then no icon is displayed. Semantically,
     * the detail icons represent attributes of the item and are always small.
     * They may come and go or change frequently. Only one detail icon is
     * guaranteed to be displayed. Use of more than one detail icon is
     * implementation dependent.
     *
     * @param icons
     *            array providing detail icons
     */
    public void setDetailIcons(Image[] icons)
    {
        detailIcons = icons;
    }

    /**
     * Sets the detail text of the item.
     *
     * @param detailText
     *            string providing detail text
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if detail text is null</li>
     *                </ul>
     */
    public void setDetailText(String detailText)
    {
        if (detailText == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        this.detailText = detailText;
    }

    /**
     * Sets the heading icons for this item. If null is provided for either the
     * whole array or an individual array element no icon is displayed.
     * Semantically, the heading icon is the primary icon, representing the item
     * itself or a catagory. It is likely to be static and could be small or
     * large. Only one heading icon is guaranteed to be displayed. Use of more
     * than one heading icon is implementation dependent.
     *
     * @param icons
     *            array providing heading icons, or <code>null</code>.
     *
     */
    public void setHeadingIcons(Image[] icons)
    {
        headingIcons = icons;
    }

    /**
     * Sets the heading text of the item. If null is provided, no heading is
     * displayed.
     *
     * @param headingText
     *            string providing heading text
     *
     */
    public void setHeadingText(String headingText)
    {
        this.headingText = headingText;
    }

}

