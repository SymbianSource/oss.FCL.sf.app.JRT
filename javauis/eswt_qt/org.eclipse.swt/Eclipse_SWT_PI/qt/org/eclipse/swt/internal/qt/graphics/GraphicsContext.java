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
import java.util.Vector;

import org.eclipse.swt.widgets.Internal_PackageSupport;
import org.eclipse.swt.widgets.Widget;

public final class GraphicsContext {

    // These must be in sync with graphics.h

    // Rectangle index
    /**
     * Rectangle data array's <code>x</code> index
     * @see #getClip
     * @see #getTextBoundingBox
     */
    public final static int RECT_X = 0;

    /**
     * Rectangle data array's <code>y</code> index
     * @see #getClip
     * @see #getTextBoundingBox
     */
    public final static int RECT_Y = 1;

    /**
     * Rectangle data array's <code>width</code> index
     * @see #getClip
     * @see #getTextBoundingBox
     */
    public final static int RECT_WIDTH = 2;

    /**
     * Rectangle data array's <code>height</code> index
     * @see #getClip
     * @see #getTextBoundingBox
     */
    public final static int RECT_HEIGHT = 3;

    // Blending modes
    /**
     * Constant for Source blending mode. The destination's color and
     * alpha values are replaced with those of source.
     */
    public static final int BLENDING_MODE_SRC = 10;

    /**
     * Constant for Source over blending mode. The source is composed over destination.
     */
    public static final int BLENDING_MODE_SRC_OVER = 11;

    /**
     * Constant for XOR blending mode. The resulting color in the destination is
     * the exclusive or of the color values in the source and the destination.
     */
    public static final int BLENDING_MODE_XOR = 12;

    // Stroke styles
    /**
     * Stroke style is not set.
     */
    public static final int STROKE_NO = 0;

    /**
     * Solid stroke.
     */
    public static final int STROKE_SOLID = 1;

    /**
     * Dotted stroke
     */
    public static final int STROKE_DOT = 2;

    /**
     * Dash stroke
     */
    public static final int STROKE_DASH = 3;

    /**
     * Dash-dot stroke
     */
    public static final int STROKE_DASH_DOT = 4;

    /**
     * Dash-dot-dot stroke
     */
    public static final int STROKE_DASH_DOT_DOT = 5;

    // Text flags
    /**
     * One line. Whitespaces are threated as spaces.
     * NOTE: TEXT_SINGLE_LINE and TEXT_WORD_WRAP cannot be combined.
     * @see TEXT_WORD_WRAP
     */
    public static final int TEXT_SINGLE_LINE = 0x0100;

    /**
     * Prints outside of the given bounds if needed
     */
    public static final int TEXT_DONT_CLIP = 0x0200;

    /**
     * Expand tabs
     */
    public static final int TEXT_EXPAND_TABS = 0x0400;

    /**
     * Underline the mnemonic character
     */
    public static final int TEXT_SHOW_MNEMONIC = 0x0800;

    /**
     * Breaks lines at appropriate points
     * NOTE: TEXT_SINGLE_LINE and TEXT_WORD_WRAP cannot be combined.
     * @see TEXT_SINGLE_LINE
     */
    public static final int TEXT_WORD_WRAP = 0x1000;

    /**
     * Breaks lines anywhere, even within words.
     */
    public static final int TEXT_WRAP_ANYWHERE = 0x2000;

    /**
     * Same as OS_TEXT_SHOW_MNEMONIC but no underlines.
     * @see OS_TEXT_SHOW_MNEMONIC
     */
    public static final int TEXT_HIDE_MNEMONIC = 0x8000;

    /**
     * Behaves like a "hidden" text - not printed.
     */
    public static final int TEXT_DONT_PRINT = 0x4000;

    /**
     * Trailing whitespace not deleted automatically.
     */
    public static final int TEXT_INCLUDE_TRAILING_SPACES = 0x08000000;

    /**
     * Ensures that text lines are justified.
     */
    public static final int TEXT_JUSTIFICATIONFORCED = 0x10000;

    // Alignment flags
    /**
     * Aligns with the left edge.
     */
    public static final int ALIGNMENT_LEFT = 0x0001;

    /**
     * Aligns with the right edge.
     */
    public static final int ALIGNMENT_RIGHT = 0x0002;

    /**
     * Centers horizontally in the available space.
     */
    public static final int ALIGNMENT_HCENTER = 0x0004;

    /**
     * Justifies the text in the available space.
     */
    public static final int ALIGNMENT_JUSTIFY = 0x0008;

    /**
     * Aligns with the top.
     */
    public static final int ALIGNMENT_TOP = 0x0020;

    /**
     * Aligns with the bottom.
     */
    public static final int ALIGNMENT_BOTTOM =    0x0040;

    /**
     * Centers vertically in the available space.
     */
    public static final int ALIGNMENT_VCENTER = 0x0080;

    // package constants

    /**
     * Specifies no target
     */
    static final int TARGET_NONE = 0;

    /**
     * Specifies native target type QWidget.
     */
    static final int TARGET_WIDGET = 1;

    /**
     * Specifies native target type Image.
     */
    static final int TARGET_IMAGE = 2;

    /**
     * Specifies native target type Buffer.
     */
    static final int TARGET_NATIVE_COMMAND_BUFFER = 3;
    
    /**
     * Specifies Java command buffer target
     */
    static final int TARGET_WINDOWSURFACE = 4;
    /**
     * Specifies Java command buffer target, not valid in native target types
     */
   static final int TARGET_JAVA_COMMAND_BUFFER = 5;

    /**
     * Native peer handle
     */
    int handle = -1;


    /**
     * private members
     */
    private boolean disposed;
    private RenderingTarget targetData = null;

    /**
     * Constructor
     */
    public GraphicsContext() {
        Utils.validateUiThread();
        handle = OS.graphicsContext_init();
        targetData = new RenderingTarget();
        disposed = false;
    }

    /**
     * Gets the windowsurface of currently bound target.
     * Windowsurface is the surface where external graphics APIs can render, e.g m2g and m3g.
     * @return windowSurface if currently bound target has one, otherwise null
     */
    public WindowSurface getWindowSurface() {
        checkState();

        // buffers don't have a window surface
        
        if ((targetData.type == TARGET_NATIVE_COMMAND_BUFFER) ||(targetData.type == TARGET_JAVA_COMMAND_BUFFER) ) {
            return null;
        }

        // Get the current target that is bound
        Object target = targetData.target;

        // Only shells have window surface so figure out if target is shell,
        // and if not get the shell of child widget
        if (targetData.type == TARGET_WIDGET) {
            if (!(target instanceof org.eclipse.swt.widgets.Shell)) {
                target = ((org.eclipse.swt.widgets.Control)target).getShell();
            }
        }
        SurfaceCache cache = SurfaceCache.getInstance();

        // check surface cache for existing windowsurface for current target
        WindowSurface surface = cache.getSurface(target);

        // If not found in cache we need to create new one
        // and store that to cache for future use
        if (surface == null) {
            surface = new WindowSurface(OS.graphicsContext_get_windowsurface(handle));
            if (targetData.type == TARGET_WIDGET) {
                // Add to cache and let it add dispose listener for automatic cleanup
                cache.addSurface(surface, target, true);
            } else {
                // Image associated entry's are deleted when Image dispose is called 
                cache.addSurface(surface, target, false);
            }
            return surface;
        } else {
            return surface;
        }
    }

    void bindTarget(java.lang.Object target, int type, int handle, int bufferFlushTargetHandle) {
        Utils.validateUiThread();
        // validate state
        if(disposed) {
            throw new IllegalStateException("GraphicsContext already disposed");
        }
        if (isActive()) {
            throw new IllegalStateException("Already bound to target: " + targetData.target);
        }

        // Reset iTarget before setting new target
        targetData.reset();

        // call for actual bind, this may throw an exception
        // especially when trying to bind a target which has been bound already
        // this is skipped if target is java command buffer
        if (type != TARGET_JAVA_COMMAND_BUFFER) {
            doBind(handle, type, target, bufferFlushTargetHandle);
        } else {
            if(((JavaCommandBuffer)target).isBound()) {
                throw new IllegalStateException("Target already bound by other gc instance");
            }
            targetData.commandBuffer = (JavaCommandBuffer)target;
            targetData.isJavaCommandBuffer = true;
        }

        // store target
        targetData.handle = handle;
        targetData.type = type;
        targetData.target = target;

        // If target is Image, inform that it is being bound.
        // this is needed in case image is disposed before
        // releaseTarget() is called
        if(type == TARGET_IMAGE) {
            ((org.eclipse.swt.internal.qt.graphics.Image)target).notifyBind(this);
        }
    }

    /**
     * Binds given SWT Control as the rendering target of this GraphicsContext.
     *
     * @param target SWT Control
     *
     * @throws java.lang.IllegalStateException if this GraphicsContext already has a rendering target
     * @throws java.lang.IllegalStateException if given target is already bound by some other GC instance
     * @throws java.lang.IllegalStateException if GraphicsContext has already been disposed
     * @throws java.lang.NullPointerException if given target is null
     * @throws java.lang.IllegalArgumentException if given target cannot be bound
     * @throws java.lang.IllegalArgumentException if given target is already disposed
     */
    public void bindTarget(org.eclipse.swt.widgets.Control target) {
        if(target == null) {
        	throw new NullPointerException("target is null");
        }
        if(target.isDisposed())
            throw new IllegalArgumentException("Target already disposed");
        bindTarget(target, TARGET_WIDGET, handle(target), 0);
    }

    /**
     * Binds given SWT Display as the rendering target of this GraphicsContext.
     *
     * @param target SWT Display
     *
     * @throws java.lang.IllegalStateException if this GraphicsContext already has a rendering target
     * @throws java.lang.IllegalStateException if given target is already bound by some other GC instance
     * @throws java.lang.IllegalStateException if GraphicsContext has already been disposed
     * @throws java.lang.NullPointerException if given target is null
     * @throws java.lang.IllegalArgumentException if given target cannot be bound
     * @throws java.lang.IllegalArgumentException if given target is already disposed
     */
    public void bindTarget(org.eclipse.swt.widgets.Display target) {
        if(target == null) {
        	throw new NullPointerException("target is null");
        }
        if(target.isDisposed())
            throw new IllegalArgumentException("Target already disposed");
        int handle = org.eclipse.swt.internal.qt.OS.QApplication_desktop();
        bindTarget(target, TARGET_WIDGET, handle, 0);
    }

