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

import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.internal.qt.graphics.GraphicsContext;
import org.eclipse.swt.widgets.Control;

final class CustomItemBuffer extends Buffer
{

    private CustomItem host;

    CustomItemBuffer(CustomItem customItem, Control ctrl)
    {
        super();
        setControlBounds(ctrl);
        host = customItem;
    }

    int getHostType()
    {
        return HOST_TYPE_CUSTOMITEM;
    }

    void setupWindowSurface()
    {
        throw new IllegalStateException("not impl yet");
        // TODO Auto-generated method stub

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
