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


package javax.microedition.lcdui;

import javax.microedition.midlet.MIDlet;

import com.nokia.mj.impl.rt.legacy.ToolkitObserver;

import java.util.Hashtable;

final class ToolkitInvoker extends com.nokia.mj.impl.rt.legacy.ToolkitInvoker
{
    static
    {
        ToolkitInvoker.setInvoker(new ToolkitInvoker());
    }

    Toolkit iToolkit;
    Hashtable iMidletArgs;
    boolean iStartInBackGround;
    private ToolkitInvoker()
    {
    }

    void removeToolkit()
    {
        iToolkit = null;
    }

    Toolkit getCurrentToolkit()
    {
        if (null == iToolkit)
        {
            throw new RuntimeException("Toolkit closed");
        }
        return iToolkit;
    }

    public final void createToolkit(Hashtable aMidletArgs, boolean aStartInBackGround)
    {
        iStartInBackGround = aStartInBackGround;
        iMidletArgs = aMidletArgs;
        iToolkit = new Toolkit(this);
    }

    /**
     * MIDP 2.0 Game support
     */

    //
    // Toolkit access
    //
    public final Object getToolkit()
    {
        return getCurrentToolkit();
    }

    public final int toolkitGetHandle(Object aToolkit)
    {
        final Toolkit toolkit = (Toolkit)aToolkit;
        return toolkit.getHandle();
    }

    public final void toolkitDisposeObject(Object aToolkit,int aHandle)
    {
        final Toolkit toolkit = (Toolkit)aToolkit;
        toolkit.disposeObject(aHandle);
    }

    public final void toolkitSync(Object aToolkit)
    {
        final Toolkit toolkit = (Toolkit)aToolkit;
        toolkit.sync();
    }

    /**
     * NGA specific change.
     * Check if native side's NGA flag is set
     * @return True if NGA flag is on in native side
     * @since S60 9.2
     */
    public final boolean isNgaEnabled()
    {
        return getCurrentToolkit().checkFlags(Toolkit.FLAG_NGA);
    }

    /**
     * NGA specific change.
     * New variant for command buffer sync, used by M3G.
     * @param aToolkit Toolkit instance
     * @param aObject Graphics instance
     * @since S60 9.2
     */
    public final void toolkitSync(Object aToolkit, Object aObject)
    {
        final Toolkit toolkit = (Toolkit)aToolkit;
        toolkit.sync(aObject);
    }

    //
    // Image access
    //
    public final int imageGetHandle(Object aImage)
    {
        final int handle = ((Image)aImage).getHandle(true);
        if (handle == 0)
        {
            throw new RuntimeException("Toolkit closed");
        }
        return handle;
    }

    public final boolean imageIsMutable(Object aImage)
    {
        return ((Image)aImage).isMutable();
    }

    public final boolean imageHasTransparency(Object aImage)
    {
        return ((Image)aImage).hasTransparency();
    }

    public final boolean imageCollision
    (
        Object aImage1,
        int aS1X1, int aS1Y1, int aS1X2, int aS1Y2,
        int aTransform1, int aD1X, int aD1Y,
        Object aImage2,
        int aS2X1, int aS2Y1, int aS2X2, int aS2Y2,
        int aTransform2, int aD2X, int aD2Y
    )
    {
        Image image1 = (Image)aImage1;
        Image image2 = (Image)aImage2;
        return Image.collision(
                   image1, aS1X1, aS1Y1, aS1X2, aS1Y2, aTransform1, aD1X, aD1Y,
                   image2, aS2X1, aS2Y1, aS2X2, aS2Y2, aTransform2, aD2X, aD2Y
               );
    }

    //
    // GameCanvas support
    //
    public final Object canvasInitGameCanvas(Object aCanvas, boolean aSuppressKeys)
    {
        Canvas canvas = (Canvas)aCanvas;
        return canvas.initGameCanvas(aSuppressKeys);
    }

    public final Object canvasGetBufferGraphics(Object aCanvas)
    {
        Canvas canvas = (Canvas)aCanvas;
        return canvas.getBufferGraphics();
    }

    public final int canvasGetKeyStates(Object aCanvas)
    {
        Canvas canvas = (Canvas)aCanvas;
        return canvas.getGameKeyStates();
    }

    public final int canvasGetHandle(Object aCanvas)
    {
        return ((Canvas)aCanvas).getContentHandle();
    }

    public void canvasRenderGraphics(Object aCanvas, Object aGraphics)
    {
        Canvas canvas = (Canvas)aCanvas;
        Graphics graphics = (Graphics)aGraphics;
        Image frameBuffer = canvas.iFrameBuffer;
        // NGA specific change
        if (getCurrentToolkit().checkFlags(Toolkit.FLAG_NGA))
        {
            if (canvas.iFrameGraphics != graphics)
            {
                graphics.copyGraphics(canvas.iFrameGraphics);
            }
        }
        else if (frameBuffer != graphics.iImage)
        {
            graphics.drawImage(frameBuffer, 0, 0, Graphics.TOP|Graphics.LEFT);
        }
    }

    public void canvasFlushGraphics(Object aCanvas, int aX, int aY, int aW, int aH)
    {
        final Canvas  canvas  = (Canvas)aCanvas;
        final Toolkit toolkit = canvas.iToolkit;
        synchronized (toolkit)
        {
            canvas.flush(aX, aY, aW, aH);
        }
    }

    //
    // Graphics support
    //
    public final int graphicsGetHandle(Object aGraphics)
    {
        return ((Graphics)aGraphics).iHandle;
    }

    public final int graphicsGetMutableImageHandle(Object aGraphics)
    {
        Image image = ((Graphics)aGraphics).iImage;
        if (null == image) throw new RuntimeException();

        return image.getHandle(true);
    }

    /**
     JSR 135 Support
     */
    public final int getUIEventServerHandle()
    {
        return getCurrentToolkit().getEventServerHandle();
    }

    public final int itemGetHandle(Object aItem)
    {
        return ((Item)aItem).getHandle();
    }

    public final void addObserver(Object aToolkit, ToolkitObserver aObserver)
    {
        ((Toolkit)aToolkit).addObserver(aObserver);
    }

    public final void removeObserver(Object aToolkit,ToolkitObserver aObserver)
    {
        ((Toolkit)aToolkit).removeObserver(aObserver);
    }
    /** Getter for Displayable's native handle
     * Object parameter is used because runtime package
     * don't know Displayable class.
     * @param aDisplayable Displayable which handle is returned.
     * @return Native handle for displayable.
     */
    public int getDisplayableHandle(Object aDisplayable)
    {
        return ((Displayable)aDisplayable).iContent;
    }

    /**
     * Getter for Item's native handle
     * Object parameter is used because runtime package
     * don't know Item class.
     * @param aItem Item which handle is returned.
     * @return Native handle for item.
     */
    public int getItemHandle(Object aItem)
    {
        return ((Item)aItem).iHandle;
    }

    public int getImageHandle(Object aImage)
    {
        if (aImage == null)
        {
            return 0;
        }
        return ((Image)aImage).iHandle;
    }

    public int getGraphicsHandle(Object aGraphics)
    {
        if (aGraphics == null)
        {
            return 0;
        }
        return ((Graphics)aGraphics).iHandle;
    }
}

