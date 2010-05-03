/*******************************************************************************
 * Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 * 
 * Contributors:
 *     Nokia Corporation - initial implementation 
 *******************************************************************************/
package org.eclipse.swt.internal.qt.graphics;

import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Internal_PackageSupport;

public final class WindowSurface {
    
	/**
	 * Undefined type
	 */
	final static int UNKNOWN_TYPE  = 0;
	
	/**
	 * QImage
	 */
    final static int QT_IMAGE      = 1;
    
    /**
     * CFbsBitmap
     */
    final static int SYMBIAN_IMAGE = 2;
    
    /**
     * EGL surface 
     */
    final static int EGL_SURFACE   = 4;
     
    /**
     * Handle of native peer
     */  
    int handle;
    
    /**
     * Disposal status of native peer
     */
    private boolean disposed; 
    
    /**
     * Indicates if this window surface is being
     * currently painted
     */
    private boolean paintingActive;

    /**
     * Constructor for Shell
     * @param shell Top level Shell
     * @param autoRefresh If true the window surface is automatically kept up to date
     *                    in each call to native objects bind, otherwise refresh needs to be called manually.
     * @throws IllegalArgumentException if given shell is not window owning or top level shell  
     * @throws NullPointerException if shell is null
     */
    public WindowSurface(Shell shell, boolean autoRefresh) {
    	if(shell == null) {
    		throw new NullPointerException("shell is null");
    	}
    	if(shell.getParent() != null) {
    		throw new IllegalArgumentException("Given shell is not window owning");
    	}
        int widgetHandle = Internal_PackageSupport.handle(shell);
      	handle = OS.windowsurface_create(widgetHandle, autoRefresh);
    }
    
    /**
     * Constructor for native window surface
     * @param nativeHandle The handle of native widow surface instance
     */            
    WindowSurface(int nativeHandle) {
        handle = nativeHandle;
    }
    
    /**
     * Opens this window surface for painting. Must be called before painting to window surface
     * with any renderer
     * @param x The x-coordinated of painting area
     * @param y The y-coordinated of painting area
     * @param width The width of painting area
     * @param height The height of painting area
     */
    public void beginPaint(int x, int y, int width, int height) {
    	if(paintingActive) {
    		return;
    	}
    	checkState();
    	OS.windowsurface_beginPaint(handle, x, y, width, height);
    	paintingActive = true;
    }
    
    /**
     * Ends paint cycle, must be called when painting ends
     */
    public void endPaint() {
    	checkState();
    	OS.windowsurface_endPaint(handle);
    	paintingActive = false;	
    }
    
    /**
     * Flushes window surface to display
     */
    public void flush() {
    	checkState();
    	OS.windowsurface_flush(handle);
    }
    
    /**
     * Gets the type of window surface
     */ 
    public int getType() {
    	checkState();
        int type = OS.windowsurface_getType(handle);
        return type;
    } 
    
    /**
     * Gets the handle of the native peer
     */     
    public int getHandle() {
    	checkState();
        return handle;
    }
    
    /**
     * Disposes this instance
     */ 
    public void dispose() {
    	Utils.validateUiThread();
        if (handle != 0) {
            OS.windowsurface_dispose(handle);
            handle = 0;
        }
        disposed = true;
    }
    
    /**
     * Dispose status of this instance.
     * @return True if this instance has been disposed, othewise false
     */
    public boolean isDisposed() {
    	return disposed;
    }
    
    /**
     * Refreshes the contained Qt window surface information, which might change e.g. when 
     * application is sent to background. If auto refresh is on there's no additional need to call
     * this method.
     */
    public void refresh() 
    {
    	checkState();
    	OS.windowsurface_refresh(handle);
    }
    
    private void checkState() {
    	Utils.validateUiThread();
    	if (disposed) {
            throw new IllegalStateException("WindowSurface already disposed");
        }
    }
    
}