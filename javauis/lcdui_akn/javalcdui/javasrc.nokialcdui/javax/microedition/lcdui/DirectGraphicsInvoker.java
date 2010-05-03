/*
* Copyright (c) 2000-2007 Nokia Corporation and/or its subsidiary(-ies).
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


package javax.microedition.lcdui;

import com.nokia.mid.ui.DirectGraphics;


final class DirectGraphicsInvoker
        extends com.nokia.mid.ui.DirectGraphicsInvoker
{
    static void createInvoker()
    {
        DirectGraphicsInvoker.setInvoker(
            new javax.microedition.lcdui.DirectGraphicsInvoker());
    }

    protected DirectGraphics directGraphics(Graphics aGraphics)
    {
        if (aGraphics.iDirectGraphics == null)
        {
            aGraphics.iDirectGraphics = new DirectGraphicsImpl(aGraphics);
        }

        return aGraphics.iDirectGraphics;
    }

    protected void setColor(DirectGraphics aDirectGraphics, int aColor,
                            Image aImage)
    {
        DirectGraphicsImpl dgi = (DirectGraphicsImpl)aDirectGraphics;
        dgi.setColor(aColor, aImage);
    }

    protected javax.microedition.lcdui.Display display()
    {
        Toolkit toolkit = Toolkit.getToolkit();
        Display display = toolkit.getDisplay();
        return display;
    }

}
