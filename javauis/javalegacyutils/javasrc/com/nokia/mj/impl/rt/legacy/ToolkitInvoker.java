/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.rt.legacy;

import java.util.Hashtable;

public abstract class ToolkitInvoker
{
    private static ToolkitInvoker sInvoker;

    protected static synchronized void setInvoker(ToolkitInvoker aInvoker)
    {
        if (null != sInvoker)
        {
            throw new RuntimeException("Toolkit invoker already exists");
        }
        sInvoker = aInvoker;
    }

    public static synchronized ToolkitInvoker getToolkitInvoker()
    {
        if (sInvoker == null)
        {
            try
            {
                // It's a static initializer of this class that installs
                // the concrete invoker object by calling setInvoker()
                Class.forName("javax.microedition.lcdui.ToolkitInvoker");
            }
            catch (ClassNotFoundException x)
            {
                throw new Error("Failed to load the toolkit class: " + x);
            }
        }
        return sInvoker;
    }

    public abstract void createToolkit(Hashtable args, boolean startInBackGround);

    /**
     * MIDP 2.0 Game support
     */

    //
    // Toolkit access
    //
    public abstract Object  getToolkit();

    /**
     * unsafe
     */
    public abstract int     toolkitGetHandle(Object aToolkit);

    public abstract void    toolkitDisposeObject(Object aToolkit,int aHandle);

    public abstract void    toolkitSync(Object aToolkit);

    /**
     * NGA specific change.
     * New variant for synchronization initiated from M3G
     * @param aToolkit -
     * @param aObject -
     * @since S60 9.2
     */
    public abstract void toolkitSync(Object aToolkit, Object aObject);

    /**
     * NGA specific change.
     * Checks if the NGA flag is set
     * @return true if the NGA flag is set
     * @since S60 9.2
     */
    public abstract boolean isNgaEnabled();


    /**
     * NGA specific change.
     * LCDUI's client APIs (e.g M3G) can check are they on foreground or not. 
     * @return <code>true</code> if MIDlet is on foreground.
     * @since S60 9.2
     */
    public abstract boolean isForeground();
    
    //
    // Image access
    //
    public abstract int     imageGetHandle(Object aImage);
    public abstract boolean imageIsMutable(Object aImage);
    public abstract boolean imageHasTransparency(Object aImage);

    /**
    Detect pixel level collision between two Images.
    @param aImage1                  sprite1 raw frame image
    @param aS1X1,aS1Y1,aS1X2,aS1Y2  sprite1 collision rect (clipped to frame rect) in sprite1 raw frame image co-ordinates
    @param aTransform1              sprite1 transform
    @param aD1X1, aD1Y1             position of sprite1 transformed collision rect (clipped to frame rect) in painter's co-ordinates
    @param aImage2                  sprite2\image\tiledlayer image
    @param aS2X1,aS2Y1,aS2X2,aS2Y2  sprite2\image\tiledlayer collision rect in image co-ordinates
    @param aTransform2              sprite2 transform. 0 if image\tiledlayer
    @param aD2X1, aD2Y1             position of collision rect in painter's coordinates
    */
    public abstract boolean imageCollision(Object aImage1,
                                           int aS1X1,int aS1Y1,int aS1X2,int aS1Y2,
                                           int aTransform1,
                                           int aD1X1, int aD1Y1,
                                           Object aImage2,
                                           int aS2X1,int aS2Y1,int aS2X2,int aS2Y2,
                                           int aTransform2,
                                           int aD2X1, int aD2Y1
                                          );

    //
    // Game Canvas support
    //
    public abstract Object  canvasInitGameCanvas(Object aCanvas, boolean aSuppressKeys);
    public abstract Object  canvasGetBufferGraphics(Object aCanvas);
    public abstract int     canvasGetKeyStates(Object aCanvas);
    public abstract int     canvasGetHandle(Object aCanvas);
    public abstract void    canvasRenderGraphics(Object aCanvas, Object aGraphics);
    public abstract void    canvasFlushGraphics(Object aCanvas, int aX, int aY, int aW, int aH);

    // Graphics
    /**
     * @return the MMIDGraphics handle for a graphics instance.
     *
     * WARNING: Unsupported. The validity of the return handle cannot be guaranteed. It should not be
     * dereferenced on anythread other than the UI thread on which the native graphics peer was instantiated.
     *
     */
    public abstract int     graphicsGetHandle(Object aGraphics);

    /**
     * @return the MMIDImage handle for a mutable image that is the target surface of
     * a graphics instance.
     *
     * WARNING: Unsupported. The validity of the return handle cannot be guaranteed. It should not be
     * dereferenced on anythread other than the UI thread on which the native graphics peer was instantiated.
     *
     */
    public abstract int     graphicsGetMutableImageHandle(Object aGraphics);

    /**
     * JSR 135 support
     */
    public abstract int getUIEventServerHandle();
    public abstract int itemGetHandle(Object aItem);
    public abstract void addObserver(Object aToolkit,ToolkitObserver aObserver);
    public abstract void removeObserver(Object aToolkit,ToolkitObserver aObserver);

    /** S60 AddOn for JSR135
     * Getter for Displayable's native handle.
     * Object parameter is used because runtime package
     * don't know Displayable class.
     * @param aDisplayable Displayable which handle is returned.
     * @return Native handle for displayable.
     */
    public abstract int getDisplayableHandle(Object aDisplayable);

    /**
     * Getter for Item's native handle.
     * Object parameter is used because runtime package
     * don't know Item class.
     * @param aItem Item which handle is returned.
     * @return Native handle for item.
     */
    public abstract int getItemHandle(Object aItem);

}
