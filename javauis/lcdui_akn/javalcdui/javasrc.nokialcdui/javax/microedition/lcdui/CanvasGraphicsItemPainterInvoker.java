/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Invoker for access to lcdui package.
*
*/

package javax.microedition.lcdui;

import com.nokia.mid.ui.CanvasGraphicsItem;

final class CanvasGraphicsItemPainterInvoker
        extends com.nokia.mid.ui.CanvasGraphicsItemPainterInvoker
{
    static void createInvoker()
    {
        CanvasGraphicsItemPainterInvoker.setInvoker(
            new javax.microedition.lcdui.CanvasGraphicsItemPainterInvoker());
    }

    protected com.nokia.mid.ui.CanvasGraphicsItemPainter doCreateCanvasGraphicsItemPainter(
        CanvasGraphicsItem aItem,
        Object aToolkit,
        int aWidth,
        int aHeight)
    {
        return new javax.microedition.lcdui.CanvasGraphicsItemPainter(
                   aItem, aToolkit, aWidth, aHeight);
    }
}
