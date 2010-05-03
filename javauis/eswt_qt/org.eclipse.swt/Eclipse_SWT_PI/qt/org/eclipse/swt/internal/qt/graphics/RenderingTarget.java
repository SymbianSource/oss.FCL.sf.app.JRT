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

/**
 * 
 * Wrapper for rendering target related data.
 * 
 * @author sampkaar
 *
 */
final class RenderingTarget {
    
    /**
     * Native handle of rendering target
     */
    int handle;
    
    /**
     * The type of rendering target, 
     * one of types defined in GraphicsContext
     */
    int type;
    
    /**
     * The target
     */
    java.lang.Object target;
       
    /**
     * Command buffer 
     */
    JavaCommandBuffer commandBuffer;
    
    /**
     * Flag for indicating if the current target is JavaCommandBuffer
     */
    boolean isJavaCommandBuffer;
    
    /**
     * Constructor
     */
    RenderingTarget() {
        reset();
    }
    
    /**
     * Resets all stored fields and data
     */
    void reset() {
        handle = 0;
        type = GraphicsContext.TARGET_NONE;
        target = null;
        commandBuffer = null;
        isJavaCommandBuffer = false;
    }

}
