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


package javax.microedition.m2g;

import com.nokia.microedition.m2g.M2GScalableGraphics;

/**
 * This is the fundamental class for 2D rendering. The ScalableGraphics context class
 * provides and handles all the rendering capability within this package. In other
 * words, the rendering can only be achieved through the render method provided
 * in this class. Note that the ScalableGraphics instance must be bound to the rendering
 * target prior to calling the render method. The implementation must clip to the
 * viewport boundaries.
*/
public class ScalableGraphics
{
    // Defines a low rendering quality level.
    public static final int RENDERING_QUALITY_LOW = 1;
    // Defines a high rendering quality level.
    public static final int RENDERING_QUALITY_HIGH = 2;

    private M2GScalableGraphics iSg;

    /**
     *  Constructor
     */
    private ScalableGraphics()
    {
				
        iSg = new M2GScalableGraphics();
        
    }

    /**
     * Binds the given Graphics as the rendering target of this ScalableGraphics context. The
     * type of the Graphics object depends on the Java profile that this specification is
     * implemented on, as follows:
     * <ul>
     * <li> javax.microedition.lcdui.Graphics on profiles supporting LCDUI;</li>
     * <li> java.awt.Graphics on profiles supporting AWT;</li>
     * <li> either of the above on profiles supporting both AWT and LCDUI.</li>
     * </ul>
     * @param target the object (Graphics) to receive the rendered image.
     * @throws NullPointerException if <code>target</code> is null.
     * @throws IllegalArgumentException if <code>target</code> is invalid.
     * @throws IllegalStateException if <code>target</code> is already bound.
     */
    public void bindTarget(java.lang.Object target)
    {
    		
        iSg.bindTarget(target);
        
    }

    /**
     * Retrieve a new instance of ScalableGraphics that can be associated to
     * an application.
     * <p>
     * @return the newly created <code>ScalableGraphics</code> instance.
     */
    public static ScalableGraphics createInstance()
    {
        return new ScalableGraphics();
    }

    /**
     * Flushes the rendered ScalableImage to the currently bound target and then releases
     * the target. This ensures that the ScalableImage is actually made visible on the target
     * that was set in bindTarget. Otherwise, the image may or may not become visible.
     * @throws IllegalStateException if <code>target</code> is not bound.
     */
    public void releaseTarget()
    {
				
        iSg.releaseTarget();
     		
    }

    /**
     * Renders the specified ScalableImage using the supplied anchor point. The anchor point given
     * is relative to the upper left corner of the rendering surface. It is important to note that
     * the content is made visible or flushed to the display only after a call is made to
     * <code>releaseTarget</code>.
     * @param x the X coordinate of the anchor point, in pixels.
     * @param y the Y coordinate of the anchor point, in pixels.
     * @param image the ScalableImage to be rendered.
     * @throws NullPointerException if <code>image</code> is null.
     * @throws IllegalStateException if <code>target</code> is not bound.
     * @see #releaseTarget
     */
    public void render(int x, int y, ScalableImage image)
    {
    		
        iSg.render(x,y, image);
        
    }

    /**
     * Set the quality of rendering in the ScalableGraphics context. It can take one of the values,
     * RENDERING_QUALITY_LOW or RENDERING_QUALITY_HIGH. Default=RENDERING_QUALITY_HIGH. The
     * implementation of these quality levels is implementation dependent and should be mapped
     * to definitions in SVG spec (shape, text, image and color rendering).
     * @param mode this value indicates the quality of rendering required.
     * @throws IllegalArgumentException if the <code>mode</code> is invalid.
     */

    public void setRenderingQuality(int mode)
    {
        iSg.setRenderingQuality(mode);
    }

    /**
     * Set the transparency in the ScalableGraphics context with the supplied alpha value.
     * Alpha value must be a floating point number in the range [0.0, 1.0]. The source pixels
     * are always combined with destination pixels using the <i>Source Over Destination</i>
     * rule [Porter-Duff]. In this context, the Source Over Destination rule has the following
     * properties: a fully opaque pixel in the source must replace the destination pixel, a
     * fully transparent pixel in the source must leave the destination pixel unchanged, and
     * a semitransparent pixel in the source must be alpha blended with the destination pixel using
     * the supplied value. The default alpha value is 1.0 (fully opaque), when not specified.
     *
     * @param alpha the constant alpha value to be used for rendering.
     *
     * @throws IllegalArgumentException if <code>alpha</code> is out of range.
     */
    public void setTransparency(float alpha)
    {
        iSg.setTransparency(alpha);
    }
}
