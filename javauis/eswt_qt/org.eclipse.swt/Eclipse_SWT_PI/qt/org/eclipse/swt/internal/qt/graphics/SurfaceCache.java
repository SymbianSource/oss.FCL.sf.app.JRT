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

import java.util.Hashtable;
import java.util.Enumeration;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Listener; 
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.SWT;

/**
 * Class that caches created <code>WindowSurface </code> instances.
 * This implementation is not implemented as thread safe as it should
 * be used only in UI thread.
 */ 

final class SurfaceCache {
    
    /**
     * Table for cached items
     */
    private Hashtable surfaces = null;
    
    /**
     * State of SurfaceCache diposeListener
     */
    private boolean isDisposeListenerSet = false; 
    
    /**
     * Instance
     */
    private static SurfaceCache instance = null;
    
    /**
     * State
     */
    private static boolean disposed = false;
    
    /**
     * Gets singleton instance of this object
     */
    static SurfaceCache getInstance() {
        if (disposed) {
            return null;
        }
        if (instance == null) {
            instance = new SurfaceCache();   
        }
        return instance;
    }
 
    /**
     * Constructor
     */
    private SurfaceCache() {
        surfaces = new Hashtable();
    } 
 
    /**
     * Adds new window surface and target object pair to cache if the target does 
     * not already exist in cache. 
     * 
     * @param surface The window surface to add be added to the cache
     * @param target The key for the window surface being added
     * @param addListener True if the target to be added is a Shell and dispose listener for that entry is needed
     * for automatic cache entry cleanup, otherwise false, i.e. when adding window surface for Image
     * @throws NullPointerException if surface or target is null 
     * @throws IllegalStateException if Cache already disposed 
     */
    void addSurface(WindowSurface surface, Object target, boolean addListener) {
        if (disposed) {
            throw new IllegalStateException("Cache already disposed");
        }
        if (surface == null || target == null) {
            throw new NullPointerException("Parameter surface or target is null"); 
        }

        if (surfaces.containsKey(target)) {
            return;
        } else {
            surfaces.put(target, surface); 
            if (target instanceof org.eclipse.swt.widgets.Widget) {
                // When first widget is added to cache set dispose listener for whole 
                // cache disposal upon the Display 
                if (!isDisposeListenerSet) {
                    setDisposeListener(((org.eclipse.swt.widgets.Widget)target).getDisplay());
                }
                // set target listener if requested
                if (addListener) {
                    ((org.eclipse.swt.widgets.Widget)target).addDisposeListener(new SurfaceCacheItemDisposeListener());
                } 
            } 
        }
    }
    
    /**
     * Deletes window surface matching the given target if found in cache
     * otherwise request is ignored.
     *
     * @param target The key to delete in cache
     * @throws NullPointerException if target is null 
     * @throws IllegalStateException if Cache already disposed 
     */
    void deleteSurface(Object target) {
        if (disposed) {
            throw new IllegalStateException("Cache already disposed");
        }
        if (target == null) {
            throw new NullPointerException("Parameter target is null"); 
        }
        if (surfaces.containsKey(target)) {
            surfaces.remove(target);    
        } 
    }
    
    /**
     * Searches and returns a window surface matching given target.
     * The target can be any object bindable by GraphicsContext.
     *
     * @param target The key for entry to return
     * @return WindowSurface for given target or null if target is not found in cache
     * @throws NullPointerException if target is null 
     * @throws IllegalStateException if Cache already disposed 
     */
    WindowSurface getSurface(Object target) {
        if (disposed) {
            throw new IllegalStateException("Cache already disposed");
        }
        if (target == null) {
            throw new NullPointerException("Parameter target is null"); 
        }
        return (WindowSurface)surfaces.get(target);
    }
    
    /**
     * Gets the disposal status of the instance
     */
    boolean isDisposed() {
        return disposed;
    }
    
    /**
     * Disposes all stored window surfaces and emptys the cache completely
     * If the instance is already disposed the request is ignored
     */
    void dispose() {
        if (disposed) {
            return;
        }
        if (surfaces == null) {
            return;
        }
        if (surfaces.size() == 0) {
            return;
        }
        Enumeration keys = surfaces.keys();
        while (keys.hasMoreElements()) {
            Object target = keys.nextElement();
            ((WindowSurface)surfaces.get(target)).dispose();
            surfaces.remove(target);   
        }
        disposed = true;
        surfaces = null;
    }
        
    /**
     * Sets dispose listener for cache
     */
     private void setDisposeListener(Display display) {
           display.addListener(SWT.Dispose, 
                               new Listener() {
                                    public void handleEvent(Event e) {
                                        SurfaceCache.getInstance().dispose();        
                                }
                            });
           isDisposeListenerSet = true;
    }
}