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

import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.internal.qt.graphics.GraphicsContext;
import org.eclipse.swt.internal.qt.graphics.WindowSurface;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Internal_PackageSupport;
import org.eclipse.swt.widgets.Widget;

final class CustomItemBufferSymbian extends Buffer
{
    private CustomItem host;
    private WindowSurface surface;

    CustomItemBufferSymbian(CustomItem customItem, Control ctrl)
    {
        super();
        setControlBounds(ctrl);
        host = customItem;
    }

    protected void init() 
    {
    	super.init();
    	surface = Internal_PackageSupport.getWindowsurface(host.layouter.formLayouter.getForm().getContentComp().getShell());
    }
    
    int getHostType()
    {
        return HOST_TYPE_CUSTOMITEM;
    }

    Object getHost()
    {
        return host;
    }

    void bindToHost(GraphicsContext gc)
    {
        gc.bindTarget(surface);
    }

    void blit(GraphicsContext gc, Widget widget) {
		surface.flush();
	}

    void endPaint() {
		surface.endPaint();
	}

    void beginPaint(int x, int y, int w, int h) {
		Rectangle rect = toWindowCoordinates(x, y, w, h);
		surface.beginPaint(rect.x, rect.y, rect.width, rect.height);
	}

	boolean isPaintingActive() {
		return false;
	}
	
	WindowSurface getWindowSurface()
    {
        return surface;
    }
}
