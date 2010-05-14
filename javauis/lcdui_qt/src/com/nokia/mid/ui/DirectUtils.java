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
package com.nokia.mid.ui;

import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;

import com.nokia.mj.impl.nokialcdui.LCDUIInvoker;

/**
 * DirectUtils class contains utility methods for lcdui classes.
 */
public final class DirectUtils
{

    private DirectUtils()
    {
    }

    /**
     * Create mutable image by decoding image data array.
     *
     * @param imageData data
     * @param imageOffset offset
     * @param imageLength length
     * @return mutable image
     */
    public static Image createImage(byte[] imageData, int imageOffset,
                                    int imageLength)
    {
        Image src = Image.createImage(imageData, imageOffset, imageLength);
        Image ret = Image.createImage(src.getWidth(), src.getHeight());
        ret.getGraphics().drawImage(src, 0, 0, 0);
        return ret;
    }

    /**
     * Create mutable image with specified dimension and color.
     *
     * @param width image width
     * @param height image height
     * @param argbColor fill color
     * @return mutable image
     */
    public static Image createImage(int width, int height, int argbColor)
    {
        Image ret = Image.createImage(width, height);
        Graphics g = ret.getGraphics();
        LCDUIInvoker.getDirectGraphics(g).setARGBColor(argbColor);
        g.fillRect(0, 0, width, height);
        return ret;
    }

    /**
     * Convert standard Graphics object to DirectGraphics.
     *
     * @param g graphics object
     * @return DirectGraphics object
     */
    public static DirectGraphics getDirectGraphics(Graphics g)
    {
        return LCDUIInvoker.getDirectGraphics(g);
    }

    /**
     * Request Font instance with custom font height.
     *
     * @param face font face
     * @param style font style
     * @param height font height
     * @return custom font
     */
    public static Font getFont(int face, int style, int height)
    {
        return LCDUIInvoker.getFreeSizedFont(face, style, height);
    }

}
