/*
* Copyright (c) 1999 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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

import com.nokia.mj.impl.rt.legacy.NativeError;

public class ImageItem extends Item
{
    private Image  iImage;
    private String iAltText;

    public ImageItem(String aLabel,Image aImage,int aLayout,String aAltText)
    {
        this(aLabel,aImage,aLayout,aAltText,PLAIN);
    }

    public ImageItem(String aLabel,Image aImage,int aLayout,String aAltText,int aAppearanceMode)
    {
        if (aAppearanceMode != PLAIN && aAppearanceMode != HYPERLINK && aAppearanceMode != BUTTON)
        {
            throw new IllegalArgumentException();
        }
        checkLayout(aLayout);

        iImage = aImage;
        iAppearance = aAppearanceMode;
        iLayout  = aLayout;
        iAltText = aAltText;
        iLabel   = aLabel;

        //
        // Create a snapshot in the case of mutable images. Don't need to keep
        // snapshot reference as native layer must copy or inc refcount
        //
        Image snapShot = aImage;
        if (null != aImage && aImage.isMutable())
        {
            snapShot = Image.createImage(aImage);
        }

        synchronized (iToolkit)
        {
            final String label = aLabel == null?"":aLabel;
            final int    image = snapShot == null ? 0 : snapShot.getHandle(false);
            iHandle = Toolkit.checkHandle(_create(iToolkit.getHandle(),this, label, image, aLayout, aAppearanceMode, aAltText));
        }
    }

    public Image getImage()
    {
        return iImage;
    }

    public void setImage(Image aImage)
    {
        //
        // Create a snapshot in the case of mutable images. Don't need to keep
        // snapshot reference as native layer must copy or inc refcount
        //
        Image snapShot = aImage;
        if (null != aImage && aImage.isMutable())
        {
            snapShot = Image.createImage(aImage);
        }

        synchronized (iToolkit)
        {
            final int image = (null != snapShot ? snapShot.getHandle(false) : 0);

            NativeError.check(_setImage(getHandle(), iToolkit.getHandle(), image));
            iImage = aImage;

            RefreshForm();
        }
    }

    public String getAltText()
    {
        return iAltText;
    }

    public void setAltText(String aAltText)
    {
        synchronized (iToolkit)
        {
            NativeError.check(_setAltText(getHandle(),iToolkit.getHandle(),aAltText==null?"":aAltText));
            iAltText = aAltText;
            RefreshForm();
        }
    }

    public int getAppearanceMode()
    {
        return iAppearance;
    }

    private native int _create(int aToolkit,ImageItem aImageItem,String aLabel,int aImage,int aLayout,int aAppearanceMode,String aAltText);
    private native int _setImage(int aImageItem,int aToolkit,int aImage);
    private native int _setAltText(int aImageItem,int aToolkit,String aAltText);
}
