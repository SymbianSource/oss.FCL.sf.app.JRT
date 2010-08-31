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

import org.eclipse.swt.internal.extension.CanvasExtension;
import org.eclipse.swt.internal.qt.graphics.GraphicsContext;
import org.eclipse.swt.internal.qt.graphics.WindowSurface;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Internal_PackageSupport;
import org.eclipse.swt.widgets.Widget;


final class CustomItemBufferLinux extends Buffer {

    private CustomItem host;
    private WindowSurface surface;
    private boolean syncPaintCallback;
	private int paintX, paintY;
	private int paintW, paintH;

    CustomItemBufferLinux(CustomItem customItem, Control ctrl)
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
    
    void bindToHost(GraphicsContext gc) {
		gc.bindTarget(surface);
	}

	void blit(GraphicsContext gc, Widget widget) {
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
			((CanvasExtension)widget).redrawNow(paintX, paintY, paintW, paintH);
			syncPaintCallback = false;
			
			// Once the display has been updated, 
			// clear the invalidate area
			paintX = paintY = paintW = paintH = 0; 
		}		
	}

	void endPaint() {
		// nothing to do
	}

	Object getHost() {
		return host;
	}

	int getHostType() {
		return Buffer.HOST_TYPE_CUSTOMITEM;
	}

	void beginPaint(int x, int y, int w, int h) {
		paintX = x; 
		paintY = y;
		paintW = w;
		paintH = h;
		surface.beginPaint(paintX, paintY, paintW, paintH);
	}

	boolean isPaintingActive() {
		return syncPaintCallback;
	}

    WindowSurface getWindowSurface()
    {
        return surface;
    }
}
