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
* Description:  Invoker for access to lcdui package.
*
*/

package com.nokia.mid.ui;

public abstract class CanvasGraphicsItemPainterInvoker
{
    private static com.nokia.mid.ui.CanvasGraphicsItemPainterInvoker sInvoker;

    public static void setInvoker(com.nokia.mid.ui.CanvasGraphicsItemPainterInvoker invoker)
    {
        sInvoker = invoker;
    }

    protected static com.nokia.mid.ui.CanvasGraphicsItemPainter createCanvasGraphicsItemPainter(
        CanvasGraphicsItem aItem,
        Object aToolkit,
        int aWidth,
        int aHeight)
    {
        return sInvoker.doCreateCanvasGraphicsItemPainter(
                   aItem, aToolkit, aWidth, aHeight);
    }

    protected abstract com.nokia.mid.ui.CanvasGraphicsItemPainter doCreateCanvasGraphicsItemPainter(
        CanvasGraphicsItem aItem,
        Object aToolkit,
        int aWidth,
        int aHeight);
}
