/*******************************************************************************
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial implementation
 *******************************************************************************/

package org.eclipse.swt.internal.extension;

import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.internal.symbian.OS;
import org.eclipse.swt.widgets.Display;

public final class DisplayExtension extends Display 
{
    /* Image types, same values as MIDP */
    public static final int LIST_ELEMENT = 1;
    public static final int CHOICE_GROUP_ELEMENT = 2;
    public static final int ALERT = 3;

    public DisplayExtension() 
    {
        super();
    }

    /**
     * Determine the best width for the given image type.
     * 
     * @param imageType
     * @return Best height or -1 if invalid image type given.
     */
    public static int getBestImageWidth(int imageType) 
    {
        return getBestImageSize(imageType).x;
    }

    /**
     * Determine the best height for the given image type.
     * 
     * @param imageType
     * @return Best height or -1 if invalid image type given.
     */
    public static int getBestImageHeight(int imageType) 
    {
        return getBestImageSize(imageType).y;
    }

    private static Point getBestImageSize(int imageType) 
    {
        return OS.DisplayExtension_getBestImageSize(imageType);
    }

}
