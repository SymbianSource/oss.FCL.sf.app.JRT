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
* Description: Abstract class for accessing Canvas graphics item
*              implementation in javax.microedition.lcdui package.
*
*/

package com.nokia.mid.ui;

import com.nokia.mid.ui.CanvasGraphicsItem;
import javax.microedition.lcdui.Graphics;

public abstract class CanvasGraphicsItemPainter
{
    // Canvas graphics item
    protected CanvasGraphicsItem iItem;

    protected abstract int getHandle();

    protected abstract void Repaint(int aX, int aY, int aWidth, int aHeight);

    protected void Repaint(Graphics aGraphics)
    {
        iItem.Repaint(aGraphics);
    }
}