    /**
     * Binds given org.eclipse.swt.internal.qt.graphics.Image as the rendering target of this GraphicsContext.
     *
     * @param target The image
     *
     * @throws java.lang.IllegalStateException if this GraphicsContext already has a rendering target
     * @throws java.lang.IllegalStateException if given target is already bound by some other GC instance
     * @throws java.lang.IllegalStateException if GraphicsContext has already been disposed
     * @throws java.lang.NullPointerException if given target is null
     * @throws java.lang.IllegalArgumentException if given target cannot be bound
     * @throws java.lang.IllegalArgumentException if given target is already disposed
     */
    public void bindTarget(org.eclipse.swt.internal.qt.graphics.Image target) {
        if(target == null) {
        	throw new NullPointerException("target is null");
        }
        if(target.isDisposed())
            throw new IllegalArgumentException("Target already disposed");
        bindTarget(target, TARGET_IMAGE, target.handle, 0);
    }
    
    /**
     * Binds given org.eclipse.swt.internal.qt.graphics.Buffer as the rendering target of this GraphicsContext.
     *
     * @param target The buffer
     * @param flushTargetHandle The actual target where the buffer will be flushed to
     *
     * @throws java.lang.IllegalStateException if this GraphicsContext already has a rendering target
     * @throws java.lang.IllegalStateException if given target is already bound by some other GC instance
     * @throws java.lang.IllegalStateException if CraphicsContext has already been disposed
     * @throws java.lang.NullPointerException if given target is null
     * @throws java.lang.NullPointerException if given flushTarget is null
     * @throws java.lang.IllegalArgumentException if given target cannot be bound
     * @throws java.lang.IllegalArgumentException if given target is already disposed
     */
    public void bindTarget(org.eclipse.swt.internal.qt.graphics.NativeCommandBuffer target, org.eclipse.swt.widgets.Control flushTarget) {
        if(target == null) {
        	throw new NullPointerException("target is null");
        }
        if(flushTarget == null) {
        	throw new NullPointerException("flushTarget is null");
        }
        if(target.isDisposed())
            throw new IllegalArgumentException("Target already disposed");
        if(flushTarget.isDisposed())
            throw new IllegalArgumentException("Buffer flush target already disposed");
        bindTarget(target, TARGET_NATIVE_COMMAND_BUFFER, target.handle, handle(flushTarget));
    }

    
    /**
     * Binds given org.eclipse.swt.internal.qt.graphics.CommandBuffer as the rendering target of this GraphicsContext.
     * When JavaCommandBuffer is set as rendering target only one way methods, i.e. draw and setters are supported.
     * See JavaCommanduffer implementation for details of what operations are supported.
     *
     * @param target The buffer where commands are recorded
     * @throws java.lang.IllegalStateException if this GraphicsContext already has a rendering target
     * @throws java.lang.IllegalStateException if given target is already bound by some other GC instance
     * @throws java.lang.IllegalStateException if CraphicsContext has already been disposed
     * @throws java.lang.NullPointerException if given target is null
     */
    public void bindTarget(org.eclipse.swt.internal.qt.graphics.JavaCommandBuffer target) {
        if(target == null)
          throw new NullPointerException("target is null");
        bindTarget(target, TARGET_JAVA_COMMAND_BUFFER, 0, 0);
    }
    
    
    /**
     * Don't use. This is here only for testing purposes. 
     * @deprecated Buffer flush target must be given be given when binding to a buffer. 
     * @see bindTarget(org.eclipse.swt.internal.qt.graphics.Buffer target, org.eclipse.swt.widgets.Control flushTarget)
     */
    public void bindTarget(org.eclipse.swt.internal.qt.graphics.NativeCommandBuffer target) {
        if(target == null) {
        	throw new NullPointerException("Target is null");
        }
        if(target.isDisposed()) {
            throw new IllegalArgumentException("target already disposed");
        }
        bindTarget(target, TARGET_NATIVE_COMMAND_BUFFER, target.handle, 0);
    }
    
    /**
     * Binds given org.eclipse.swt.internal.qt.graphics.WindowSurface as the rendering target of this GraphicsContext.
     *
     * @param target The WindowSurface instance to be bound
     * @throws java.lang.IllegalStateException if this GraphicsContext already has a rendering target
     * @throws java.lang.IllegalStateException if given target is already bound by some other GC instance
     * @throws java.lang.IllegalStateException if CraphicsContext has already been disposed
     * @throws java.lang.NullPointerException if given target is null
     * @throws java.lang.IllegalArgumentException if given target is already disposed
     */
    public void bindTarget(org.eclipse.swt.internal.qt.graphics.WindowSurface target) {
        if(target == null) {
        	throw new NullPointerException("target is null");
        }
    	if(target.isDisposed()) {
            throw new IllegalArgumentException("target already disposed");
        }
    	bindTarget(target, TARGET_WINDOWSURFACE, target.handle, 0); 
    }
    
    /**
     * Get target to which current GraphicsContext is bound to
     *
     * @return Object to which GraphicsContext is bound to or null if there is no target
     */
    public Object getBoundTarget() {
        checkState();
        return targetData.target;
    }

    /**
     * Flushes all pending draw operations and releases current rending target.
     * If no target is bound, request is silently ingored.
     */
    public void releaseTarget() {
        Utils.validateUiThread();
        if (disposed) {
            throw new IllegalStateException("GraphicsContext already disposed");
        }
        
        if (targetData.target != null) {
            // In case of java command buffer no native calls needed
            if (targetData.type == TARGET_JAVA_COMMAND_BUFFER) {
                targetData.commandBuffer.release();
                targetData.reset();
                return;
            }
            // call native release
            OS.graphicsContext_releaseTarget(handle);

            // if target is Image notify about release
            if(targetData.type == TARGET_IMAGE) {
                ((org.eclipse.swt.internal.qt.graphics.Image)targetData.target).notifyRelease();
            }
            // reset target data
            targetData.reset();
        } else {
            //ignore, i.e. no need to throw an exception
        }
    }

    /**
     * Checks if target is already bound by some other gc instance and does
     * native bind operation.
     *
     * @param gc The GraphicsContext doing the binding
     * @throws IllegalStateException if target is found in the list before adding it,
     * i.e. it is already being painted by some other gc.
     */
    private void doBind(int targetHandle, int targetType, java.lang.Object target, int bufferFlushTargetHandle) {
        // call native bind. In case this fails the native peer throws an exception
        // and resets state automatically, i.e. there no need to rewind from here
        OS.graphicsContext_bindTarget(handle, targetHandle, targetType, bufferFlushTargetHandle);
    }

    /**
     * Renders all draw commands stored in given buffer to currently bound target.
     *
     * @param buffer The buffer to be processed
     * @throws NullPointerException If buffer is null
     * @throws IllegalArgumentException If buffer has been already disposed
     */
    public void render(NativeCommandBuffer buffer) {
        checkState();
        if(buffer == null) {
            throw new NullPointerException("buffer is null");
        }
        if(buffer.isDisposed()) {
            throw new IllegalArgumentException("buffer already disposed");
        }
        OS.graphicsContext_render(handle, buffer.handle);
    }

    /**
     * Renders all draw commands stored in given CommandBuffer to currently bound target.
     * Note that JavaCommandBuffer can be rendered only once as some of the large data objects placed in 
     * the buffer are freed right after they have been processed.
     *
     * @param buffer The CommandBuffer to be processed
     * @throws NullPointerException If CommandBuffer is null
     */
    public void render(JavaCommandBuffer buffer) {
        checkState();
        if(buffer == null) {
            throw new NullPointerException("buffer is null");
        }
        // if buffer does not contain data do nothing
        if(buffer.containsData()) {
            processBuffer(buffer);
        }
    }
    
