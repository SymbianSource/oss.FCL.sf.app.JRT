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

import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Widget;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.internal.qt.graphics.GraphicsContext;
import org.eclipse.swt.widgets.Internal_PackageSupport;
import org.eclipse.swt.internal.qt.graphics.WindowSurface;

final class CanvasBufferSymbian extends Buffer
{
    private Canvas host;
    private WindowSurface surface;

    CanvasBufferSymbian(Canvas canvas, Control ctrl)
    {
        super();
        setControlBounds(ctrl);
        host = canvas;
    }
    
    void ensureWindowSurface() 
    {
    	if(surface == null)
    	{
    	    surface = Internal_PackageSupport.getWindowsurface(host.getShell());
    	}
    }
    
    void beginPaint(int x, int y, int w, int h) {
		ensureWindowSurface();
		Rectangle rect = toWindowCoordinates(x, y, w, h);
		surface.beginPaint(rect.x, rect.y, rect.width, rect.height);		
	}
	
    void endPaint() {
		surface.endPaint();
	}
	
	void blit(GraphicsContext gc, Widget widget)
	{
	    if( surface != null )
	    {
    		surface.flush();
    	}
	}
	
    int getHostType()
    {
        return HOST_TYPE_CANVAS;
    }

    Object getHost()
    {
        return host;
    }

    void bindToHost(GraphicsContext gc)
    {
    	gc.bindTarget(surface);
    }

	boolean isPaintingActive() {
		return false;
	}
	
    WindowSurface getWindowSurface()
    {
        return surface;
    }
}
