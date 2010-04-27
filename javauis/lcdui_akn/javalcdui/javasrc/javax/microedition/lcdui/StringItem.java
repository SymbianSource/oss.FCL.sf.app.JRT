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

public class StringItem extends Item
{
    private String iText;
    private Font iFont;

    public StringItem(String aLabel,String aText)
    {
        this(aLabel,aText,PLAIN);
    }

    public StringItem(String aLabel,String aText,int aAppearanceMode)
    {
        if (aAppearanceMode < PLAIN || aAppearanceMode > BUTTON)
            throw new IllegalArgumentException();
        //
        synchronized (iToolkit)
        {
            final int toolkit  = iToolkit.getHandle();
            final String label = aLabel == null?"":aLabel;
            final String text  = aText == null?"":aText;
            iHandle = Toolkit.checkHandle(_create(toolkit,this,label,text,aAppearanceMode));
        }
        iLabel = aLabel;
        iText = aText;
        iAppearance = aAppearanceMode;
    }

    public String getText()
    {
        synchronized (iToolkit)
        {
            return iText;
        }
    }

    public void setText(String aText)
    {
        synchronized (iToolkit)
        {
            NativeError.check(_setText(getHandle(),iToolkit.getHandle(),aText == null?"":aText));
            iText = aText;
            RefreshForm();
        }
    }

    public int getAppearanceMode()
    {
        return iAppearance;
    }

    public void setFont(Font aFont)
    {
        if (aFont == null || aFont.iIsFreeSizeFont == true)
        {
            aFont = Font.getDefaultFont();
        }
        synchronized (iToolkit)
        {
            NativeError.check(_setFont(getHandle(), iToolkit.getHandle(),
                                       aFont == null ? 0 : aFont.iHandle));
            iFont = aFont;
            UpdateForm();
        }
    }

    public Font getFont()
    {
        synchronized (iToolkit)
        {
            return iFont == null ? Font.getDefaultFont() : iFont;
        }
    }

    private native int _create(int aToolkit,StringItem aStringItem,String aLabel,String aText,int aAppearanceMode);
    private native int _setFont(int aStringItem,int aToolkit,int aFont);
    private native int _setText(int aStringItem,int aToolkit,String atext);
}
