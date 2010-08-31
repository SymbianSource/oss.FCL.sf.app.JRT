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

import org.eclipse.swt.internal.extension.CompositeExtension;
import org.eclipse.swt.internal.qt.graphics.GraphicsContext;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Internal_PackageSupport;
import org.eclipse.swt.widgets.Widget;
import org.eclipse.swt.internal.qt.graphics.WindowSurface;


final class CanvasBufferLinux extends Buffer {

	private Canvas host;
	private WindowSurface surface;
	private boolean syncPaintCallback;
	private int paintX, paintY;
	private int paintW, paintH;
	
	CanvasBufferLinux(Canvas canvas, Control ctrl)
	{
        super();
        setControlBounds(ctrl);
        host = canvas;
	}
    
	void bindToHost(GraphicsContext gc) 
	{
		gc.bindTarget(surface);
	}

    void ensureWindowSurface() 
    {
    	if(surface == null)
    	{
    		surface = Internal_PackageSupport.getWindowsurface(host.getShell());
    	}
    }
	
	void beginPaint(int x, int y, int w, int h) 
	{
		ensureWindowSurface();
		paintX = x; 
		paintY = y;
		paintW = w;
		paintH = h;
		surface.beginPaint(paintX, paintY, paintW, paintH);
	}
	
	void endPaint() 
	{
	    // nothing to do	
	}

	void blit(GraphicsContext gc, Widget widget) 
	{
		// On linux platform this method is called in order to 
		// request new paint event and also from the actual paint event itself to blit the
		// frame to display.
		// The behavior of this method depends on the syncpaintCallback flag, which 
		// indicates on which above scenario we are in each time.
        
		// If we are within the paint callback
		// draw the window surface to display
		if(syncPaintCallback) 
		{
			gc.drawWindowSurface(surface, paintX, paintX, paintW, paintH);
		}
		else
		{	
			// We are requesting a paint event
			syncPaintCallback = true;
			((CompositeExtension)widget).redrawNow(paintX, paintY, paintW, paintH);
			syncPaintCallback = false;
			
			// Once the display has been updated, 
			// clear the invalidate area
			paintX = paintY = paintW = paintH = 0; 
		}
	}
	
	Object getHost() 
	{	
		return host;
	}

	int getHostType() 
	{
		return HOST_TYPE_CANVAS;
	}

	boolean isPaintingActive() {
		return syncPaintCallback;
	}
	
	WindowSurface getWindowSurface()
    {
        return surface;
    }
}
