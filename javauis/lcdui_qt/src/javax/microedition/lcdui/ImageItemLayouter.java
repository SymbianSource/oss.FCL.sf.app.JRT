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
import org.eclipse.ercp.swt.mobile.CaptionedControl;

/**
 * Corresponds for correct layouting of ImageItems.
 */
class ImageItemLayouter extends ItemLayouter
{

    /**
     * Constructor.
     *
     * @param aFormLayouter FormLayouter used for layouting.
     */
    ImageItemLayouter(FormLayouter aFormLayouter)
    {
        super(aFormLayouter);
    }

    /**
     * Creates LayoutObject for the given Item.
     *
     * @param item Item to layout
     * @return LayoutObject
     */
    LayoutObject getLayoutObject(Item item)
    {
    	LayoutObject lo = formLayouter.getLayoutObject(item);
    	if(lo == null)
    	{
        	lo = new LayoutObject(item, eswtGetControl(formComposite, item));
    	}
		return lo;
    }	

    /**
     * Creates the eSWT CaptionedControl or Composite for this item.
     */
    Control eswtGetControl(Composite parent, Item item)
    {
        Control ret = eswtCreateControl(parent, item);
        eswtCaptionedResize(item, ret, item.getLayoutWidth(), item.getLayoutHeight());
        return ret;
    }

    /**
     * Creates the eSWT CaptionedControl or Composite for this item.
     */
    static Control eswtCreateControl(Composite parent, Item item)
    {
        CaptionedControl captioned = new CaptionedControl(parent, SWT.VERTICAL);
		if(item.hasLabel())
		{
        	captioned.setText(item.getLabel());
		}

		if(((ImageItem)item).getImage() != null)
		{
			eswtSpecificControl(captioned, item);
		}
		else
		{
			updateControlSize(null, captioned, (ImageItem)item);
		}

        return captioned;
    }

    /**
     * Creates the eSWT specific control for this item.
     */
    static Control eswtSpecificControl(Composite parent, Item item)
    {
        ImageItem imgItem = (ImageItem) item;

        if(imgItem.getNumCommands() == 0)
        {
            Label imageLabel = new Label(parent, SWT.CENTER);
			constructLabel(imageLabel, parent, imgItem);
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
			constructButton(button, parent, imgItem);
            return button;
        }
    }

	static private void constructLabel(Label imageLabel, Composite parent, ImageItem imgItem)
	{
        imageLabel.setImage(Image.getESWTImage(imgItem.getImage()));
        imageLabel.pack();
		updateControlSize(imageLabel, parent, imgItem);
	}

	static private void constructButton(Button button, Composite parent, ImageItem imgItem)
	{
		button.setImage(Image.getESWTImage(imgItem.getImage()));
		button.pack();
		updateControlSize(button, parent, imgItem);
	}

	static private void updateControlSize(Control control, Composite parent, ImageItem imgItem)
	{
		int controlWidth;
		int controlHeight;
		int maxWidth = getMaximumItemWidth(imgItem);

		if(control != null)
		{
			controlWidth = control.getBounds().width;
			controlHeight = control.getBounds().height;
			
			if(controlWidth > maxWidth)
			{
				//Image Resize has to be done.
				Point size = control.computeSize(maxWidth, SWT.DEFAULT);
				control.setSize(size.x, size.y);
				controlWidth = size.x;
				controlHeight = size.y;
			}
		}
		else
		{
			controlWidth = maxWidth;
		}
		
		if(imgItem.hasLabel())
		{
			int labelWidth = Font.getDefaultFont().stringWidth(imgItem.getLabel());
		
			if(labelWidth > controlWidth)
			{
				Point size = parent.computeSize(Math.min(labelWidth, maxWidth), SWT.DEFAULT);
				parent.setSize(size.x, size.y);
				return;
			}
		}
		else if(control == null)
		{
			parent.setSize(0, 0);
			return;
		}

		parent.pack();
	}

    /**
     * Returns if this eSWT control is Layouter specific.
     */
    boolean eswtIsSpecificControl(Item item, Control control)
    {
        return ((control instanceof Label) || (control instanceof Button));
    }

    /**
     * Updates the values of ImageItem.
     */
    void eswtUpdateItem(Item item, Control control, int aReason, Object param)
    {
   		if(!(control instanceof  CaptionedControl))
		{
			return;
		}
			
    	ImageItem imgItem = (ImageItem)item;
		int reason = aReason & Item.UPDATE_SIZE_MASK;

		switch(reason)
		{
		case Item.UPDATE_NONE:
		case ImageItem.UPDATE_ALTTEXT:
			break;

		case Item.UPDATE_LABEL:
		{
			String label = imgItem.getLabel();
			if(label == null)
			{
				label = "";
			}

			((CaptionedControl)control).setText(label);
			Control sCtrl = eswtFindSpecificControl(imgItem, control);
			updateControlSize(sCtrl, (Composite)control, imgItem);
			break;
		}

		case ImageItem.UPDATE_IMAGE:
		{
			Control sCtrl = eswtFindSpecificControl(imgItem, control);
			if(sCtrl != null)
			{
				if(imgItem.getImage() != null)
				{
					if(sCtrl instanceof Label)
					{
						constructLabel((Label)sCtrl, (Composite)control, imgItem);
					}
					else if(sCtrl instanceof Button)
					{
						constructButton((Button)sCtrl, (Composite)control, imgItem);
					}
				}
				else
				{
					sCtrl.dispose();
					updateControlSize(null, (Composite)control, imgItem);						
				}
			}
			else
			{
				if(imgItem.getImage() != null)
				{
					eswtSpecificControl((Composite)control, imgItem);
				}
			}
			break;
		}

		case Item.UPDATE_ADDCOMMAND:
		{
			Control sCtrl = eswtFindSpecificControl(imgItem, control);
			if(sCtrl != null)
			{
				if((sCtrl instanceof Label) && (imgItem.getNumCommands()==1))
				{
					sCtrl.dispose();
					eswtSpecificControl((Composite)control, imgItem);
				}
			}
		}
		break;
		
		case Item.UPDATE_REMOVECOMMAND:
		{
			Control sCtrl = eswtFindSpecificControl(imgItem, control);
			if(sCtrl != null)
			{
				if((sCtrl instanceof Button) && (imgItem.getNumCommands()==0))
				{
					sCtrl.dispose();
					eswtSpecificControl((Composite)control, imgItem);
				}
			}
		}
		break;

		default:
		{
			break;
		}
		}
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
