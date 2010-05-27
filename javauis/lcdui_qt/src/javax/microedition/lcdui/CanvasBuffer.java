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

import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.internal.qt.graphics.GraphicsContext;
import org.eclipse.swt.widgets.Internal_PackageSupport;

final class CanvasBuffer extends Buffer
{

    private Canvas host;

    CanvasBuffer(Canvas canvas, Control ctrl)
    {
        super();
        setControlBounds(ctrl);
        host = canvas;
    }

    int getHostType()
    {
        return HOST_TYPE_CANVAS;
    }

    void setupWindowSurface()
    {
        if(super.windowSurface == null)
        {
            super.windowSurface = Internal_PackageSupport.getWindowsurface(host.getShell());
        }
    }

    Object getHost()
    {
        return host;
    }

    void bindToHost(GraphicsContext gc)
    {
        gc.bindTarget(super.windowSurface);
    }
}
