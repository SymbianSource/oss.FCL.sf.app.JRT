/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class is a placeholder of utility methods.
*
*/



package com.nokia.mid.ui;

import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;
import javax.microedition.lcdui.Font;

/**
 *  This class is a placeholder of utility methods. It contains
 *  methods for converting standard lcdui classes to Nokia UI classes
 *  and vice versa, and method for creating Image out of ARGB pixel
 *  array, creating empty transparent or specific
 *  background colored images, creating mutable image from encoded
 *  image byte array.
 *
 *  @see javax.microedition.lcdui.Graphics
 *  @see com.nokia.mid.ui.DirectGraphics
 *  @see javax.microedition.lcdui.Canvas
 *  @see com.nokia.mid.ui.DirectCanvas
 *  @version 0.11
 *  @since 1.0
 */
public class DirectUtils
{

    private DirectUtils()
    {
    }

    /**
    * Converts standard javax.microedition.lcdui.Graphics to DirectGraphics. The
    * returned Graphics context is the same, e.g. calling draw operations
    * via original Graphics reference affect the DirectGraphics context
    * and vice versa. Note that eventhough the graphics context is the same
    * the returned object reference may or may not be the same as passed.
    * This means that purely casting Graphics (g) passed in paint of lcdui
    * Canvas to DirectGraphics may not work ok. The safe way is to always
    * do the conversion with this method.
    *
    * @return the DirectGraphics object based on Graphics
    * @param g Graphics object for which DirectGraphics should be returned
    * @since 1.0
    */

    public static DirectGraphics getDirectGraphics(Graphics aG)
    {
        return com.nokia.mid.ui.DirectGraphicsInvoker.getDirectGraphics(aG);
    }

    /**
     *  Creates a mutable image which is decoded from the data stored
     *  in the specified byte array at the specified offset and length.
     *  The data must be in a self-identifying image file format
     *  supported by the implementation, such as PNG.
     *  <p>
     *  Note that the semantics of this method are exactly the same
     *  as
     *  {@link javax.microedition.lcdui.Image#createImage(byte[],int,int)}
     *  except that the returned image is mutable.
     *
     *  @param imageData the array of image data in a supported image format
     *  @param imageOffset the offset of the start of the data in the array
     *  @param imageLength the length of the data in the array
     *  @return the created mutable image
     *  @throws ArrayIndexOutOfBoundsException if imageOffset and
     *  imageLength specify an invalid range
     *  @throws NullPointerException if imageData is null
     *  @throws IllegalArgumentException if imageData is incorrectly
     *  formatted or otherwise cannot be decoded
     *  @see javax.microedition.lcdui.Image#createImage(byte[],int,int)
     *  @since 1.0
     */
    public static Image createImage(byte[] aImageData, int aImageOffset,
                                    int aImageLength)
    {
        Image source = Image.createImage(aImageData, aImageOffset, aImageLength);
        Image copy = Image.createImage(source.getWidth(), source.getHeight());
        DirectGraphics dg = getDirectGraphics(copy.getGraphics());
        DirectGraphicsInvoker.setColorValues(dg, 0x00FFFFFF, copy);
        dg.drawImage(source, 0, 0, 0, 0);
        return copy;
    }

    /**
     * <p>
     * The method will return a newly created mutable Image with specified
     * dimension with all pixels of an image of defined ARGB color. The color
     * can contain alpha channel transparency information.
     * </p>
     *
     * @param width the width of the new image, in pixels
     * @param height the height of the new image, in pixels
     * @return the created image
     * @throws IllegalArgumentException if either width or height is
     * zero or less
     * @since 1.0
     */
    public static Image createImage(int aWidth, int aHeight, int aColor)
    {
        Image result = Image.createImage(aWidth, aHeight);
        DirectGraphics dg = getDirectGraphics(result.getGraphics());
        DirectGraphicsInvoker.setColorValues(dg, aColor, result);
        return result;
    }

    /**
     * <p>
     * The method returns new instance of {@link javax.microedition.lcdui.Font}
     * with custom font height. System provides a font that matches
     * the requested attributes as closely as possible.
     * </p>
     * <p>
     * Font created in this way can be used only for Graphics instance
     * (Canvas, CustomItem, Image). This font is not supported
     * for high-level UI components (ChoiceGroup, StringItem and List).
     * If font with custom height is set to some high-level component, it's
     * replaced by default font.
     * </p>
     * <p>
     * Actual font height could be affected by system limitations, there may be
     * a maximum height defined by the system.
     * </p>
     *
     * @param face one of FACE_SYSTEM, FACE_MONOSPACE, or FACE_PROPORTIONAL
     * @param style STYLE_PLAIN, or a combination of STYLE_BOLD, STYLE_ITALIC,
     * and STYLE_UNDERLINED
     * @param height font height in pixels
     * @return new instance of Font
     * @throws IllegalArgumentException if height is negative, if face or style
     * are not legal values
     * @since 1.2
     */
    public static Font getFont(int face, int style, int height)
    {
        return com.nokia.mid.ui.FreeSizeFontInvoker.getFont(face, style, height);
    }
}
