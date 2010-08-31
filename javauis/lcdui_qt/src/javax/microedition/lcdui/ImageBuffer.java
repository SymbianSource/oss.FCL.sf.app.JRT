/*
* Copyright (c) 2009,2010 Nokia Corporation and/or its subsidiary(-ies).
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

import org.eclipse.swt.graphics.Internal_GfxPackageSupport;
import org.eclipse.swt.internal.qt.graphics.GraphicsContext;
import org.eclipse.swt.widgets.Widget;

final class ImageBuffer extends Buffer
{

    private Image host;

    ImageBuffer(Image image)
    {
        host = image;
        setImageBounds(image.getWidth(), image.getHeight());
    }

    int getHostType()
    {
        return HOST_TYPE_IMAGE;
    }

    Object getHost()
    {
        return host;
    }

    void bindToHost(GraphicsContext gc)
    {
        gc.bindTarget(Internal_GfxPackageSupport.getImage(Image.getESWTImage(host)));
    }

    void blit(GraphicsContext gc, Widget widget) {
		// nothing to do 
	}

    void endPaint() {
		// nothing to do 
	}

	boolean isPaintingActive() {
		return false;
	}

	void beginPaint(int x, int y, int w, int h) {
		// nothing to do 
	}

}
