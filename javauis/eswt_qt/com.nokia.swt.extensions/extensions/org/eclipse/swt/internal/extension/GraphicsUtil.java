/*******************************************************************************
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 * 
 * Contributors:
 *     Nokia Corporation - initial implementation 
 *******************************************************************************/

package org.eclipse.swt.internal.extension;

import org.eclipse.swt.graphics.Internal_GfxPackageSupport;
import org.eclipse.swt.widgets.Internal_PackageSupport;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.internal.qt.graphics.WindowSurface;

public final class GraphicsUtil {

    /**
     * Ends window surface session started with startExternalRendering() -call.
     * Must be called after the rendering of external renderer has finished.
     *
     * @param gc The gc instance where the rendering is targeted
     */
    public static void endExternalRendering(GC gc) {
        Internal_GfxPackageSupport.endExternalRendering(gc);          
    }	

    /**
     * Returns the WindowSurface instance that is the ultimate target of the
     * rendering for the given gc instance.
     *
     * @param gc The gc instance asossiated with the WindowSurface
     * @return The WindowSurface instance that is the target of given gc instance
     */
    public static WindowSurface getWindowSurface(GC gc) {
        return Internal_GfxPackageSupport.getWindowSurface(gc);
    }

    /**
     * Start a window surface rendering session on the surface that is the 
     * target of the given gc instance. This method must be called before 
     * rendering to the window surface from external rendering APIs like m3g and m2g. 
     * The caller must not render outside the area defined by the returned rectangle, 
     * as it may result in rendering on top of other controls.
     *
     * @param gc The gc instance where the rendering is targeted
     * @return The current clip of the gc in window surface coordinates or
     *         null if the target of given gc is not a Control
     */
    public static Rectangle startExternalRendering(GC gc) {
        return Internal_GfxPackageSupport.startExternalRendering(gc);          
    }	
    
    /**
     * Converts the given rectangle to window surface coordinates. 
     *
     * @param c The Control instance on which coordinates the conversion is done 
     * @param rect The rectangle to be converted
     * @return Rectangcular area in window surface coordinates
     */
    public static Rectangle toWindowSurface(Control c, Rectangle rect) {
        return Internal_PackageSupport.toWindowSurface(c, rect);
    }
}