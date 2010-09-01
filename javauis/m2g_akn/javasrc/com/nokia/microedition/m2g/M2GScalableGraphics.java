/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.microedition.m2g;

import javax.microedition.m2g.*;
import javax.microedition.m2g.ScalableGraphics;
import javax.microedition.lcdui.Graphics;
import com.nokia.mj.impl.rt.legacy.ToolkitInvoker;
import java.lang.StringBuffer;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Rectangle;
import com.nokia.mj.impl.utils.Logger;

/*
 * ScalableGraphics
 */
public class M2GScalableGraphics
        extends M2GObject
{
    //--------------------------------------------------
    // STATIC CONSTANTS
    //--------------------------------------------------
    // Exception text
    /* Optimization: static finals changed to local variables
    private static final String ALPHA_OUT_OF_RANGE_ESTR =
        "The alpha is out of range";
    private static final String INVALID_TARGET_ESTR =
        "The target is invalid";
    private static final String MODE_INVALID_ESTR =
        "The mode is invalid";
    private static final String NULL_IMAGE_ESTR =
        "The image is null";
    private static final String NULL_TARGET_ESTR =
        "The target is null";
    private static final String TARGET_ALREADY_BOUND_ESTR =
        "Target is already bound";
    private static final String TARGET_NOT_BOUND_ESTR =
        "Target is not bound";
    */
    // Transparency alpha max and min limits
    private static final float MAX_TRANSPARENCY_ALPHA = 1.0f;
    private static final float MIN_TRANSPARENCY_ALPHA = 0.0f;

    //--------------------------------------------------
    // VARIABLES
    //--------------------------------------------------
    private GC        iTargetGC      = null;
    private int iUseNativeClear = 0;
    private Graphics iTargetGraphics;

    //--------------------------------------------------
    // METHODS
    //--------------------------------------------------
    /**
     * Constructor
     */
    public M2GScalableGraphics()
    {
        super();
        doConstruct();
    }

    /**
     * @see javax.microedition.m2g.ScalableGraphics#bindTarget()
     */
    public synchronized void bindTarget(Object target)
    {
        // Current Toolkit is eSWT_TOOLKIT
        if (iUIToolkit)
        {
            if (iTargetGC != null)
            {
                Logger.ELOG(Logger.EJavaUI,
                            "bindTarget() - exception: " + /*SF*/"Target is already bound"/*SF*/);
                throw new IllegalStateException(/*SF*/"Target is already bound"/*SF*/);
            }
            if (target == null)
            {
                Logger.ELOG(Logger.EJavaUI,
                            "bindTarget() - exception: " + /*SF*/"The target is null"/*SF*/);
                throw new NullPointerException(/*SF*/"The target is null"/*SF*/);
            }


            if (!(target instanceof GC))
            {
                Logger.ELOG(Logger.EJavaUI, "bindTarget() - exception: "
                            + /*SF*/"The target is invalid"/*SF*/);
                throw new IllegalArgumentException(/*SF*/"The target is invalid"/*SF*/);
            }
            // Save the current graphics reference
            iTargetGC = (GC)target;

            _bind(getNativeEventSourceHandle(), getHandle(), iTargetGC.handle, iUIToolkit);
        }
        else
        {
            if (iTargetGraphics != null)
            {
                Logger.ELOG(Logger.EJavaUI, "bindTarget() - exception: "
                            + /*SF*/"Target is already bound"/*SF*/);
                throw new IllegalStateException(/*SF*/"Target is already bound"/*SF*/);
            }
            if (target == null)
            {
                Logger.ELOG(Logger.EJavaUI, "bindTarget() - exception: "
                            + /*SF*/"The target is null"/*SF*/);
                throw new NullPointerException(/*SF*/"The target is null"/*SF*/);
            }
            if (!(target instanceof Graphics))
            {
                Logger.ELOG(Logger.EJavaUI, "bindTarget() - exception: "
                            + /*SF*/"The target is invalid"/*SF*/);
                throw new IllegalArgumentException(/*SF*/"The target is invalid"/*SF*/);
            }
            // Save the current graphics reference
            iTargetGraphics = (Graphics)target;
            ToolkitInvoker invoker = ToolkitInvoker.getToolkitInvoker();
            // Flush Canvas
            M2GManager.flushDisplayBuffer();
            _bind(getNativeEventSourceHandle(),
                  getHandle(),
                  invoker.graphicsGetHandle(iTargetGraphics),
                  iUIToolkit);
        }
    }


    /**
     * @see com.nokia.microedition.m2g.M2GObject#doConstruct()
     */
    public void doConstruct()
    {
        super.doConstruct();
        setHandle(_createRenderContext(
                      getNativeEventSourceHandle(),
                      getNativeSVGProxyHandle(),
                      iUIToolkit));
        // Add object to the live objects container
        register(this);
    }

    /**
     * @see com.nokia.microedition.m2g.M2GObject#doCleanup()
     */
    public synchronized void doCleanup()
    {
        _deleteRenderContext(
            getNativeEventSourceHandle(),
            getHandle(),
            iUIToolkit);
        resetHandles();
        iTargetGC = null;
        iTargetGraphics = null;
    }

    /**
     * @see javax.microedition.m2g.ScalableGraphics#releaseTarget()
     */
    public synchronized void releaseTarget()
    {
        if (iUIToolkit)  // If the current toolkit is eSWT_TOOLKIT
        {
            // check for invalid Graphics
            if (iTargetGC == null)
            {
                Logger.ELOG(Logger.EJavaUI, "releaseTarget() - exception: "
                            + /*SF*/"Target is not bound"/*SF*/);
                throw new IllegalStateException(/*SF*/"Target is not bound"/*SF*/);
            }
            _release(getNativeEventSourceHandle(), getHandle(), iUIToolkit);
            iTargetGC = null;
        }
        else
        {
            // check for invalid Graphics
            if (iTargetGraphics == null)
            {
                Logger.ELOG(Logger.EJavaUI, "releaseTarget() - exception: "
                            + /*SF*/"Target is not bound"/*SF*/);
                throw new IllegalStateException(/*SF*/"Target is not bound"/*SF*/);
            }

            _release(getNativeEventSourceHandle(), getHandle(), iUIToolkit);
            iTargetGraphics = null;
        }

    }
    public synchronized void render(
        int x, int y, ScalableImage image, boolean aUseNativeClear)
    {
        iUseNativeClear = aUseNativeClear ? 1 : 0;
        render(x, y, image);
        iUseNativeClear = 0;
    }

    /**
     * @see javax.microedition.m2g.ScalableGraphics#render()
     */
    public synchronized void render(int x, int y, ScalableImage image)
    {
        if (image == null)
        {
            Logger.ELOG(Logger.EJavaUI, "render() - exception: "
                        + /*SF*/"The target is null"/*SF*/);
            throw new NullPointerException(/*SF*/"The target is null"/*SF*/);
        }
        if (iUIToolkit)   // If the current toolkit is eSWT_TOOLKIT
        {
            if (iTargetGC == null)
            {
                Logger.ELOG(Logger.EJavaUI, "render() - exception: "
                            + /*SF*/"Target is not bound"/*SF*/);
                throw new IllegalStateException(/*SF*/"Target is not bound"/*SF*/);
            }

            M2GDocument doc = (M2GDocument)((SVGImage)image).getDocument();
            // Get synchronized svg image data
            int svgW  = image.getViewportWidth();
            int svgH = image.getViewportHeight();
            if ((svgW == 0) || (svgH == 0))
            {
                return;
            }
            // Calculate clip dimensions
            Rectangle clipRect = iTargetGC.getClipping();
            int clipX = clipRect.x;
            int clipY = clipRect.y;
            int clipW = clipRect.width;
            int clipH = clipRect.height;

            // if none of the svg image is visible due to clipping then don't
            // bother to call render
            if ((x >= (clipX + clipW)) || (y >= (clipY + clipH)))
            {
                Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                           "render() - svg image isn't visible due to clipping");
                return;
            }
            if (((x + svgW) < clipX) || ((y + svgH) < clipY))
            {
                Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                           "render() - svg image isn't visible due to clipping");
                return;
            }
            int[] data = _renderESWT(getNativeEventSourceHandle(),
                                     getHandle(),
                                     doc.getHandle(), x, y,
                                     clipX, clipY, clipW, clipH, svgW, svgH,
                                     doc.getRootElement().getCurrentTime(),
                                     iUseNativeClear);
            int[] bitmapHandles;
            bitmapHandles = new int[4];
            for (int i = 0; i < 4; i++)
            {
                bitmapHandles[i] = data[i];
            }
            int[] renderData;
            renderData = new int[6];
            for (int i = 0; i < 6; i++)
            {
                renderData[i] = data[i+4];
            }
            iTargetGC.internal_drawM2GImage(bitmapHandles, renderData, iUseNativeClear);
        }//if(true)
        else
        {
            if (iTargetGraphics == null)
            {
                Logger.ELOG(Logger.EJavaUI, "render() - exception: "
                            + /*SF*/"Target is not bound"/*SF*/);
                throw new IllegalStateException(/*SF*/"Target is not bound"/*SF*/);
            }
            M2GDocument doc = (M2GDocument)((SVGImage)image).getDocument();
            // Get synchronized svg image data
            int svgW  = image.getViewportWidth();
            int svgH = image.getViewportHeight();
            if ((svgW == 0) || (svgH == 0))
            {
                return;
            }
            // Calculate clip dimensions
            int clipX = iTargetGraphics.getClipX() + iTargetGraphics.getTranslateX();
            int clipY = iTargetGraphics.getClipY() + iTargetGraphics.getTranslateY();
            int clipW  = iTargetGraphics.getClipWidth();
            int clipH = iTargetGraphics.getClipHeight();
            // if none of the svg image is visible due to clipping then don't
            // bother to call render
            if ((x >= (clipX + clipW)) || (y >= (clipY + clipH)))
            {
                Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                           "render() - svg image isn't visible due to clipping");
                return;
            }
            if (((x + svgW) < clipX) || ((y + svgH) < clipY))
            {
                Logger.LOG(Logger.EJavaUI, Logger.EInfo,
                           "render() - svg image isn't visible due to clipping");
                return;
            }
            _renderLCDUI(getNativeEventSourceHandle(), getHandle(), doc.getHandle(), x, y,
                         clipX, clipY, clipW, clipH, svgW, svgH,
                         doc.getRootElement().getCurrentTime());
        }
    }

    /**
     * @see javax.microedition.m2g.ScalableGraphics#setRenderingQuality(int)
     */
    public synchronized void setRenderingQuality(int mode)
    {
        if ((mode == ScalableGraphics.RENDERING_QUALITY_LOW) ||
                (mode == ScalableGraphics.RENDERING_QUALITY_HIGH))
        {
            _setRenderingQuality(getNativeEventSourceHandle(), getHandle(), mode, iUIToolkit);
        }
        else
        {
            Logger.ELOG(Logger.EJavaUI,
                        "setRenderingQuality() - exception: " + /*SF*/"The mode is invalid"/*SF*/);
            throw new IllegalArgumentException(/*SF*/"The mode is invalid"/*SF*/);
        }
    }

    /**
     * @see javax.microedition.m2g.ScalableGraphics#setTransparency(float)
     */
    public synchronized void setTransparency(float alpha)
    {
        if (alpha >= MIN_TRANSPARENCY_ALPHA && alpha <= MAX_TRANSPARENCY_ALPHA)
        {
            _setTransparency(getNativeEventSourceHandle(), getHandle(), alpha, iUIToolkit);
        }
        else
        {
            Logger.ELOG(Logger.EJavaUI,
                        "setTransparency() - exception: " + /*SF*/"The alpha is out of range"/*SF*/);
            throw new IllegalArgumentException(/*SF*/"The alpha is out of range"/*SF*/);
        }
    }


    //--------------------------------------------------
    // NATIVE METHODS
    //--------------------------------------------------
    private native int _bind(
        int aEventSourceHandle, int aRenderContextHandle, int aTargetHandle, boolean iUIToolkit);
    private native int _createRenderContext(
        int aEventSourceHandle, int aSVGProxyHandle, boolean iUIToolkit);
    private native void _deleteRenderContext(
        int aEventSourceHandle, int aRenderContextHandle, boolean iUIToolkit);
    private native int _release(
        int aEventSourceHandle, int aRenderContextHandle, boolean iUIToolkit);
    private native int _renderLCDUI(
        int aEventSourceHandle, int aRenderContextHandle, int aDocumentHandle,
        int aX, int aY, int aClipX, int aClipY, int aClipW, int aClipH,
        int aSvgW, int aSvgH, float aCurrentTime);
    private native int[] _renderESWT(
        int aEventSourceHandle, int aRenderContextHandle, int aDocumentHandle,
        int aX, int aY, int aClipX, int aClipY, int aClipW, int aClipH,
        int aSvgW, int aSvgH, float aCurrentTime, int iUseNativeClear);
    private native void _setRenderingQuality(
        int aEventSourceHandle, int aRenderContextHandle, int aMode, boolean iUIToolkit);
    private native void _setTransparency(
        int aEventSourceHandle, int aRenderContextHandle, float aAlpha, boolean iUIToolkit);
}
