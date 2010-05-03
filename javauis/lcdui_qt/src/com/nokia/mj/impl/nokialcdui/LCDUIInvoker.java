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
package com.nokia.mj.impl.nokialcdui;

import javax.microedition.lcdui.*;

import org.eclipse.swt.internal.qt.graphics.GraphicsContext;

import com.nokia.mid.ui.DirectGraphics;

/**
 * Abstract class providing the interface to internal LCDUI resources.
 */
public abstract class LCDUIInvoker {

    /**
     * Invoker instance.
     */
    private static com.nokia.mj.impl.nokialcdui.LCDUIInvoker invokerInstance;

    /**
     * Method called at LCDUI initialization and setting the invoker.
     *
     * @param invoker In
     */
    public static void setInvoker(LCDUIInvoker invoker) {
        invokerInstance = invoker;
    }

    /**
     * Synchronized execution of runnable with eSWT thread.
     *
     * @param runnable a runnable
     */
    public static void eSWTUIThreadRunnerSyncExec(Runnable runnable) {
        invokerInstance.doESWTUIThreadRunnerSyncExec(runnable);
     }
    
    /**
     * Returns main eSWT Control that is used by LCDUI Displayable or Item.
     *
     * @param lcduiObject - LCDUI Displayable or Item.
     * @return eSWT Control that is used by Displayable/Item or null if no
     *         Control is available.
     */
    public static org.eclipse.swt.widgets.Control getEswtControl(Object lcduiObject) {
        return invokerInstance.doGetEswtControl(lcduiObject);
    }

    /**
     * Returns eSWT Display that is used by LCDUI.
     *
     * @return eSWT Display that is used by LCDUI.
     */
    public static org.eclipse.swt.widgets.Display getEswtDisplay() {
        return invokerInstance.doGetEswtDisplay();
    }

    public static org.eclipse.swt.graphics.Image getEswtImage(Image img) {
    	return invokerInstance.doGetEswtImage(img);
    }

    /**
     * Get the DirectGraphics object associated with this Graphics object.
     *
     * @param g Graphics object
     * @return DirectGraphics object
     */
    public static DirectGraphics getDirectGraphics(Graphics g) {
        return invokerInstance.doGetDirectGraphics(g);
    }

    public static GraphicsContext getGc(Graphics g) {
    	return invokerInstance.doGetGc(g);
    }

    /**
     * Get a freely sized Font with the given parameters.
     *
     * @param face font face
     * @param style font style
     * @param height font height
     * @return the requested Font
     */
    public static Font getFreeSizedFont(int face, int style, int height) {
        return invokerInstance.doGetFreeSizedFont(face, style, height);
    }

    /**
     * Init GameCanvas.
     *
     * @param canvas GameCanvas object
     * @param suppressKeys suppress game keys
     * @return framebuffer image
     */
    public static void initGameCanvas(Object canvas, boolean suppressKeys) {
        invokerInstance.doInitGameCanvas(canvas, suppressKeys);
    }

    /**
     * Get the frameBuffer's Graphics object.
     *
     * @param canvas GameCanvas object
     * @return Graphics object
     */
    public static Object getGraphics(Object canvas) {
        return invokerInstance.doGetGraphics(canvas);
    }

    public static Object getFlushLock(Object graphics) {
    	return invokerInstance.doGetFlushLock(graphics);
    }
    
    /**
     * Get game key states.
     *
     * @param canvas GameCanvas object
     * @return key states
     */
    public static int getKeyStates(Object canvas) {
        return invokerInstance.doGetKeyStates(canvas);
    }

    /**
     * Renders the GameCanvas's frameBuffer to the Graphics object at (0,0).
     *
     * @param canvas GameCanvas object
     * @param graphics Graphics object
     */
    public static void renderGraphics(Object canvas, Object graphics) {
        invokerInstance.doRenderGraphics(canvas, graphics);
    }

    /**
     * Flushes the GameCanvas's frameBuffer to the display.
     *
     * @param canvas GameCanvas object
     * @param x clip X
     * @param y clip Y
     * @param width clip width
     * @param height clip height
     */
    public static void flushGraphics(Object canvas,
            int x, int y, int width, int height) {
        invokerInstance.doFlushGraphics(canvas, x, y, width, height);
    }

    /**
     * Returns the dynamic property value.
     *
     * @param key property key
     * @return property value
     */
    public static String getDynamicProperty(String key) {
        return invokerInstance.doGetDynamicProperty(key);
    }

    /**
     * Detect pixel level collision.
     */
    public static boolean detectCollision(Image image1, int transform1, int p1x, int p1y,
                                          int r1x1, int r1y1, int r1x2, int r1y2,
                                          Image image2, int transform2, int p2x, int p2y,
                                          int r2x1, int r2y1, int r2x2, int r2y2) {
        return invokerInstance.doDetectCollision(image1, transform1, p1x, p1y,
                                                 r1x1, r1y1, r1x2, r1y2,
                                                 image2, transform2, p2x, p2y,
                                                 r2x1, r2y1, r2x2, r2y2);
    }


    // MMAPI support

    protected abstract org.eclipse.swt.widgets.Control doGetEswtControl(
            Object lcduiObj);

    protected abstract org.eclipse.swt.widgets.Display doGetEswtDisplay();

    protected abstract org.eclipse.swt.graphics.Image doGetEswtImage(Image img);


    // DirectGraphics support

    protected abstract DirectGraphics doGetDirectGraphics(Graphics g);

    protected abstract GraphicsContext doGetGc(Graphics g);

    protected abstract Font doGetFreeSizedFont(int face, int style, int height);

    // GameCanvas support

    protected abstract void doInitGameCanvas(Object canvas,
            boolean suppressKeys);

    protected abstract Object doGetGraphics(Object canvas);
    
    protected abstract Object doGetFlushLock(Object graphics);

    protected abstract int doGetKeyStates(Object canvas);

    protected abstract void doRenderGraphics(Object canvas, Object graphics);

    protected abstract void doFlushGraphics(Object canvas,
            int x, int y, int width, int height);

    protected abstract String doGetDynamicProperty(String key);

    protected abstract boolean doDetectCollision(Image image1, int transform1, int p1x, int p1y,
            int r1x1, int r1y1, int r1x2, int r1y2,
            Image image2, int transform2, int p2x, int p2y,
            int r2x1, int r2y1, int r2x2, int r2y2);
    
    // TactileFeedback support
    protected abstract void doESWTUIThreadRunnerSyncExec(Runnable runnable);
   

}
