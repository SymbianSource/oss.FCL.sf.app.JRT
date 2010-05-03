/*******************************************************************************
 * Copyright (c) 2009, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 * 
 * Contributors:
 *     Nokia Corporation - initial implementation 
 *******************************************************************************/
package org.eclipse.swt.internal.qt;

import org.eclipse.swt.internal.qt.graphics.GraphicsContext;

public final class SharedGCWrapper {

    private static GraphicsContext gc;

    /**
     * Returns the shared GraphicsContext that can be used for drawing or null if
     * the shared GraphicContext is already being used and is unavailable. 
     */
    public static GraphicsContext getSharedGC() {
        if(gc == null || gc.isDisposed()) {
            gc = new GraphicsContext();
        } else if(gc.isActive()) {
            return null;
        }
        return gc;
    }
}
