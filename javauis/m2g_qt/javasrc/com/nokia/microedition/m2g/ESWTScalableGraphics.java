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

import javax.microedition.m2g.ScalableGraphics;
import javax.microedition.m2g.ScalableImage;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.*;

public class ESWTScalableGraphics
{
    Display iDisplay = null;
    Rectangle iCanvasBounds = null;
    ScalableGraphics iSg = null;
    GC iBufferGC = null;
    GC iRealGC = null;
    Image iBufferedImage = null;

    public ESWTScalableGraphics(Display display)
    {
				
				
        iSg = ScalableGraphics.createInstance();
        iDisplay = display;
    }

    public ESWTScalableGraphics(Display display, Rectangle canvasBounds)
    {
				
        iSg = ScalableGraphics.createInstance();
        iDisplay = display;
        iCanvasBounds = canvasBounds;
    }

    public void setBounds(Rectangle canvasBounds)
    {
        iCanvasBounds = canvasBounds;
    }

    public void bindTarget(GC gc)
    {
				
        iRealGC = gc;
        iBufferedImage = new Image(iDisplay, iCanvasBounds);
        iBufferGC = new GC(iBufferedImage);
        iSg.bindTarget(iBufferGC);
    }

    public void releaseTarget()
    {
    		

        iSg.releaseTarget();
        iBufferedImage.dispose();
        iBufferGC.dispose();
        /* iRealGC is disposed in paintControl */
        iRealGC = null;
    }

    public void render(int x, int y, ScalableImage image)
    {
    		
        iSg.render(x,y, image);
        iRealGC.drawImage(iBufferedImage, x, y);
    }

    public void setRenderingQuality(int mode)
    {
    		
        iSg.setRenderingQuality(mode);
    }

    public void setTransparency(float alpha)
    {
    		
        iSg.setTransparency(alpha);
    }
}
