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

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.*;

/**
 * Corresponds for correct layouting of ImageItems.
 */
class ImageItemLayouter extends ItemLayouter {

    /**
     * Constructor.
     *
     * @param dflp - DefaultFormLayoutPolicy used for layouting.
     */
    ImageItemLayouter(DefaultFormLayoutPolicy dflp) {
        super(dflp);
    }

    int eswtGetLabelAlignmentDirective() {
        return Item.LAYOUT_BOTTOM;
    }

    /**
     * Creates the eSWT Button or Label for this item.
     */
    Control eswtGetControl(Composite parent, Item item) {
        ImageItem imgItem = (ImageItem) item;
        if (item.getNumCommands() == 0) {
            Label label = new Label(parent, SWT.NONE);
            label.setImage(Image.getESWTImage(imgItem.getImage()));
            label.pack();
            return label;
        }
        else {
            Button button;
            switch (imgItem.getAppearanceMode()) {
                case ImageItem.BUTTON:
                    button = new Button(parent, SWT.PUSH);
                    break;
                case ImageItem.HYPERLINK:
                case ImageItem.PLAIN:
                default:
                    button = new Button(parent, SWT.FLAT);
                    break;
            }
            button.setImage(Image.getESWTImage(imgItem.getImage()));
            button.pack();
            return button;
        }
    }

    /**
     * Returns if this eSWT control is Layouter specific.
     */
    boolean eswtIsSpecificControl(Item item, Control control) {
        // No implementation needed
        return ((control instanceof Label) || (control instanceof Button)); // default
    }

    /**
     * Updates the values of ImageItem.
     */
    void eswtUpdateItem(Item item, Control control, int reason, Object param) {
        // No implementation needed
    }

    /**
     * Returns the minimum area needed to display an ImageItem.
     *
     * @param imageItem ImageItem object.
     * @return Minimum area needed to display ImageItem.
     */
    static Point calculateMinimumBounds(ImageItem imageItem) {
        Display disp = Display.getDisplay();
        final Point minSize = new Point(
                disp.getBestImageWidth(Display.CHOICE_GROUP_ELEMENT),
                disp.getBestImageHeight(Display.CHOICE_GROUP_ELEMENT));

        Image image = imageItem.getImage();
        if (image != null) {
            int actualImageWidth = image.getWidth();
            if ((actualImageWidth < minSize.x) || (minSize.x == 0)) {
                minSize.x = actualImageWidth;
            }
            int actualImageHeight = image.getHeight();
            if ((actualImageHeight < minSize.y) || (minSize.y == 0)) {
                minSize.y = actualImageHeight;
            }
        }
        applyMinMargins(imageItem, minSize);
        return minSize;
    }

    /**
     * Returns the preferred area needed to display an Item.
     *
     * @param item Item.
     * @return Preferred area needed to display Item. x is width and y is
     *         height.
     */
    static Point calculatePreferredBounds(final ImageItem imageItem) {
        final Point prefSize = new Point(0, 0);
        Image image = imageItem.getImage();
        if (image != null) {
            prefSize.x = image.getWidth();
            prefSize.y = image.getHeight();
        }
        applyPrefMargins(imageItem, prefSize);
        return prefSize;
    }

}
