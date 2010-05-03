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
package javax.microedition.lcdui;

import org.eclipse.swt.graphics.Point;

/**
 * An item containing image. Image used will be copy of original image, so if
 * application modifies image after setting it to ImageItem, the Application
 * must call setImage()-method again if modifications need to appear in
 * ImageItem too.
 */
public class ImageItem extends Item {

    /**
     * Left here for source compatibility purposes. Use layout directives from
     * Item class instead.
     */
    public static final int LAYOUT_DEFAULT = Item.LAYOUT_DEFAULT;

    /**
     * Left here for source compatibility purposes. Use layout directives from
     * Item class instead.
     */
    public static final int LAYOUT_LEFT = Item.LAYOUT_LEFT;

    /**
     * Left here for source compatibility purposes. Use layout directives from
     * Item class instead.
     */
    public static final int LAYOUT_RIGHT = Item.LAYOUT_RIGHT;

    /**
     * Left here for source compatibility purposes. Use layout directives from
     * Item class instead.
     */
    public static final int LAYOUT_CENTER = Item.LAYOUT_CENTER;

    /**
     * Left here for source compatibility purposes. Use layout directives from
     * Item class instead.
     */
    public static final int LAYOUT_NEWLINE_BEFORE = Item.LAYOUT_NEWLINE_BEFORE;

    /**
     * Left here for source compatibility purposes. Use layout directives from
     * Item class instead.
     */
    public static final int LAYOUT_NEWLINE_AFTER = Item.LAYOUT_NEWLINE_AFTER;

    private Image image;
    private int appearanceMode;
    private String alternateText;

    /**
     * Constructor.
     *
     * @param label Item's label.
     * @param image Image associated with this ImageItem.
     * @param layout Combination of layout directives.
     * @param altText Alternate text which might be displayed if image cannot be
     *            displayed for some reason.
     * @throws IllegalArgumentException if Layout is invalid.
     */
    public ImageItem(String label, Image image, int layout, String altText) {
        this(label, image, layout, altText, Item.PLAIN);
    }

    /**
     * Constructor.
     *
     * @param label Item's label.
     * @param image Image associated with tihs ImageItem.
     * @param layout Combination of layout directives.
     * @param altText Alternate text which might be displayed if image cannot be
     *            displayed for some reason.
     * @param appMode Appearance mode.
     * @throws IllegalArgumentException if apperance mode is invalid.
     * @throws IllegalArgumentException if Layout is invalid.
     */
    public ImageItem(String label, Image image, int layout, String altText,
            int appMode) {

        if (appMode != Item.PLAIN && appMode != Item.BUTTON
                && appMode != Item.HYPERLINK) {
            throw new IllegalArgumentException(
                    MsgRepository.IMAGEITEM_EXCEPTION_INVALID_APPMODE);
        }

        setLabel(label);
        setImage(image);
        setLayout(layout);
        alternateText = altText;
        appearanceMode = appMode;
    }

    /**
     * Gets the image object associated with this ImageItem.
     *
     * @return Image-object.
     */
    public Image getImage() {
        return image;
    }

    /**
     * Sets the image.
     *
     * @param image Image to be set.
     */
    public void setImage(Image image) {
        this.image = image;
        updateParent(UPDATE_SIZE_CHANGED);
    }

    /**
     * Gets alternate text.
     *
     * @return Alternate text or null if no alternate text defined.
     */
    public String getAltText() {
        return alternateText;
    }

    /**
     * Sets alternate text.
     *
     * @param newAltText Alternate text to be set.
     */
    public void setAltText(String newAltText) {
        alternateText = newAltText;
        updateParent(UPDATE_SIZE_CHANGED);
    }

    /**
     * Gets appearance mode of this item. Modes are defined in Item-class.
     *
     * @return Appearance mode.
     */
    public int getAppearanceMode() {
        return appearanceMode;
    }

    /* (non-Javadoc)
     * @see javax.microedition.lcdui.Item#isFocusable()
     */
    boolean isFocusable() {
        return (getNumCommands() > 0);
    }

    /**
     * Calculates minimum size of this item.
     *
     * @return Minimum size.
     */
    Point calculateMinimumSize() {
        return ImageItemLayouter.calculateMinimumBounds(this);
    }

    /**
     * Calculates preferred size of this item.
     *
     * @return Preferred size.
     */
    Point calculatePreferredSize() {
        return ImageItemLayouter.calculatePreferredBounds(this);
    }

}