    /**
     * Copies rectangular area from currently bound target to given image target.
     * The copy rectangle top left corned is defined by x and y and bottom right corner
     * by image width and height.
     *
     * @param target The image to copy into
     * @param x The x coordinate of top left corner copy area
     * @param y The y coordinate of top left corner copy area
     * @throws NullPointerException if Image target is null
     * @throws IllegalStateException if no target is bound
     */
    public void copyArea(Image target, int x, int y) {
        checkState();
        if (target == null) {
            throw new NullPointerException("Target image is null");
        }
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.copyArea(target, x, y);
        } else {
            OS.graphicsContext_copyArea(handle, target.handle, x, y);
        }
    }

    /**
     * Copies rectangular area within currently bound target from source
     * location to destination location.
     *
     * @param srcX The top left x coordinate of source rectangle
     * @param srcY The top left y coordinate of source rectangle
     * @param width The width of source rectangle
     * @param height The height of source rectangle
     * @param destX The top left x coordinate for the copy
     * @param destY The top left y coordinate for the copy
     * @throws IllegalStateException if no target is bound
     */
    public void copyArea(int srcX, int srcY, int width, int height, int destX, int destY) {
        copyArea(srcX, srcY, width, height, destX, destY, false);
    }
    
    /**
     * Copies rectangular area within currently bound target from source
     * location to destination location.
     *
     * @param srcX The top left x coordinate of source rectangle
     * @param srcY The top left y coordinate of source rectangle
     * @param width The width of source rectangle
     * @param height The height of source rectangle
     * @param destX The top left x coordinate for the copy
     * @param destY The top left y coordinate for the copy
     * @param paint If to generate paint events for the new and obscured areas
     * @throws IllegalStateException if no target is bound
     */
    public void copyArea(int srcX, int srcY, int width, int height, int destX, int destY, boolean paint) {
        checkState();

        if(width <= 0 || height <= 0) {
            return;
        }
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.copyArea(srcX, srcY, width, height, destX, destY, paint);
        } else {
            OS.graphicsContext_copyArea(handle, srcX, srcY, width, height, destX, destY, paint);
        }
    }

    /**
     * Draws outline of elliptical arc within specified rectangle using foreground color.
     * Arc is drawn counterclockwise.
     *
     * @param x The x coordinate of top left corner of arc rectangle
     * @param y The y coordinate of top left corner of arc rectangle
     * @param width The width of arc rectangle
     * @param height The height of arc rectangle
     * @param startAngle The starting angle for arc
     * @param arcAngle The span angle for arc
     * @throws IllegalStateException if no target is bound
     */
    public void drawArc(int x, int y, int width, int height, int startAngle, int arcAngle) {
        checkState();

        if(width <= 0 || height <= 0) {
            return;
        }
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.drawArc(x, y, width, height, startAngle, arcAngle);
        } else {
            OS.graphicsContext_drawArc(handle, x, y, width, height, startAngle, arcAngle);
        }
    }

    /**
     * Draws an ellipse specified by rectangle using foreground color.
     *
     * @param x The x coordinate of top left corner of ellipse rectangle
     * @param y The y coordinate of top left corner of ellipse rectangle
     * @param width The width of ellipse rectangle
     * @param height The height of ellipse rectangle
     * @throws IllegalStateException if no target is bound
     */
    public void drawEllipse(int x, int y, int width, int height) {
        checkState();

        if(width <= 0 || height <= 0) {
            return;
        }
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.drawEllipse(x, y, width, height);
        } else {
            OS.graphicsContext_drawEllipse(handle, x, y, width, height);
        }
    }

    public void drawFocus(int x, int y, int width, int height) {
        checkState();

        if(width <= 0 || height <= 0) {
            return;
        }
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.drawFocus(x, y, width, height);
        } else {
            OS.graphicsContext_drawFocus(handle, x, y, width, height);
        }
    }

    /**
     * Draws given image to specified location.
     *
     * @param img The image to draw
     * @param x The x coordinate of top left corner where to draw image
     * @param y The y coordinate of top left corner where to draw image
     * @throws NullPointerException if Image is null
     * @throws IllegalStateException if no target is bound
     */
    public void drawImage(Image img, int x, int y) {
        checkState();
        if (img == null) {
            throw new NullPointerException("Image is null");
        }
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.drawImage(img, x, y);
        } else {
            OS.graphicsContext_drawImage(handle, img.handle, x, y);
        }
    }

    /**
     * Draws given image to specified location.
     *
     * @param img The image to draw
     * @param tx The top-left x coordinate in image that is to be drawn onto
     * @param ty The top-left y coordinate in image that is to be drawn onto
     * @param tw The width of the image that is to be drawn onto.
     * @param th The heigth of the image that is to be drawn onto.
     * @param sx The top-left x coordinate in image that is to be drawn
     * @param sy The top-left y coordinate in image that is to be drawn
     * @param sw The width of the image that is to be drawn.
     * @param sh The heigth of the image that is to be drawn.
     * @throws NullPointerException if Image is null
     * @throws IllegalStateException if no target is bound
     */
    public void drawImage(Image img,
                        int tx, int ty, int tw, int th,
                        int sx, int sy, int sw, int sh) {
        checkState();
        if (img == null) {
            throw new NullPointerException("Image is null");
        }
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.drawImage(img, tx, ty, tw, th, sx, sy, sw, sh, Image.TRANS_NONE);
        } else {
            OS.graphicsContext_drawImage(handle, img.handle, tx, ty, tw, th, sx, sy, sw, sh, Image.TRANS_NONE);
        }
    }

    /**
     * Draws given image to specified location.
     *
     * @param img The image to draw
     * @param tx The top-left x coordinate in image that is to be drawn onto
     * @param ty The top-left y coordinate in image that is to be drawn onto
     * @param tw The width of the image that is to be drawn onto.
     * @param th The heigth of the image that is to be drawn onto.
     * @param sx The top-left x coordinate in image that is to be drawn
     * @param sy The top-left y coordinate in image that is to be drawn
     * @param sw The width of the image that is to be drawn.
     * @param sh The heigth of the image that is to be drawn.
     * @param manipulation transformation to be applied to region to be drawn, one of Image.TRANS_* definitions
     * @throws NullPointerException if Image is null
     * @throws IllegalArgumentException if manipulation is invalid
     * @throws IllegalStateException if no target is bound
     */
    public void drawImage(Image img,
                        int tx, int ty, int tw, int th,
                        int sx, int sy, int sw, int sh, int manipulation) {
        checkState();
        if (img == null) {
            throw new NullPointerException("Image is null");
        }
        if (!isManipulationValid(manipulation)) {
            throw new IllegalArgumentException("Invalid manipulation");
        }
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.drawImage(img, tx, ty, tw, th, sx, sy, sw, sh, manipulation);
        } else {
            OS.graphicsContext_drawImage(handle, img.handle, tx, ty, tw, th, sx, sy, sw, sh, manipulation);
        }
    }
    
    /**
     * Draws a line between defined points using foreground color.
     *
     * @param x1 The x coordinate of staring point
     * @param y1 The y coordinate of staring point
     * @param x2 The x coordinate of ending point
     * @param y2 The y coordinate of ending point
     * @throws IllegalStateException if no target is bound
     */
    public void drawLine(int x1, int y1, int x2, int y2) {
        checkState();
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.drawLine(x1, y1, x2, y2);
        } else {
            OS.graphicsContext_drawLine(handle, x1, y1, x2, y2);
        }
    }

    /**
     * Draws a point to specified location using foreground color.
     *
     * @param x The x coordinate to draw the point
     * @param y The y coordinate to draw the point
     * @throws IllegalStateException if no target is bound
     */
    public void drawPoint(int x, int y) {
        checkState();
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.drawPoint(x, y);
        } else {
            OS.graphicsContext_drawPoint(handle, x, y);
        }
    }

    /**
     * Draws a closed polygon which is defined integer array of coorinates, using foreground color.
     * Lines are drawn between each consecutive x,y pairs and between first and last point.
     *
     * @param pointArray An array alternating x and y values which are the verices of the polygon
     * @throws NullPointerException if <code>pointArray</code> is null
     * @throws IllegalStateException if no target is bound
     */
    public void drawPolygon(int[] pointArray)  {
        checkState();
        if (pointArray == null) {
            throw new NullPointerException("pointaArray is null");
        }
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.drawPolygon(pointArray);
        } else {
            OS.graphicsContext_drawPolygon(handle, pointArray);
        }
    }

    /**
     * Draws polyline which is defined integer array of coorinates, using foreground color.
     * Lines are drawn between each consecutive x,y pairs, but not between first and last coordinate.
     *
     * @param pointArray An array alternating x and y values which are the corners of the polyline
     * @throws NullPointerException if <code>pointArray</code> is null
     * @throws IllegalStateException if no target is bound
     */
    public void drawPolyline(int[] pointArray)  {
        checkState();
        if (pointArray == null) {
            throw new NullPointerException("pointaArray is null");
        }
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.drawPolyline(pointArray);
        } else {
            OS.graphicsContext_drawPolyline(handle, pointArray);
        }
    }

    /**
     * Draws outline rectangle specified by the arguments using foreground color.
     *
     * @param x The x coordinate of top left corner of rectangle
     * @param y The y coordinate of top left corner of rectangle
     * @param width The width of rectangle
     * @param height The height of rectangle
     * @throws IllegalStateException if no target is bound
     */
    public void drawRect(int x, int y, int width, int height) {
        checkState();

        if(width <= 0 || height <= 0) {
            return;
        }
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.drawRect(x, y, width, height);
        } else {
            OS.graphicsContext_drawRect(handle, x, y, width, height);
        }
    }

    /**
     * Draws a series of RGB+transparency values to specified region
     *
     * Scanlength can be negative which results in reading <code>rgbData</code> array in reverse direction
     *
     * @param rgbData an array or ARGB values in format #AARRGGBB
     * @param offset the array index of the first ARGB value
     * @param scanlenght the relative arra offset between consecutive rows in the <code>rgbData</code> array
     * @param x the horizontal location of the region to be rendered
     * @param y the vertical location of the region to be rendered
     * @param width the width of the region to be rendered
     * @param height the height of the region to be rendered
     * @param processAlpha <code>true</code> if <code>rgbData</code> has an alpha channel, <code>false</code> if all pixels are fully opaque
     * @param manipulation transformation to be applied, one of Image.TRANS_* definitions
     * @throws NullPointerException if <code>rgbData</code> is null
     * @throws IllegalArgumentException if manipulation is invalid
     * @throws ArrayIndexOutOfBoundsException if operation will attempt to access elements outside the range of <code>rgbData</code> array
     * @throws IllegalStateException if no target is bound
     */
    public void drawRGB(int[] rgbData, int offset, int scanlength, int x, int y, int width, int height, boolean processAlpha, int manipulation) {
        checkState();
        if(rgbData == null) {
            throw new NullPointerException("rgbData is null");
        }
        // check most obvious cases where we end up outside array bounds and abort
        // native peer will throw also exception if operation goes outside of array bounds
        if((offset < 0) || (rgbData.length < offset + width) ) {
            throw new ArrayIndexOutOfBoundsException();
        }
        if (!isManipulationValid(manipulation)) {
            throw new IllegalArgumentException("Invalid manipulation");
        } 
        if(scanlength == 0) {
            return;
        }
        if(width <= 0 || height <= 0) {
            return;
        }
        if(Math.abs(scanlength) < width) {
            return;
        }
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.drawRGB(rgbData, offset, scanlength, x, y, width, height, processAlpha, manipulation);
        } else {
            OS.graphicsContext_drawRGB(handle, rgbData, offset, scanlength, x, y, width, height, processAlpha, manipulation);
        }
    }

    /**
     * Draws a series of RGB+transparency values to specified region
     *
     * Scanlength can be negative which results in reading <code>rgbData</code> array in reverse direction
     *
     * @param rgbData an array or ARGB values in format #AARRGGBB
     * @param offset the array index of the first ARGB value
     * @param scanlenght the relative arra offset between consecutive rows in the <code>rgbData</code> array
     * @param x the horizontal location of the region to be rendered
     * @param y the vertical location of the region to be rendered
     * @param width the width of the region to be rendered
     * @param height the height of the region to be rendered
     * @param processAlpha <code>true</code> if <code>rgbData</code> has an alpha channel, <code>false</code> if all pixels are fully opaque
     * @throws NullPointerException if <code>rgbData</code> is null
     * @throws ArrayIndexOutOfBoundsException if operation will attempt to access elements outside the range of <code>rgbData</code> array
     * @throws IllegalStateException if no target is bound
     */
    public void drawRGB(int[] rgbData, int offset, int scanlength, int x, int y, int width, int height, boolean processAlpha) {
        checkState();
        drawRGB(rgbData, offset, scanlength, x, y, width, height, processAlpha, Image.TRANS_NONE);
    }

    /**
     * Draws a series of RGB+transparency values to specified region
     *
     * Scanlength can be negative which results in reading <code>rgbData</code> array in reverse direction
     *
     * @param rgbData an array or RGB values in format 1 bit per pixel
     * @param transparencyMask an array of mask values
     * @param offset the array index of the first ARGB value
     * @param scanlenght the relative arra offset between consecutive rows in the <code>rgbData</code> array
     * @param x the horizontal location of the region to be rendered
     * @param y the vertical location of the region to be rendered
     * @param width the width of the region to be rendered
     * @param height the height of the region to be rendered
     * @param manipulation transformation to be applied, one of Image.TRANS_* definitions
     * @throws NullPointerException if <code>rgbData</code> is null
     * @throws IllegalArgumentException if manipulation is invalid
     * @throws ArrayIndexOutOfBoundsException if operation will attempt to access elemens outside the range of <code>rgbData</code> array
     * @throws IllegalStateException if no target is bound
     */
    public void drawRGB(byte[] rgbData, byte[] transparencyMask,int offset, int scanlength, int x, int y, int width, int height, int manipulation, int format) {
        checkState();
        if(rgbData == null) {
            throw new NullPointerException("rgbData is null");
        }
        // check most obvious cases where we end up outside array bounds and abort
        // native peer will throw also exception if operation goes outside of array bounds
        if((offset < 0) || (rgbData.length < offset + width) ) {
            throw new ArrayIndexOutOfBoundsException();
        }
        if (!isManipulationValid(manipulation)) {
            throw new IllegalArgumentException("Invalid manipulation");
        }
        
        if(scanlength == 0) {
            return;
        }
        if(width <= 0 || height <= 0) {
            return;
        }
        if(Math.abs(scanlength) < width) {
            return;
        }
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.drawRGB(rgbData, transparencyMask, offset, scanlength, x, y, width, height, manipulation, format);
        } else {
            OS.graphicsContext_drawRGB(handle, rgbData, transparencyMask, offset, scanlength, x, y, width, height, manipulation, format);
        }
    }


    /**
     * Draws a series of RGB+transparency values to specified region
     *
     * Scanlength can be negative which results in reading <code>rgbData</code> array in reverse direction
     *
     * @param rgbData an array or ARGB values in format #AARRGGBB
     * @param offset the array index of the first ARGB value
     * @param scanlenght the relative arra offset between consecutive rows in the <code>rgbData</code> array
     * @param x the horizontal location of the region to be rendered
     * @param y the vertical location of the region to be rendered
     * @param width the width of the region to be rendered
     * @param height the height of the region to be rendered
     * @param processAlpha <code>true</code> if <code>rgbData</code> has an alpha channel, <code>false</code> if all pixels are fully opaque
     * @param manipulation transformation to be applied, one of Image.TRANS_* definitions
     * @throws NullPointerException if <code>rgbData</code> is null
     * @throws IllegalArgumentException if manipulation is invalid
     * @throws ArrayIndexOutOfBoundsException if operation will attempt to access elemens outside the range of <code>rgbData</code> array
     * @throws IllegalStateException if no target is bound
     */
    public void drawRGB(short[] rgbData, int offset, int scanlength, int x, int y, int width, int height, boolean processAlpha, int manipulation, int format) {
        checkState();
        if(rgbData == null) {
            throw new NullPointerException("rgbData is null");
        }
        // check most obvious cases where we end up outside array bounds and abort
        // native peer will throw also exception if operation goes outside of array bounds
        if((offset < 0) || (rgbData.length < offset + width) ) {
            throw new ArrayIndexOutOfBoundsException();
        }
        if (!isManipulationValid(manipulation)) {
            throw new IllegalArgumentException("Invalid manipulation");
        }
        if(scanlength == 0) {
            return;
        }
        if(width <= 0 || height <= 0) {
            return;
        }
        if(Math.abs(scanlength) < width) {
            return;
        }
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.drawRGB(rgbData, offset, scanlength, x, y, width, height, processAlpha, manipulation, format);
        } else {
            OS.graphicsContext_drawRGB(handle, rgbData, offset, scanlength, x, y, width, height, processAlpha, manipulation, format);
        }
    }
    /**
     * Draws outline round-cornered rectangle specified by the arguments using foreground color.
     *
     * @param x The x coordinate of top left corner of rectangle
     * @param y The y coordinate of top left corner of rectangle
     * @param width The width of rectangle
     * @param height The height of rectangle
     * @param arcWidth The width of the rectangle corner arc
     * @param arcHeight The heiht of the rectangle corner arc
     * @throws IllegalStateException if no target is bound
     */
    public void drawRoundRect(int x, int y, int width, int height, int arcWidth, int arcHeight) {
        checkState();
        if(width <= 0 || height <= 0) {
            return;
        }
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.drawRoundRect(x, y, width, height, arcWidth, arcHeight);
        } else {
            OS.graphicsContext_drawRoundRect(handle, x, y, width, height, arcWidth, arcHeight);
        }
    }

    /**
     * Draws the given string to specified location using the current font and foreground color.
     * Note: no tab expansion or carriage return processing will be performed.
     *
     * @param string The string to draw
     * @param x The x coordinate of top left corner of rectangular are where the string is drawn
     * @param y The y coordinate of top left corner of rectangular are where the string is drawn
     * @param sTransparent If true the background will be transparent, otherwise it will be opaque
     * @throws IllegalStateException if no target is bound
     */
    // TODO version for drawing bounding box (eSWT)
    public void drawString(String string, int x, int y, boolean isTransparent) {
        checkState();
        drawString(
            string,
            x,
            y,
            0,
            0,
            (GraphicsContext.ALIGNMENT_LEFT | GraphicsContext.ALIGNMENT_TOP),
            GraphicsContext.TEXT_SINGLE_LINE,
            isTransparent);
    }

    /**
     * Draws the given string to specified location using the current font and foreground color.
     * <code>alignments</code> and <code>flags</code> can be combined by
     * using the bit-wise or operation.
     *
     * @param string The string to draw
     * @param x The x coordinate of top left corner of rectangular are where the string is drawn
     * @param y The y coordinate of top left corner of rectangular are where the string is drawn
     * @param width The width of rectangular are where the string is drawn.
     * If 0 then the width is calculated based on the given <code>string</code>, <code>alignments</code> and <code>flags</code>.
     * @param height The height of rectangular are where the string is drawn.
     * If 0 then the height is calculated based on the given <code>text</code>, <code>alignments</code> and <code>flags</code>.
     * @param alingments Alingments:
     * <ul>
     * <li>ALIGNMENT_LEFT - Aligns with the left edge</li>
     * <li>ALIGNMENT_RIGHT - Aligns with the right edge</li>
     * <li>ALIGNMENT_HCENTER - Centers horizontally in the available space</li>
     * <li>ALIGNMENT_TOP - Aligns with the top</li>
     * <li>ALIGNMENT_BOTTOM - Aligns with the bottom</li>
     * <li>ALIGNMENT_VCENTER - Centers vertically in the available space</li>
     * </ul>
     * @param flags Flags:
     * <ul>
     * <li>TEXT_SINGLE_LINE - One line. Whitespaces are threated as spaces</li>
     * <li>TEXT_DONT_CLIP - Prints outside of the given bounds if needed</li>
     * <li>TEXT_EXPAND_TABS - Expand tabs</li>
     * <li>TEXT_SHOW_MNEMONIC - Underline the mnemonic character</li>
     * <li>TEXT_WORD_WRAP - Breaks lines at appropriate points</li>
     * <li>TEXT_WRAP_ANYWHERE - Breaks lines anywhere, even within words</li>
     * <li>TEXT_DONT_PRINT - Behaves like a "hidden" text, not printed</li>
     * <li>TEXT_INCLUDE_TAILING_SPACES - Trailing whitespace not deleted automatically</li>
     * <li>TEXT_JUSTIFICATIONFORCED - Ensures that text lines are justified</li>
     * </ul>
     */
    public void drawString(
        String string, int x, int y, int width, int height, int alignments, int flags, boolean isTransparent) {
        checkState();

        if (string == null) {
            throw new NullPointerException("string is null");
        }
        if( (flags & TEXT_DONT_PRINT)== TEXT_DONT_PRINT)
        { // nothing should be drawn
            return;
        }
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.drawString(string, x, y, width, height, alignments, flags, isTransparent);
        } else {
            OS.graphicsContext_drawString(handle, string, x, y, width, height, alignments, flags, isTransparent);
        }
    }

    /**
     * Draws given WindowSurface. Supported only when WindowSurface
     * uses local off-screen image as draw surface.
     * The defined rectangular area is drawn to the same x and y position in the 
     * target as the given arguments x and y specify.
     * 
     * @param surface The WindowSurface to draw
     * @param x The x-coordinate of the source area to draw
     * @param y The y-coordinate of the source area to draw
     * @param width The width of the source area to draw
     * @param height The height of the source area to draw
     * @throws NullPointerException if surface is null
     */
    public void drawWindowSurface(WindowSurface surface, int x, int y, int width, int height) {
    	if(surface == null) 
    	{
    	    throw new NullPointerException("surface is null");
    	}
    	OS.graphicsContext_drawWindowSurface(handle, surface.handle, x, y, width, height);
    }
    
    
    /**
     * Fill interior of circular or elliptical arc within specified rectangular area, using background color.
     *
     * @param x The x coordinate of top left corner of arc rectangle
     * @param y The y coordinate of top left corner of arc rectangle
     * @param width The width of arc rectangle
     * @param height The height of arc rectangle
     * @param startAngle The starting angle for arc
     * @param arcAngle The span angle for arc
     * @throws IllegalStateException if no target is bound
     */
    public void fillArc(int x, int y, int width, int height, int startAngle, int arcAngle) {
        checkState();
        if(width <= 0 || height <= 0) {
            return;
        }
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.fillArc(x, y, width, height, startAngle, arcAngle);
        } else {
            OS.graphicsContext_fillArc(handle, x, y, width, height, startAngle, arcAngle);
        }
    }

    /**
     * Fills interior of an ellipse/oval within specified rectangular area, using background color.
     *
     * @param x The x coordinate of top left corner of ellipse rectangle
     * @param y The y coordinate of top left corner of ellipse rectangle
     * @param width The width of ellipse rectangle
     * @param height The height of ellipse rectangle
     * @throws IllegalStateException if no target is bound
     */
    public void fillEllipse(int x, int y, int width, int height) {
        checkState();
        if(width <= 0 || height <= 0) {
            return;
        }
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.fillEllipse(x, y, width, height);
        } else {
            OS.graphicsContext_fillEllipse(handle, x, y, width, height);
        }
    }

    /**
     * Fills interior of specified rectangle with a gradient sweeping from left to right or top to bottom
     * from foregroundcolor to backgroundcolor or from backgroundcolor to foregroundcolor depending on <code>swapColors</code>.
     *
     * @param x The x coordinate of top left corner of rectangle
     * @param y The y coordinate of top left corner of rectangle
     * @param width The width of rectangle
     * @param height The height of rectangle
     * @param vertical if true sweeps from top to bottom, else sweeps from left to right
     * @param swapColors if true sweep is performed from background color to foreground color, else from foreground color to background color
     * @throws IllegalStateException if no target is bound
     */
    public void fillGradientRect(int x, int y, int width, int height, boolean vertical, boolean swapColors) {
        checkState();
        if(width <= 0 || height <= 0) {
            return;
        }
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.fillGradientRect(x, y, width, height, vertical, swapColors);
        } else {
            OS.graphicsContext_fillGradientRect(handle, x, y, width, height, vertical, swapColors);
        }
    }

    /**
     * Fills interior of closed polygon which is specified by array of integer coordinates,
     * using background color. Lines are drawn between each consecutive x,y pairs and between first and last point.
     *
     * @param pointArray An array alternating x and y values which are the verices of the polygon
     * @throws NullPointerException if <code>pointArray</code> is null
     * @throws IllegalStateException if no target is bound
     */
    public void fillPolygon(int[] pointArray)  {
        checkState();
        if (pointArray == null) {
            throw new NullPointerException("pointaArray is null");
        }
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.fillPolygon(pointArray);
        } else {
            OS.graphicsContext_fillPolygon(handle, pointArray);
        }
    }

    /**
     * Fills interior of the specified rectangle, using background color.
     *
     * @param x The x coordinate of top left corner of rectangle
     * @param y The y coordinate of top left corner of rectangle
     * @param width The width of rectangle
     * @param height The height of rectangle
     * @throws IllegalStateException if no target is bound
     */
    public void fillRect(int x, int y, int width, int height) {
        checkState();
        if(width <= 0 || height <= 0) {
            return;
        }
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.fillRect(x, y, width, height);
        } else {
            OS.graphicsContext_fillRect(handle, x, y, width, height);
        }
    }

    /**
     * Fills interior of round-cornered rectangle, using background color.
     *
     * @param x The x coordinate of top left corner of rectangle
     * @param y The y coordinate of top left corner of rectangle
     * @param width The width of rectangle
     * @param height The height of rectangle
     * @param arcWidth The width of the rectangle corner arc
     * @param arcHeight The heiht of the rectangle corner arc
     * @throws IllegalStateException if no target is bound
     */
    public void fillRoundRect(int x, int y, int width, int height, int arcWidth, int arcHeight) {
        checkState();

        if(width <= 0 || height <= 0) {
            return;
        }
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.fillRoundRectangle(x, y, width, height, arcWidth, arcHeight);
        } else {
            OS.graphicsContext_fillRoundRect(handle, x, y, width, height, arcWidth, arcHeight);
        }
    }

    /**
     * Get the advance width of the specified character in the font which is
     * currently selected into the receiver.
     * @param ch The character to be measured
     * @return The distance in the x direction to move past the character before painting the next
     */
    public int getAdvancedCharacterWidth(char ch) {
        checkState();
        if(targetData.isJavaCommandBuffer) {
            return targetData.commandBuffer.getAdvancedCharacterWidth(ch, true);
        } else {
            return OS.graphicsContext_getCharacterWidth(handle, ch, true);
        }
    }

    /**
     * Get the advance width of the specified character in the font which is
     * currently selected into the receiver.
     * The width is defined as the space taken up by the actual character,
     * not including the leading and tailing whitespace or overhang.
     *
     * @param ch The character to be measured
     * @return The width of the character
     */
    public int getCharacterWidth(char ch) {
        checkState();
        if(targetData.isJavaCommandBuffer) {
            return targetData.commandBuffer.getAdvancedCharacterWidth(ch, true);
        } else {
            return OS.graphicsContext_getCharacterWidth(handle, ch, false);
        }
    }

    /**
     * Gets the <code>FontMetrics</code> data.
     *
     * Values are stored in the data array as follows:
     * <ul>
      * <li>data[0] = ASCENT
     * <li>data[1] = AVERAGE CHAR WIDTH
     * <li>data[2] = DESCENT
     * <li>data[3] = HEIGHT
     * <li>data[4] = LEADING
     * </ul>
     * @param data The integer array where to copy <code>FontMetrics</code> data.
     * @param fontHandle If not zero then Fontmetrics data will be based on it
     * otherwise data is based on the font which is currently selected into the receiver.
     * @throws NullPointerException if data is null
     * @throws IllegalStateException if no target is bound
     */

    public void getFontMetricsData(int[] data, int fontHandle) {
        getFontMetricsData(data, fontHandle, true);
    }

    public void getFontMetricsData(int[] data, int fontHandle, boolean check) {
        if (check) checkState();
        if (data == null) {
            throw new NullPointerException("data is null");
        }
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.getFontMetricsData(data, fontHandle);
        } else {
            OS.graphicsContext_getFontMetricsData(handle, data, fontHandle);
        }
    }

    /**
     * Gets the alpha component of the background color.
     *
     * @return The alpha value (0-255)
     * @throws IllegalStateException if no target is bound
     */
    public int getBackgroundAlpha() {
        checkState();
        if(targetData.isJavaCommandBuffer) {
            return targetData.commandBuffer.getBackgroundAlpha();
        } else {
            return OS.graphicsContext_getBackgroundAlpha(handle);
        }
    }

    /**
     * Gets current background color in argb format.
     *
     * @return Interger containing current color in #AARRGGBB format
     * @throws IllegalStateException if no target is bound
     */
    public int getBackgroundColor() {
        checkState();
        if(targetData.isJavaCommandBuffer) {
            return targetData.commandBuffer.getBackgroundColor();
        } else {
            return OS.graphicsContext_getBackgroundColor(handle);
        }
    }

    /**
     * Gets current blending mode.
     * Default blending mode is SRC_OVER.
     *
     * @return blending mode
     * @throws IllegalStateException if no target is bound
     */
    public int getBlendingMode() {
        checkState();
        if(targetData.isJavaCommandBuffer) {
            return targetData.commandBuffer.getBlendingMode();
        } else {
            return OS.graphicsContext_getBlendingMode(handle);
        }
    }

    /**
     * Gets the current clip rectangle. Retrieved clip is copied in given integer array which
     * must have at least size of 4, in order to populate all clip values.
     *
     * Values are stored in clip array as follows:
     * <ul>
     * <li>clip[RECT_X] - the x coordinate of clip</li>
     * <li>clip[RECT_Y] - the y coordinate of clip
     * <li>clip[RECT_WIDTH] - the width of clip
     * <li>clip[RECT_HEIGHT] - the height of clip
     * </ul>
     * @param clip The integer array where to copy clip data
     * @throws NullPointerException if clip is null
     * @throws IllegalArgumentException if clip.length < 4
     * @throws IllegalStateException if no target is bound
     */

    public void getClip(int[] clip) {
        checkState();
        if (clip == null) {
            throw new NullPointerException("clip is null");
        }
        if (clip.length < 4 ) {
            throw new IllegalArgumentException("clip array size is < 4");
        }
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.getClip(clip);
        } else {
            OS.graphicsContext_getClip(handle, clip);
        }
    }

    /**
     * Gets the alpha component of the foreground color.
     *
     * @return The alpha value (0-255)
     * @throws IllegalStateException if no target is bound
     */
    public int getForegroundAlpha() {
        checkState();
        if(targetData.isJavaCommandBuffer) {
            return targetData.commandBuffer.getForegroundAlpha();
        } else {
            return OS.graphicsContext_getForegroundAlpha(handle);
        }
    }

    /**
     * Gets current foreground color in argb format.
     *
     * @return Integer containing current color in #AARRGGBB format
     * @throws IllegalStateException if no target is bound
     */
    public int getForegroundColor() {
        checkState();
        if(targetData.isJavaCommandBuffer) {
            return targetData.commandBuffer.getForegroundColor();
        } else {
            return OS.graphicsContext_getForegroundColor(handle);
        }
    }

    /**
     * @see #getTextBoundingBox()
     */
    public void getTextBoundingBox(
        int[] boundingBox, String string, int alignments, int flags) {
        getTextBoundingBox(boundingBox, string, alignments, flags, 0, 0, 0, 0);
    }

    /**
     * Returns a bounding box required by the given <code>string</code> when it is
     * drawn inside the given rectangle (<code>rectX</code>, <code>rectY</code>,
     * <code>rectWidth</code>, <code>rectHeight</code>) with the given <code>flags</code>
     * and <code>alignments</code> using the currently set font.
     * If the text does not fit within the given rectangle then the required bounding box
     * is returned.
     *
     * @param boundingBox The integer array where to copy bounding box data.
     * Result is stored in the array as follows:
     * <ul>
     * <li>boundingBox[RECT_X] - top-left x of the text bounding box</li>
     * <li>boundingBox[RECT_Y] - top-left y of the text bounding box</li>
     * <li>boundingBox[RECT_WIDTH] - the width of the text bounding box</li>
     * <li>boundingBox[RECT_HEIGHT] - the height of the text bounding box</li>
     * </ul>
     * @param string The string
     * @param alignments Alignments @see #drawString()
     * @param flags Flags  @see #drawString()
     * @param rectX Drawing rectangle's top-left x coordinate.
     * @param rectY Drawing rectangle's top-left y coordinate.
     * @param rectWidth Drawing rectangle width.
     * @param rectHeight Drawing rectangle height.
     * @throws NullPointerException if <code>boundingBox</code> is null
     * @throws NullPointerException if <code>string</code> is null
     * @throws IllegalArgumentException if <code>boundingBox</code>.length < 4
     * @throws IllegalStateException if no target is bound
     */
    public void getTextBoundingBox(
        int[] boundingBox, String string, int alignments, int flags,
        int rectX, int rectY, int rectWidth, int rectHeight) {
        checkState();
        if (boundingBox == null) {
            throw new NullPointerException("boundingBox is null");
        }
        if (string == null) {
            throw new NullPointerException("string is null");
        }
        if (boundingBox.length < 4 ) {
            throw new IllegalArgumentException("boundingBox size must not be < 4");
        }
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.getTextBoundingBox(boundingBox, string, alignments, flags, rectX, rectY, rectWidth, rectHeight);
        } else {
            OS.graphicsContext_getTextBoundingBox(
                    handle, boundingBox, string, alignments, flags, rectX, rectY, rectWidth, rectHeight);
        }
    }

    /**
     * Gets the stroke width in pixels.
     *
     * @return stroke width in pixels
     * @throws IllegalStateException if no target is bound
     */
    public int getStrokeWidth() {
        checkState();
        if(targetData.isJavaCommandBuffer) {
            return targetData.commandBuffer.getStrokeWidth();
        } else {
            return OS.graphicsContext_getStrokeWidth(handle);
        }
    }

    /**
     * Gets current stroke style, which will be one of the constants:
     * <ul>
     * <li>STROKE_NO: Stroke is not used</li>
     * <li>STROKE_SOLID: Plain line style</li>
     * <li>STROKE_DASH: Line contains dashes separated by a few pixels</li>
     * <li>STROKE_DOT: Line contains dots separated by a few pixels</li>
     * <li>STROKE_DASH_DOT: Dot-dash line type</li>
     * <li>STROKE_DASH_DOT_DOT: Dash-dot-dot line type</li>
     * </ul>
     *
     * @return The style used for strokes
     * @throws IllegalStateException if no target is bound
     */
    public int getStrokeStyle() {
        checkState();
        if(targetData.isJavaCommandBuffer) {
            return targetData.commandBuffer.getStrokeStyle();
        } else {
            return OS.graphicsContext_getStrokeStyle(handle);
        }
    }

    /**
     * Gets the x coorinate of translated origin
     * @return the x coorinate of origin
     */
    public int getTranslateX() {
        checkState();
        if(targetData.isJavaCommandBuffer) {
            return targetData.commandBuffer.getTranslateX();
        } else {
            return OS.graphicsContext_getTranslateX(handle);
        }
    }

    /**
     * Gets the y coorinate of translated origin
     * @return the y coorinate of origin
     */
    public int getTranslateY() {
        checkState();
        if(targetData.isJavaCommandBuffer) {
            return targetData.commandBuffer.getTranslateY();
        } else {
            return OS.graphicsContext_getTranslateY(handle);
        }
    }

    /**
     * Sets alpha component value for background color
     *
     * @param alpha The alpha value to be set in range 0-255
     * @throws IllegalArgumentException if alpha is not in range of 0-255
     * @throws IllegalStateException if no target is bound
     */
    public void setBackgroundAlpha(int alpha) {
        checkState();
        if (alpha < 0 || alpha > 255 ) {
            throw new IllegalArgumentException("alpha is not in range 0-255");
        }
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.setBackgroundAlpha(alpha);
        } else {
            OS.graphicsContext_setBackgroundAlpha(handle, alpha);
        }
    }

    /**
     * Sets the background color.
     * The background color is used for fill operations and as the background color when text is drawn
     *
     * @param argb The color and alpha to be set in 32-bit format #AARRGGBB
     * @throws IllegalStateException if no target is bound
     */
    public void setBackgroundColor(int argb, boolean updateAlpha) {
        checkState();
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.setBackgroundColor(argb, updateAlpha);
        } else {
            OS.graphicsContext_setBackgroundColor(handle, argb, updateAlpha);
        }
    }

    /**
     * Sets the background color.
     * The background color is used for fill operations and as the background color when text is drawn
     *
     * @param red The red component of the color being set in range 0-255
     * @param green The green component of the color being set in range 0-255
     * @param blue The blue component of the color being set in range 0-255
     * @throws IllegalArgumentException if red, green or blue is not in range of 0-255
     * @throws IllegalStateException if no target is bound
     */
    public void setBackgroundColor(int red, int green, int blue) {
        checkState();

        if (red   < 0 || red   > 255 ||
            green < 0 || green > 255 ||
            blue  < 0 || blue  > 255 ) {
            throw new IllegalArgumentException();
        }
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.setBackgroundColor((0xff000000 | (red << 16) | (green << 8) | blue), false);
        } else {
            OS.graphicsContext_setBackgroundColor(handle, (0xff000000 | (red << 16) | (green << 8) | blue), false);
        }
    }

    /**
     * Sets the background color.
     * The background color is used for fill operations and as the background color when text is drawn
     *
     * @param alpha The alpha component of the color being set in range 0-255
     * @param red The red component of the color being set in range 0-255
     * @param green The green component of the color being set in range 0-255
     * @param blue The blue component of the color being set in range 0-255
     * @throws IllegalArgumentException if alpha, red, green or blue is not in range of 0-255
     * @throws IllegalStateException if no target is bound
     */
    public void setBackgroundColor(int alpha, int red, int green, int blue) {
        checkState();

        if (alpha < 0 || alpha > 255 ||
            red   < 0 || red   > 255 ||
            green < 0 || green > 255 ||
            blue  < 0 || blue  > 255 ) {
                throw new IllegalArgumentException();
        }
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.setBackgroundColor(((alpha << 24) | (red << 16) | (green << 8) | blue), true);
        } else {
            OS.graphicsContext_setBackgroundColor(handle,((alpha << 24) | (red << 16) | (green << 8) | blue), true);
        }
    }

    /**
     * Sets blending mode. The blending mode dictates how rendered pixels are combined with the destination pixels.
     * The default blending mode is BLENDING_MODE_SRC_OVER, i.e. if not other mode is set using this method.
     *
     * If the bound target does not have and alpha channel the set mode has no effect.
     *
     * @param mode The blending mode to be set, one of BLENDING_MODE_SRC, BLENDING_MODE_SRC_OVER or BLENDING_MODE_XOR
     * @throws IllegalArgumentException if mode is not one of listed above
     * @throws IllegalStateException if no target is bound
     */
    public void setBlendingMode(int mode) {
        checkState();
        if ( mode != BLENDING_MODE_SRC &&
             mode != BLENDING_MODE_SRC_OVER &&
             mode != BLENDING_MODE_XOR) {
            throw new IllegalArgumentException("Illegal blending mode");
        }
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.setBlendingMode(mode);
        } else {
            OS.graphicsContext_setBlendingMode(handle, mode);
        }
    }

    /**
     * Returns if clipped or not.
     * @return True if clipping region is set, and false otherwise
     */
    public boolean hasClipping() {
        checkState();
        if(targetData.isJavaCommandBuffer) {
            return targetData.commandBuffer.hasClipping();
        } else {
            return OS.graphicsContext_hasClipping(handle);
        }
    }

    /**
     * Reset clipping region.
     */
    public void cancelClipping() {
        checkState();
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.cancelClipping();
        } else {
            OS.graphicsContext_cancelClipping(handle);
        }
    }

    /**
     * Sets the clip rectangle where draw operations will take affect.
     * Rendering operations have no effect outside of the clipping area.
     *
     * If intersects is set as true then the clip area is set to area where the current and given
     * clip rectangle intersect, i.e. where they overlap.
     *
     * If width or height is negative request is silently ignored.
     *
     * @param x The x coordinate for the clip area
     * @param y The y coordinate for the clip area
     * @param width The width of the clip area
     * @param height The height of the clip area
     * @param intersects if true clip is set as interscetion of current and given clip rectangle,
     * otherwise the clip is set as specified
     * @throws IllegalStateException if no target is bound
     */
    public void setClip(int x, int y, int width, int height, boolean intersects) {
        checkState();

        if(width < 0 || height < 0) {
            return;
        }
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.setClip(x, y, width, height, intersects);
        } else {
            OS.graphicsContext_setClip(handle, x, y, width, height, intersects);
        }
    }

    /**
     * Sets font which is used in drawString.
     *
     * @param fontHandle The handle of native QFont
     */
    public void setFont(int fontHandle) {
        checkState();
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.setFont(fontHandle);
        } else {
            OS.graphicsContext_setFont(handle, fontHandle);
        }
    }

    /**
     * Sets alpha component value for foreground color
     *
     * @param alpha The alpha value to be set in range 0-255
     * @throws IllegalArgumentException if alpha is not in range of 0-255
     * @throws IllegalStateException if no target is bound
     */
    public void setForegroundAlpha(int alpha) {
        checkState();
        if (alpha < 0 || alpha > 255 ) {
            throw new IllegalArgumentException("alpha is not in range 0-255");
        }
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.setForegroundAlpha(alpha);
        } else {
            OS.graphicsContext_setForegroundAlpha(handle, alpha);
        }
    }

    /**
     * Sets the foreground color.
     * TThe foreground color is used for non fill primitives.
     *
     * @param argb The color and alpha to be set in 32-bit format #AARRGGBB
     * @throws IllegalStateException if no target is bound
     */
    public void setForegroundColor(int argb, boolean updateAlpha) {
        checkState();
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.setForegroundColor(argb, updateAlpha);
        } else {
            OS.graphicsContext_setForegroundColor(handle, argb, updateAlpha);
        }
    }
    /**
     * Sets the foreground color.
     * The foreground color is used for non fill primitives.
     *
     * @param red The red component of the color being set in range 0-255
     * @param green The green component of the color being set in range 0-255
     * @param blue The blue component of the color being set in range 0-255
     * @throws IllegalArgumentException if red, green or blue is not in range of 0-255
     * @throws IllegalStateException if no target is bound
     */
    public void setForegroundColor(int red, int green, int blue) {
        checkState();

        if (red   < 0 || red   > 255 ||
            green < 0 || green > 255 ||
            blue  < 0 || blue  > 255 ) {
            throw new IllegalArgumentException();
        }
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.setForegroundColor((0xff000000 | (red << 16) | (green << 8) | blue), false);
        } else {
            OS.graphicsContext_setForegroundColor(handle, (0xff000000 | (red << 16) | (green << 8) | blue), false);
        }
    }

    /**
     * Sets the foreground color.
     * The foreground color is used for non fill primitives.
     *
     * @param alpha The alpha component of the color being set in range 0-255
     * @param red The red component of the color being set in range 0-255
     * @param green The green component of the color being set in range 0-255
     * @param blue The blue component of the color being set in range 0-255
     * @throws IllegalArgumentException if alpha, red, green or blue is not in range of 0-255
     * @throws IllegalStateException if no target is bound
     */
    public void setForegroundColor(int alpha, int red, int green, int blue) {
        checkState();

        if (alpha < 0 || alpha > 255 ||
            red   < 0 || red   > 255 ||
            green < 0 || green > 255 ||
            blue  < 0 || blue  > 255 ) {
            throw new IllegalArgumentException();
        }
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.setForegroundColor(((alpha << 24) | (red << 16) | (green << 8) | blue), true);
        } else {
            OS.graphicsContext_setForegroundColor(handle,((alpha << 24) | (red << 16) | (green << 8) | blue), true);
        }
    }

    /**
     * Sets the stroke style, which is one of STROKE_SOLID, STROKE_DOT, STROKE_DASH,
     * STROKE_DASHDOT or STROKE_DASHDOTDOT.
     *
     * @param style The style to be set
     * @throws IllegalArgumentException if style is not one of listed above
     * @throws IllegalStateException if no target is bound
     */
    public void setStrokeStyle(int style) {
        checkState();

        if (style != STROKE_SOLID &&
            style != STROKE_DOT &&
            style != STROKE_DASH &&
            style != STROKE_DASH_DOT &&
            style != STROKE_DASH_DOT_DOT ) {
            throw new IllegalArgumentException("Invalid style: " + style);
        }
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.setStrokeStyle(style);
        } else {
            OS.graphicsContext_setStrokeStyle(handle, style);
        }
    }

    /**
     * Sets the stroke width in pixels.
     *
     * @param width The stroke width in pixels
     * @throws IllegalArgumentException if width is negative or zero
     * @throws IllegalStateException if no target is bound
     */
    public void setStrokeWidth(int width) {
        checkState();
        if(width < 0) {
            throw new IllegalArgumentException("Invalid width: " + width);
        }
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.setStrokeWidth(width);
        } else {
            OS.graphicsContext_setStrokeWidth(handle, width);
        }
    }

    /**
     * Translates the origin of this graphics context to point (x,y). All coordinates used in subsequent
     * rendering operations are relative to this new origin.
     *
     * @param x the x coordinate of new origin
     * @param y the y coordinate of new origin
     * @throws IllegalStateException if no target is bound
     */
    public void translate(int x, int y) {
        checkState();
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.translate(x, y);
        } else {
            OS.graphicsContext_translate(handle, x, y);
        }
    }

    /**
     * Scales the coordinate system by (x, y). All coordinates used in subsequent
     * rendering operations are scaled with these values.
     *
     * @param x the x scaling factor
     * @param y the y scaling factor
     * @throws IllegalStateException if no target is bound
     */
    public void scale(int x, int y) {
        checkState();
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.scale(x, y);
        } else {
            OS.graphicsContext_scale(handle, x, y);
        }
    }

    /**
     * Resets any transformations that were made using translate(), scale().
     *
     * @throws IllegalStateException if no target is bound
     */
    public void resetTransform() {
        checkState();
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.resetTransform();
        } else {
            OS.graphicsContext_resetTransform(handle);
        }
    }

    /**
     * Disposes this graphics context and frees all native resources. If there is a target bound, it is released
     * automatically before cleaning resources. If this instance has already been disposed request is silently ignored.
     */
    public void dispose() {
        Utils.validateUiThread();
        // If already disposed just return
        if (disposed) {
            return;
        } else {
            // in case there's a bound target release it before disposing
            if (targetData.target != null) {
                releaseTarget();
            }
            // dispose native peer
            OS.graphicsContext_dispose(handle);
            handle = 0;
            disposed = true;
        }
    }

    /**
     * Gets the status of this graphics context.
     *
     * @return True if this instance has been already disposed otherwise false
     */
    public boolean isDisposed() {
        return disposed;
    }

    /**
     * Gets the status of this graphics context.
     *
     * @return True if this instance is currently being used
     */
    public boolean isActive() {
        return (targetData.target != null);
    }
    
    /**
     * Returns native side handle.
     * @return the native handle.
     */
    public int getHandle() {
        return handle;
    }

    /**
     * Saves graphics context settings.
     */
    public void saveSettings() {
        checkState();
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.saveSettings();
        } else {
            OS.graphicsContext_saveSettings(handle);
        }
    }

    /**
     * Restores graphics context settings.
     */
    public void restoreSettings() {
        checkState();
        if(targetData.isJavaCommandBuffer) {
            targetData.commandBuffer.restoreSettings();
        } else {
            OS.graphicsContext_restoreSettings(handle);
        }
    }
    
    /**
     * private ,method for checking the state of this instance.
     */
    private void checkState() {
        Utils.validateUiThread();
        if (disposed) {
            throw new IllegalStateException("GraphicsContext already disposed");
        }
        if ( targetData.target == null ) {
            throw new IllegalStateException("No target set");
        }
    }
    
    /**
     * Validates manipulation argument.
     * @param manipulation Value to be checked
     * @return true if given argument is valid, otherwise false
     */
    private boolean isManipulationValid(int manipulation) {
        if ((manipulation != Image.TRANS_NONE) &&
            (manipulation != Image.TRANS_ROT90) &&
            (manipulation != Image.TRANS_ROT180)&&
            (manipulation != Image.TRANS_ROT270) &&
            (manipulation != Image.TRANS_MIRROR) &&
            (manipulation != Image.TRANS_FLIP_HORIZONTAL) &&
            (manipulation != Image.TRANS_FLIP_VERTICAL) &&
            (manipulation != Image.TRANS_MIRROR_ROT180) &&
            (manipulation != Image.TRANS_MIRROR_ROT270) &&
            (manipulation != Image.TRANS_MIRROR_ROT90)) {
            return false;
        }
        return true;
    }

    /**
     * Processes command buffer
     * @param buffer The buffer to be processed
     */
    private void processBuffer(JavaCommandBuffer buffer) {
        int i = 0;
        int intCount = buffer.intParamCount();
        int imageCount = 0;
        int stringCount = 0;
        int rgbCount = 0;
        int[] intParams = buffer.intParams();
        Vector images = buffer.images();
        String strings = buffer.strParams();
        Vector rgbs = buffer.rgbParams();
        
        while( i < intCount ) {
            switch ((intParams[i] >> 24)) {
                //
                // Draw op code handling
                //
                case JavaCommandBuffer.DRAW_PREFIX:
                {
                    switch (intParams[i++]) {
                        case JavaCommandBuffer.OP_DRAWARC:
                        {
                            OS.graphicsContext_drawArc(handle, 
                                                       intParams[i++], 
                                                       intParams[i++], 
                                                       intParams[i++], 
                                                       intParams[i++], 
                                                       intParams[i++], 
                                                       intParams[i++]);
                            break;
                        }
                        case JavaCommandBuffer.OP_DRAWLINE:
                        {
                            OS.graphicsContext_drawLine(handle, 
                                                        intParams[i++], 
                                                        intParams[i++], 
                                                        intParams[i++], 
                                                        intParams[i++]);
                            break;
                        }
                        case JavaCommandBuffer.OP_DRAWIMAGE1:
                        {
                            Image img = (Image)images.elementAt(imageCount++);
                            OS.graphicsContext_drawImage(handle, 
                                                         img.handle, 
                                                         intParams[i++], 
                                                         intParams[i++]);
                            // As we made a shallow copy of the image in buffer
                            // its safe to delete that here
                            img.dispose(); 
                            break;
                        }
                        case JavaCommandBuffer.OP_DRAWIMAGE2:
                        {
                            Image img = (Image)images.elementAt(imageCount++);  
                            OS.graphicsContext_drawImage(handle, 
                                                         img.handle, 
                                                         intParams[i++], 
                                                         intParams[i++], 
                                                         intParams[i++], 
                                                         intParams[i++], 
                                                         intParams[i++], 
                                                         intParams[i++], 
                                                         intParams[i++], 
                                                         intParams[i++],
                                                         intParams[i++]);
                            // As we made a shallow copy of the image in buffer
                            // its safe to delete that here
                            img.dispose(); 
                            break;
                        }
                        case JavaCommandBuffer.OP_DRAWFOCUS:
                        {
                            OS.graphicsContext_drawFocus(handle, 
                                                         intParams[i++], 
                                                         intParams[i++], 
                                                         intParams[i++], 
                                                         intParams[i++]);
                            break;
                        }
                        case JavaCommandBuffer.OP_DRAWELLIPSE:
                        {
                            OS.graphicsContext_drawEllipse(handle, 
                                                           intParams[i++], 
                                                           intParams[i++], 
                                                           intParams[i++], 
                                                           intParams[i++]);
                            break;
                        }
                        case JavaCommandBuffer.OP_DRAWPOINT:
                        {
                            OS.graphicsContext_drawPoint(handle, intParams[i++], intParams[i++]);
                            break;
                        }
                        case JavaCommandBuffer.OP_DRAWPOLYGON:
                        {
                            int arraySize = intParams[i++];
                            int[] pointArray = new int[arraySize];
                            System.arraycopy(intParams, i, pointArray, 0, arraySize);
                            i += arraySize;
                            OS.graphicsContext_drawPolygon(handle, pointArray);
                            break;
                        }
                        case JavaCommandBuffer.OP_DRAWPOLYLINE:
                        {
                            int arraySize = intParams[i++];
                            int [] pointArray = new int[arraySize];
                            System.arraycopy(intParams, i, pointArray, 0, arraySize);
                            i += arraySize;
                            OS.graphicsContext_drawPolyline(handle, pointArray);
                            break;
                        }
                        case JavaCommandBuffer.OP_DRAWRECT:
                        {
                            OS.graphicsContext_drawRect(handle, intParams[i++], intParams[i++], intParams[i++], intParams[i++]);
                            break;
                        }
                        case JavaCommandBuffer.OP_DRAWRGB_INT:
                        {
                            int index = rgbCount++;
                            int[] rgbData = (int[])rgbs.elementAt(index);
                            OS.graphicsContext_drawRGB(handle, 
                                                       rgbData, 
                                                       intParams[i++], 
                                                       intParams[i++], 
                                                       intParams[i++], 
                                                       intParams[i++], 
                                                       intParams[i++], 
                                                       intParams[i++], 
                                                      (intParams[i++] != 0), 
                                                       intParams[i++]);
                            rgbs.setElementAt(null, index);
                            break;
                        }
                        case JavaCommandBuffer.OP_DRAWRGB_BYTE:
                        {
                            int index1 = rgbCount++;
                            byte[] rgbData = (byte[])rgbs.elementAt(index1);
                            int index2 = rgbCount++;
                            byte[] transparencyMask = (byte[])rgbs.elementAt(index2);
                            OS.graphicsContext_drawRGB(handle, 
                                                       rgbData, 
                                                       transparencyMask, 
                                                       intParams[i++], 
                                                       intParams[i++], 
                                                       intParams[i++], 
                                                       intParams[i++], 
                                                       intParams[i++], 
                                                       intParams[i++], 
                                                       intParams[i++], 
                                                       intParams[i++]);
                            rgbs.setElementAt(null, index1);
                            rgbs.setElementAt(null, index2);
                            break;
                        }
                        case JavaCommandBuffer.OP_DRAWRGB_SHORT:
                        {
                            int index = rgbCount++;
                            short[] rgbData = (short[])rgbs.elementAt(index);
                            OS.graphicsContext_drawRGB(handle, 
                                                       rgbData, 
                                                       intParams[i++], 
                                                       intParams[i++], 
                                                       intParams[i++], 
                                                       intParams[i++], 
                                                       intParams[i++], 
                                                       intParams[i++], 
                                                      (intParams[i++] != 0), 
                                                       intParams[i++], 
                                                       intParams[i++]);
                            rgbs.setElementAt(null, index);
                            break;
                        }
                        case JavaCommandBuffer.OP_DRAWROUNDRECT:
                        {
                            OS.graphicsContext_fillRoundRect(handle, 
                                                             intParams[i++], 
                                                             intParams[i++], 
                                                             intParams[i++], 
                                                             intParams[i++], 
                                                             intParams[i++], 
                                                             intParams[i++]);
                            break;
                        }
                        case JavaCommandBuffer.OP_DRAWSTRING:
                        {
                            int stringLength = intParams[i++];
                            String string = strings.substring(stringCount, stringCount + stringLength);
                            stringCount += stringLength;
                            OS.graphicsContext_drawString(handle, 
                                                          string, 
                                                          intParams[i++], 
                                                          intParams[i++], 
                                                          intParams[i++], 
                                                          intParams[i++], 
                                                          intParams[i++], 
                                                          intParams[i++], 
                                                         (intParams[i++] != 0));
                            break;
                        }
                        default:
                            throw new RuntimeException("Internal error: Unknown opcode");
                    }
                    break;
                }
                
                //
                // Fill op codes handling
                //
                case JavaCommandBuffer.FILL_PREFIX:
                {
                    switch (intParams[i++]) {
                        case JavaCommandBuffer.OP_FILLRECT:
                        {
                            OS.graphicsContext_fillRect(handle,
                                                        intParams[i++], 
                                                        intParams[i++], 
                                                        intParams[i++], 
                                                        intParams[i++]);
                            break;
                        }
                        case JavaCommandBuffer.OP_FILLARC:
                        {
                            OS.graphicsContext_fillArc(handle, 
                                                       intParams[i++], 
                                                       intParams[i++], 
                                                       intParams[i++], 
                                                       intParams[i++], 
                                                       intParams[i++], 
                                                       intParams[i++]);
                            break;
                        }
                        case JavaCommandBuffer.OP_FILLGRADIENTRECT:
                        {
                            OS.graphicsContext_fillGradientRect(handle, 
                                                                intParams[i++], 
                                                                intParams[i++], 
                                                                intParams[i++], 
                                                                intParams[i++], 
                                                               (intParams[i++] != 0), 
                                                               (intParams[i++] != 0));
                            break;
                        }
                        case JavaCommandBuffer.OP_FILLELLIPSE:
                        {
                            OS.graphicsContext_fillEllipse(handle, 
                                                           intParams[i++], 
                                                           intParams[i++], 
                                                           intParams[i++], 
                                                           intParams[i++]);
                            break;
                        }
                        case JavaCommandBuffer.OP_FILLPOLYGON:
                        {
                            int arraySize = intParams[i++];
                            int[] pointArray = new int[arraySize];
                            System.arraycopy(intParams, i, pointArray, 0, arraySize);
                            i += arraySize;
                            OS.graphicsContext_fillPolygon(handle, pointArray);
                            break;
                        }
                        case JavaCommandBuffer.OP_FILLROUNDRECT:
                        {
                            OS.graphicsContext_fillRoundRect(handle, 
                                                             intParams[i++], 
                                                             intParams[i++], 
                                                             intParams[i++], 
                                                             intParams[i++], 
                                                             intParams[i++], 
                                                             intParams[i++]);
                            break;
                        }
                        default:
                            throw new RuntimeException("Internal error: Unknown opcode");
                    }
                    break;
                }
                
                //
                // Setter op code handling
                //
                case JavaCommandBuffer.SET_PREFIX:
                {
                    switch (intParams[i++]) {
                        case JavaCommandBuffer.OP_SETBACKGROUNDALPHA:
                        {
                            OS.graphicsContext_setBackgroundAlpha(handle, intParams[i++]);
                            break;
                        }
                        case JavaCommandBuffer.OP_SETBACKGROUNDCOLOR:
                        {
                            OS.graphicsContext_setBackgroundColor(handle, intParams[i++], (intParams[i++] != 0));
                            break;
                        }
                        case JavaCommandBuffer.OP_SETBLENDINGMODE:
                        {
                            OS.graphicsContext_setBlendingMode(handle, intParams[i++]);
                            break;
                        }
                        case JavaCommandBuffer.OP_SETCLIP:
                        {
                            OS.graphicsContext_setClip(handle, 
                                                       intParams[i++], 
                                                       intParams[i++], 
                                                       intParams[i++], 
                                                       intParams[i++], 
                                                      (intParams[i++] != 0));
                            break;
                        }
                        case JavaCommandBuffer.OP_SETFONT:
                        {
                            OS.graphicsContext_setFont(handle, intParams[i++]);
                            break;
                        }
                        case JavaCommandBuffer.OP_SETFOREGROUNDALPHA:
                        {
                            OS.graphicsContext_setForegroundAlpha(handle, intParams[i++]);
                            break;
                        }
                        case JavaCommandBuffer.OP_SETFOREGROUNDCOLOR:
                        {
                            OS.graphicsContext_setForegroundColor(handle, intParams[i++], (intParams[i++] != 0));
                        break;
                        }
                        case JavaCommandBuffer.OP_SETSTROKESTYLE:
                        {
                            OS.graphicsContext_setStrokeStyle(handle, intParams[i++]);
                            break;
                        }
                        case JavaCommandBuffer.OP_SETSTROKEWIDTH:
                        {
                            OS.graphicsContext_setStrokeWidth(handle, intParams[i++]);
                            break;
                        }
                        default:
                            throw new RuntimeException("Internal error: Unknown opcode");
                    }
                    break;
                }
                
                //
                // misc op code handling
                //
                case JavaCommandBuffer.MISC_PREFIX:
                {
                    switch (intParams[i++]) {
                        case JavaCommandBuffer.OP_CANCELCLIPPING:
                        {
                            OS.graphicsContext_cancelClipping(handle);
                            break;
                        }
                        case JavaCommandBuffer.OP_TRANSLATE:
                        {
                            OS.graphicsContext_translate(handle, intParams[i++], intParams[i++]);
                            break;
                        }
                        case JavaCommandBuffer.OP_SCALE:
                        {
                            OS.graphicsContext_scale(handle, intParams[i++], intParams[i++]);
                            break;
                        }
                        case JavaCommandBuffer.OP_RESETTRANSFORM:
                        {
                            OS.graphicsContext_resetTransform(handle);
                            break;
                        }
                        case JavaCommandBuffer.OP_COPYAREA1:
                        {
                            break;
                        }
                        case JavaCommandBuffer.OP_COPYAREA2:
                        {
                            break;
                        }
                        default:
                            throw new RuntimeException("Internal error: Unknown opcode");
                    }
                    break;
                }
                default:
                    throw new RuntimeException("Internal error: Unkonwn opcode category");
            }
        }
    }

    private final int handle(Widget w) {
        return Internal_PackageSupport.handle(w); 
    }
}
