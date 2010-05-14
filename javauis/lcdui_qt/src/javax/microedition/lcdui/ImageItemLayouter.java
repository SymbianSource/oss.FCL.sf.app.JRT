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
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.*;
import org.eclipse.ercp.swt.mobile.CaptionedControl;

/**
 * Corresponds for correct layouting of ImageItems.
 */
class ImageItemLayouter extends ItemLayouter
{

    /**
     * Constructor.
     *
     * @param dflp - DefaultFormLayoutPolicy used for layouting.
     */
    ImageItemLayouter(DefaultFormLayoutPolicy dflp)
    {
        super(dflp);
    }

    int eswtGetLabelAlignmentDirective()
    {
        return Item.LAYOUT_BOTTOM;
    }

    /**
     * Layout Item in a row.
     *
     * @param row current Row
     * @param item Item to layout
     */
    void eswtLayoutItem(Row row, Item item)
    {
        LayoutObject lo = new LayoutObject(item, eswtGetControl(formComposite, item));
        dfi.eswtAddNewLayoutObject(lo);
    }

    /**
     * Creates the eSWT CaptionedControl or Composite for this item.
     */
    Control eswtGetControl(Composite parent, Item item)
    {
        Control ret = eswtCreateControl(parent, item);
        Point size = new Point(0,0);
        size.x = item.getLayoutWidth();
        size.y = item.getLayoutHeight();
        eswtCaptionedResize(item, ret, size.x, size.y);
        return ret;
    }

    /**
     * Creates the eSWT CaptionedControl or Composite for this item.
     */
    static Control eswtCreateControl(Composite parent, Item item)
    {
        if(item.hasLabel())
        {
            CaptionedControl captioned = new CaptionedControl(parent, SWT.VERTICAL);
            captioned.setText(item.getLabel());
            eswtSpecificControl(captioned, item);
            return captioned;
        }
        else
        {
            Control ret = eswtSpecificControl(parent, item);
            return ret;
        }
    }

    /**
     * Creates the eSWT specific control for this item.
     */
    static Control eswtSpecificControl(Composite parent, Item item)
    {
        ImageItem imgItem = (ImageItem) item;

        if(item.getNumCommands() == 0)
        {
            Label imageLabel = new Label(parent, SWT.CENTER);
            imageLabel.setImage(Image.getESWTImage(imgItem.getImage()));
            imageLabel.pack();
            int imageWidth = imageLabel.getBounds().width;
            int imageHeight = imageLabel.getBounds().height;
            int maxWidth = getMaximumItemWidth(item);
            if(imageWidth > maxWidth)
            {
                Point size = imageLabel.computeSize(maxWidth, SWT.DEFAULT);
                //Image Resize has to be done.
                imageLabel.setSize(size.x, size.y);
                imageWidth = size.x;
                imageHeight = size.y;
            }

            if(imgItem.hasLabel())
            {
                int labelWidth = Font.getDefaultFont().stringWidth(imgItem.getLabel());

                if(labelWidth > imageWidth)
                {
                    Point size = parent.computeSize(Math.min(labelWidth, maxWidth), SWT.DEFAULT);
                    parent.setSize(size.x, size.y);
                }
                else
                {
                    parent.pack();
                }
            }

            return imageLabel;
        }
        else
        {
            Button button;

            switch(imgItem.getAppearanceMode())
            {
            case ImageItem.BUTTON:
                button = new Button(parent, SWT.PUSH | SWT.NONE);
                break;
            case ImageItem.HYPERLINK:
            case ImageItem.PLAIN:
            default:
                button = new Button(parent, SWT.FLAT | SWT.NONE);
                break;
            }
            button.setImage(Image.getESWTImage(imgItem.getImage()));
            button.pack();
            int buttonWidth = button.getBounds().width;
            int buttonHeight = button.getBounds().height;
            int maxWidth = getMaximumItemWidth(item);
            if(buttonWidth > maxWidth)
            {
                Point size = button.computeSize(maxWidth, SWT.DEFAULT);
                //Image Resize has to be done.
                button.setSize(size.x, size.y);
                buttonWidth = size.x;
                buttonHeight = size.y;
            }

            if(imgItem.hasLabel())
            {
                int labelWidth = Font.getDefaultFont().stringWidth(imgItem.getLabel());

                if(labelWidth > buttonWidth)
                {
                    Point size = parent.computeSize(Math.min(labelWidth, maxWidth), SWT.DEFAULT);
                    parent.setSize(size.x, size.y);
                }
                else
                {
                    parent.pack();
                }
            }

            return button;
        }
    }

    /**
     * Returns if this eSWT control is Layouter specific.
     */
    boolean eswtIsSpecificControl(Item item, Control control)
    {
        // No implementation needed
        return ((control instanceof Label) || (control instanceof Button));
    }

    /**
     * Updates the values of ImageItem.
     */
    void eswtUpdateItem(Item item, Control control, int reason, Object param)
    {
        // No implementation needed
    }

    /**
     * Returns the minimum area needed to display an ImageItem.
     *
     * @param imageItem ImageItem object.
     * @return Minimum area needed to display ImageItem.
     */
    static Point calculateMinimumBounds(final ImageItem imageItem)
    {
        final Point minSize = new Point(0, 0);
        ESWTUIThreadRunner.syncExec(new Runnable()
        {
            public void run()
            {
                Point size = null;
                if(imageItem.getNumCommands() > 0)
                {
                    Button button = new Button(eswtGetStaticShell(), SWT.PUSH);
                    button.setImage(Image.getESWTImage(imageItem.getImage()));
                    button.pack();
                    int buttonWidth = button.getBounds().width;
                    int maxWidth = getMaximumItemWidth(imageItem);
                    if(buttonWidth > maxWidth)
                    {
                        size = button.computeSize(maxWidth, SWT.DEFAULT);
                    }
                    else
                    {
                        size = button.computeSize(SWT.DEFAULT, SWT.DEFAULT);
                    }
                    button.dispose();
                }
                else
                {
                    Label imageLabel = new Label(eswtGetStaticShell(), SWT.NONE);
                    imageLabel.setImage(Image.getESWTImage(imageItem.getImage()));
                    imageLabel.pack();
                    int imageWidth = imageLabel.getBounds().width;
                    int maxWidth = getMaximumItemWidth(imageItem);
                    if(imageWidth > maxWidth)
                    {
                        size = imageLabel.computeSize(maxWidth, SWT.DEFAULT);
                    }
                    else
                    {
                        size = imageLabel.computeSize(SWT.DEFAULT, SWT.DEFAULT);
                    }
                    imageLabel.dispose();
                }
                minSize.x = size.x;
                minSize.y = size.y;
                applyMinMargins(imageItem, minSize);
            }
        });
        return minSize;
    }

    /**
     * Returns the preferred area needed to display an Item.
     *
     * @param item Item.
     * @return Preferred area needed to display Item. x is width and y is
     *         height.
     */
    static Point calculatePreferredBounds(final ImageItem imageItem)
    {
        final Point prefSize = new Point(0, 0);
        ESWTUIThreadRunner.syncExec(new Runnable()
        {
            public void run()
            {
                Control ctrl = eswtCreateControl(eswtGetStaticShell(), imageItem);
                Point size = ctrl.getSize();
                ctrl.dispose();
                prefSize.x = size.x;
                prefSize.y = size.y;
            }
        });
        return prefSize;
    }

}
