/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Invoker for DirectGraphics.
*
*/



package com.nokia.mid.ui;

import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;

public abstract class DirectGraphicsInvoker
{
    private static com.nokia.mid.ui.DirectGraphicsInvoker sInvoker;

    public static void setInvoker(com.nokia.mid.ui.DirectGraphicsInvoker aInvoker)
    {
        sInvoker = aInvoker;
    }

    public static com.nokia.mid.ui.DirectGraphics getDirectGraphics(Graphics aGraphics)
    {
        return sInvoker.directGraphics(aGraphics);
    }

    protected abstract com.nokia.mid.ui.DirectGraphics directGraphics(Graphics aGraphics);

    protected abstract void setColor(DirectGraphics aDirectGraphics,
                                     int aColor, Image aImage);

    static void setColorValues(DirectGraphics aDirectGraphics, int aColor,
                               Image aImage)
    {
        sInvoker.setColor(aDirectGraphics, aColor, aImage);
    }

    protected abstract javax.microedition.lcdui.Display display();

    static javax.microedition.lcdui.Display getDisplay()
    {
        return sInvoker.display();
    }

}
