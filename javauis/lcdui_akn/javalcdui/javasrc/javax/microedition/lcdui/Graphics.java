/*
* Copyright (c) 1999 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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

import javax.microedition.lcdui.game.Sprite;
import java.util.Enumeration;
import java.util.Hashtable;
import java.lang.Math;

import com.nokia.mj.impl.rt.legacy.NativeError;
import com.nokia.mid.ui.DirectUtils;
import com.nokia.mj.impl.rt.support.ApplicationInfo;
import com.nokia.mj.impl.rt.support.Finalizer;

/**
 * MIDP LCDUI API class providing methods to draw graphics primitives
 * on a Canvas, CustomItem or Image.
 *
 * Implementation writes commands into a (currently per-MIDlet) global
 * buffer. Each command is of the following form:
 *
 * [header][data]
 *
 * The header is a 32 bit integer divided into two 16 bit fields, the most
 * significant halfword contains the opcode. The opcode is further subdivided
 * into three fields:
 *
 * 15-12  11-10 9-0
 * [clear][mode][op]
 *
 * The top 4 bits are clear, although this may change in future, the top two bits
 * must always remain clear as these bits are used by the buffer itself.
 *
 * Bits 10 and 11 contain a mode identifier which is used by the native layer
 * to try to minimize the number of state changes to the native graphics context.
 *
 * Bits 0 to 9 contains an integer opcode. These must match the opcodes declared in
 * MMIDGraphics in lcdgr.h, and must remain contiguous.
 */
public class Graphics
{
    private static final int TRANS_MIN = javax.microedition.lcdui.game.Sprite.TRANS_NONE;
    private static final int TRANS_MAX = javax.microedition.lcdui.game.Sprite.TRANS_MIRROR_ROT90;

    /**
     * Pixel formats.
     *
     * These must be kept in sync with MMIDGraphics::TPixelType
     *
     * Only TYPE_INT_ARGB_8888 is currently used - in Graphics.drawRGB
     * The others may be useful in the implementation of extension APIs but
     * will require modifications to the MMIDGraphics implementation.
     */
    private static final int TYPE_INT_ARGB_8888       = 0;    // supported
    private static final int TYPE_SHORT_ARGB_4444 = 1;    // not supported yet
    private static final int TYPE_PACKED_ECOLOR_4K    = 2;    // not supported yet

    //
    // Anchor specs
    //
    public static final int HCENTER  = 1;
    public static final int VCENTER  = 2;
    public static final int LEFT     = 4;
    public static final int RIGHT    = 8;
    public static final int TOP      = 16;
    public static final int BOTTOM   = 32;
    public static final int BASELINE = 64;

    //
    // StrokeStyle consts
    //
    public static final int SOLID = 0;
    public static final int DOTTED = 1;

    //
    // Anchor masks
    //
    private static final int STRING_ANCHOR_MASK = HCENTER|LEFT|RIGHT|TOP|BOTTOM|BASELINE;
    private static final int IMAGE_ANCHOR_MASK  = HCENTER|VCENTER|LEFT|RIGHT|TOP|BOTTOM;
    private static final int HMASK              = HCENTER|LEFT|RIGHT;
    private static final int VMASK              = VCENTER|TOP|BOTTOM|BASELINE;

    //
    // These must be kept in sync with MMIDGraphics::TGraphicsOp
    //
    private static final int OP_SET_COLOR    = 0;
    private static final int OP_SET_STROKE   = 1;
    private static final int OP_SET_FONT     = 2;
    private static final int OP_SET_CLIPRECT = 3;
    private static final int OP_TRANSLATE    = 4;
    private static final int OP_RESET      = 5;

    private static final int OP_DRAW_LINE       = 6;
    private static final int OP_DRAW_RECT       = 7;
    private static final int OP_FILL_RECT       = 8;
    private static final int OP_DRAW_ROUNDRECT  = 9;
    private static final int OP_FILL_ROUNDRECT  = 10;
    private static final int OP_DRAW_ARC        = 11;
    private static final int OP_FILL_ARC      = 12;
    private static final int OP_DRAW_STRING     = 13;
    private static final int OP_DRAW_IMAGE      = 14;
    private static final int OP_DRAW_REGION     = 15;
    private static final int OP_COPY_AREA     = 16;
    private static final int OP_FILL_TRIANGLE   = 17;
    private static final int OP_DRAW_PIXELS       = 18;

    /* special ops */
    private static final int OP_FLUSH         = 19;

    private static final int OP_DRAW_BACKGROUND = 20;
    /* NGA specific change */
    private static final int OP_COPY_GRAPHICS   = 21;

    // Default color
    private static final int BLACK = 0xff000000;

    Font          iDefaultFont;

    //
    Toolkit        iToolkit;
    Buffer         iBuffer;
    Object         iTarget;
    Image          iImage;        // target is image
    int            iHandle;
    int            iDirectGraphicsHandle;
    int            iWidth;
    int            iHeight;
    private char[] iChar;     // Buffer for string/character drawing.

    // State
    private int       iTransX;
    private int       iTransY;
    private int       iClipX;
    private int       iClipY;
    private int       iClipWidth;
    private int       iClipHeight;

    private int     iColor;           // color value ARGB with 8 bits each
    private int     iStrokeStyle; // one of (SOLID,DOTTED)
    private Font    iFont;
    private Font    iScalableFont;
    private boolean iFontSet;
    private boolean iScalableFontSet;
    private boolean iM3Gdraw;

    private int iOnScreenWidth;  // width of screen
    private int iOnScreenHeight; // height of screen

    //flag if is original size set in JAD file
    private boolean iIsSetOriginalSize;
    // flag if set target size in JAD file
    private boolean iIsSetTargetSize;

    // size from Nokia-MIDlet-Canvas-Scaling-Orientation-Switch JAD attribute
    private int iTargetHeight;
    private int iTargetWidth;

    // NokiaUI DirectGraphics is stored here in order to allow a proper
    // garbage collection.
    DirectGraphicsImpl iDirectGraphics;

    private Finalizer mFinalizer = new Finalizer()
    {
        public void finalizeImpl()
        {
            doFinalize();
        }
    };

    /*
     * Constructor
     */
    Graphics(Toolkit aToolkit,
             Object aTarget,
             int aDrawable,
             int aWidth,
             int aHeight)
    {
        // saving of given parameters
        iToolkit = aToolkit;
        iBuffer = aToolkit.iBuffer;
        iTarget = aTarget;
        iM3Gdraw = false;
        iIsSetTargetSize = false;
        // If a Graphics is drawing to image then a image is saved. Null otherwise
        iImage = (aTarget instanceof Image) ? (Image)aTarget : null;

        // creating of native side graphics
        iHandle = NativeError.check(_create(aToolkit.getHandle(), aDrawable));

        // default values
        iChar = new char[1];
        iIsSetOriginalSize = false;
        iIsSetTargetSize = false;
        iDefaultFont = Font.getDefaultFont();

        // this get a correct iIsSetOriginalSize and iIsSetTargetSize values.
        //
        // getting JAD attributes
        ApplicationInfo appInfo = ApplicationInfo.getInstance();
        if (appInfo.getAttribute("Nokia-MIDlet-Original-Display-Size") != null)
        {
            iIsSetOriginalSize = true;
        }
        String targetSize = appInfo.getAttribute("Nokia-MIDlet-Target-Display-Size");
        if (targetSize != null)
        {
            // setting of iTargetHeight and iTargetWidth
            setTargetSize(targetSize);
        }

        // resenting of state of Graphics
        resetState(aWidth, aHeight);
    }

    /**
     * This method set iTargetHeight and iTargetWidth according to targetSize String.
     * @param targetSize string with Nokia-MIDlet-Target-Display-Size param
     */
    private void setTargetSize(String targetSize)
    {
        iIsSetTargetSize = false;
        String current = "";
        int indexString = 0;
        int indexArray = 0;

        // deleting of leading spaces
        targetSize = targetSize.trim();

        // loading of help string for iTargetWidth
        current += targetSize.charAt(indexString++);
        while ((indexString < targetSize.length()) && (Character.isDigit(targetSize.charAt(indexString))))
        {
            current += (targetSize.charAt(indexString++));
        }

        // If first next non-space char is not ',', input string is wrong.
        while (targetSize.charAt(indexString) == ' ')
        {
            indexString++;
        }
        if (targetSize.charAt(indexString++) != ',')
        {
            return;
        }

        // setting of iTargetWidth
        iTargetWidth = Integer.parseInt(current);

        // set to targetSize String for iTargetHeight without spaces
        targetSize = targetSize.substring(indexString).trim();
        indexString = 0;
        indexArray = 0;
        current = "";

        // loading of help string for iTargetHeight
        current += targetSize.charAt(indexString++);
        while ((indexString < targetSize.length()) && (Character.isDigit(targetSize.charAt(indexString))))
        {
            current += (targetSize.charAt(indexString++));
        }

        // setting of iTargetHeight
        iTargetHeight = Integer.parseInt(current);

        // Set of flag iIsSetTargetSize.
        iIsSetTargetSize = true;
    }

    void dispose()
    {
        final int handle;
        synchronized (iBuffer)
        {
            handle = iHandle;
            iHandle = 0;
        }
        if (0 != handle)
        {
            iToolkit.disposeObject(handle);
        }
    }

    private void doFinalize()
    {
        if (mFinalizer != null)
        {
            registeredFinalize();
            mFinalizer = null;
        }
    }

    void registeredFinalize()
    {
        dispose();
    }

    private void resetState(int aWidth, int aHeight)
    {
        // set of size of iTarget.
        iWidth       = aWidth;
        iHeight      = aHeight;
        // get of size of screen
        iToolkit.getDevice().getDeviceSizeWithoutKeypad();
        iOnScreenWidth = iToolkit.getDevice().iScreenWidth;
        iOnScreenHeight = iToolkit.getDevice().iScreenHeight;
        if (iOnScreenWidth == 0)
        {
            iOnScreenWidth = iWidth;
        }
        if (iOnScreenHeight == 0)
        {
            iOnScreenHeight = iHeight;
        }

        // reset of traslation, clip, color, stroke style and font
        iTransX        = 0;
        iTransY        = 0;
        iClipX         = 0;
        iClipY         = 0;
        iClipWidth   = aWidth;
        iClipHeight  = aHeight;
        iColor         = BLACK;
        iStrokeStyle = SOLID;
        iFont        = iDefaultFont;
        iFontSet   = false;
        iScalableFontSet = false;

        // If target is Canvas in fullscreen mode and
        // MIDlet have set original size and have't set target size,
        // then it is required set correct values of iOnScreenWidth and
        // iOnScreenHeight.
        if ((iTarget instanceof Canvas) &&
                ((Canvas)iTarget).getFullScreenMode() &&
                !iIsSetTargetSize && iIsSetOriginalSize)
        {
            // This code checks if some of measures is
            // bigger of target Canvas is bigger than
            // measure of screen.
            float ratioX = (float)iWidth / iOnScreenWidth;
            float ratioY = (float)iHeight / iOnScreenHeight;

            if (Math.max(ratioX, ratioY) > 1)
            {
                // Canvas has smaller ratio for width to for height than screen.
                if (ratioY > ratioX)
                {
                    // setting correct value of iOnScreenWidth
                    iOnScreenWidth = (iOnScreenHeight * iWidth) / iHeight;
                }
                // Canvas has bigger ratio for width to for height than screen
                // (or the same).
                else
                {
                    // setting correct value of iOnScreenHeight
                    iOnScreenHeight = (iOnScreenWidth * iHeight) / iWidth;
                }
            }
        }
        // setting of target size
        if ((iTarget instanceof Canvas) &&
                ((Canvas)iTarget).getFullScreenMode() &&
                iIsSetTargetSize && iIsSetOriginalSize)
        {
            iOnScreenHeight = iTargetHeight;
            iOnScreenWidth = iTargetWidth;
        }
    }

    void reset(int aWidth, int aHeight)
    {
        synchronized (iBuffer)
        {
            if (iHandle != 0)
            {
                resetState(aWidth, aHeight);
                iBuffer.write(iHandle, OP_RESET, aWidth, aHeight);
            }
        }
    }

    public void translate(int aDx,int aDy)
    {
        synchronized (iBuffer)
        {
            // If is downscaling on, then we change parameters.
            if (isDownscaled())
            {
                aDx = (aDx * iOnScreenWidth) / iWidth;
                aDy = (aDy * iOnScreenHeight) / iHeight;
            }
            // setting of translate in java class Graphics
            iTransX += aDx;
            iTransY += aDy;
            iClipX  -= aDx;
            iClipY  -= aDy;
            // setting of translate in native graphics
            iBuffer.write(iHandle, OP_TRANSLATE, iTransX, iTransY);
        }
    }

    public int getTranslateX()
    {
        // If is downscaling on, then it return downscaled traslate of x.
        if (isDownscaled())
            return (iTransX * iWidth) / iOnScreenWidth;
        // If is downscaling off, then it return non-scaled traslate of x.
        else
            return iTransX;
    }

    public int getTranslateY()
    {
        // If is downscaling on, then it return downscaled traslate of y.
        if (isDownscaled())
            return (iTransY * iHeight) / iOnScreenHeight;
        // If is downscaling off, then it return non-scaled traslate of y.
        else
            return iTransY;
    }

    public int getColor()
    {
        return (iColor & 0x00ffffff);
    }

    public int getRedComponent()
    {
        return (iColor >>> 16) & 0xff;
    }

    public int getGreenComponent()
    {
        return (iColor >>> 8) & 0xff;
    }

    public int getBlueComponent()
    {
        return (iColor & 0xff);
    }

    public int getGrayScale()
    {
        final int color = iColor;
        final int r = (color >>> 16) & 0xff;
        final int g = (color >>> 8) & 0xff;
        final int b = color & 0xff;
        return Math.max(r, Math.max(g,b));
    }

    public void setColor(int aRed,int aGreen,int aBlue)
    {
        /*
         * For all valid arguments, only the least significant
         * byte can contain non-zero bits.
         */
        if (((aRed|aGreen|aBlue)>>>8)!=0)
        {
            throw new IllegalArgumentException();
        }
        setColor((aRed << 16) | (aGreen << 8) | aBlue);
    }

    public void setColor(int aRgb)
    {
        //
        // Drawing colors are fully opaque.
        //
        final int color = (aRgb | 0xff000000);
        synchronized (iBuffer)
        {
            if (iColor != color)
            {
                iColor = color;
                iBuffer.write(iHandle, OP_SET_COLOR, color);
            }
        }
    }

    public void setGrayScale(int aValue)
    {
        setColor(aValue, aValue, aValue);
    }

    public Font getFont()
    {
        return iFont;
    }

    public void setFont(Font aFont)
    {
        if (aFont == null)
        {
            aFont = Font.getDefaultFont();
        }
        synchronized (iBuffer)
        {
            if (iFont != aFont)
            {
                iFont    = aFont;
                iFontSet = false;
                iScalableFontSet = false;
            }
        }
    }
    /**
     * If the downscaling is on, it is created and used the new scaled font.
     */
    private void setScalableFont()
    {
        if (!iScalableFontSet && iFont != null)
        {
            int fontHeight = iFont.getHeight();
            fontHeight = (fontHeight * iOnScreenHeight) / iHeight;
            iScalableFont = DirectUtils.getFont(iFont.getFace(), iFont.getStyle(), fontHeight);
            iFontSet = false;
            iScalableFontSet = false;
        }
    }

    /**
     * set font with buffer locked.
     */
    private void UseFont()
    {
        if (!iFontSet)
        {
            if (!isDownscaled())
            {
                iBuffer.write(iHandle, OP_SET_FONT, iFont.iHandle);
                iFontSet = true;
            }
            else if (!iScalableFontSet)
            {
                iBuffer.write(iHandle, OP_SET_FONT, iScalableFont.iHandle);
                iFontSet = true;
                iScalableFontSet = true;
            }
        }
    }

    public int getClipX()
    {
        return iClipX;
    }

    public int getClipY()
    {
        return iClipY;
    }

    public int getClipWidth()
    {
        return iClipWidth;
    }

    public int getClipHeight()
    {
        return iClipHeight;
    }

    public void clipRect(int aX, int aY, int aWidth,int aHeight)
    {
        // finding of correct clip
        final int cx2 = Math.min(iClipX + iClipWidth, aX + aWidth);
        final int cy2 = Math.min(iClipY + iClipHeight, aY + aHeight);
        // setting of clip to Java Graphics
        iClipX = Math.max(aX, iClipX);
        iClipY = Math.max(aY, iClipY);
        iClipWidth = cx2 - iClipX;
        iClipHeight = cy2 - iClipY;
        int clipX = iClipX;
        int clipY = iClipY;
        int clipWidth = iClipWidth;
        int clipHeight = iClipHeight;
        // If is downscaling on, then it downscales clip.
        if (isDownscaled())
        {
            clipX = (clipX * iOnScreenWidth) / iWidth;
            clipY = (clipY * iOnScreenHeight) / iHeight;
            clipWidth = (clipWidth * iOnScreenWidth) / iWidth;
            clipHeight = (clipHeight * iOnScreenHeight) / iHeight;
        }
        // Setting clip to native graphics
        synchronized (iBuffer)
        {
            iBuffer.write(iHandle, OP_SET_CLIPRECT, clipX, clipY, clipWidth, clipHeight);
            if (iDirectGraphicsHandle != 0)
                iBuffer.write(iDirectGraphicsHandle, OP_SET_CLIPRECT, clipX, clipY, clipWidth, clipHeight);
        }
    }

    public void setClip(int aX, int aY, int aWidth, int aHeight)
    {
        // setting of clip to Java Graphics
        iClipX = aX;
        iClipY = aY;
        iClipWidth = aWidth;
        iClipHeight = aHeight;
        int clipX = iClipX;
        int clipY = iClipY;
        int clipWidth = iClipWidth;
        int clipHeight = iClipHeight;
        // If is downscaling on, then it downscales clip.
        if (isDownscaled())
        {
            clipX = (clipX * iOnScreenWidth) / iWidth;
            clipY = (clipY * iOnScreenHeight) / iHeight;
            clipWidth = (clipWidth * iOnScreenWidth) / iWidth;
            clipHeight = (clipHeight * iOnScreenHeight) / iHeight;
        }
        synchronized (iBuffer)
        {
            // Setting clip to native graphics
            iBuffer.write(iHandle, OP_SET_CLIPRECT, clipX, clipY, clipWidth, clipHeight);
            if (iDirectGraphicsHandle != 0)
                iBuffer.write(iDirectGraphicsHandle, OP_SET_CLIPRECT, clipX, clipY, clipWidth, clipHeight);
        }
    }

    public void drawLine(int aX1, int aY1, int aX2, int aY2)
    {
        // If is downscaling on, then it downscales line's ends.
        if (isDownscaled())
        {
            aX1 = (aX1 * iOnScreenWidth) / iWidth;
            aY1 = (aY1 * iOnScreenHeight) / iHeight;
            aX2 = (aX2 * iOnScreenWidth) / iWidth;
            aY2 = (aY2 * iOnScreenHeight) / iHeight;
        }
        synchronized (iBuffer)
        {
            // Send line to native graphics
            iBuffer.write(iHandle, OP_DRAW_LINE, aX1, aY1, aX2, aY2);
        }
    }

    public void fillRect(int aX, int aY, int aWidth, int aHeight)
    {
        // If is downscaling on, then it downscales rectagle.
        if (isDownscaled())
        {
            aX = (aX * iOnScreenWidth) / iWidth;
            aY = (aY * iOnScreenHeight) / iHeight;
            /* Prevents bad drawing of thin rect.
               Rounding up the final rect after downscaling.
            */
            if (aWidth > aHeight)
            {
                aWidth = (aWidth * iOnScreenWidth) / iWidth;
                aHeight = (int)Math.ceil(((double)aHeight * iOnScreenHeight) / iHeight);
            }
            else if (aHeight > aWidth)
            {
                aWidth = (int)Math.ceil(((double)aWidth * iOnScreenWidth) / iWidth);
                aHeight = (aHeight * iOnScreenHeight) / iHeight;
            }
            else
            {
                aWidth = (int)Math.ceil(((double)aWidth * iOnScreenWidth) / iWidth);
                aHeight = (int)Math.ceil(((double)aHeight * iOnScreenHeight) / iHeight);
            }
        }
        synchronized (iBuffer)
        {
            // Send rectagle to native graphics.
            iBuffer.write(iHandle, OP_FILL_RECT, aX, aY, aWidth, aHeight);
        }
    }

    public void drawRect(int aX, int aY, int aWidth, int aHeight)
    {
        // If is downscaling on, then it downscales rectagle.
        if (isDownscaled())
        {
            aX = (aX * iOnScreenWidth) / iWidth;
            aY = (aY * iOnScreenHeight) / iHeight;
            aWidth = (aWidth * iOnScreenWidth) / iWidth;
            aHeight = (aHeight * iOnScreenHeight) / iHeight;
        }
        synchronized (iBuffer)
        {
            // Send rectagle to native graphics.
            iBuffer.write(iHandle, OP_DRAW_RECT, aX, aY, aWidth, aHeight);
        }
    }

    public void drawRoundRect(int aX, int aY, int aWidth, int aHeight, int aArcWidth, int aArcHeight)
    {
        // If is downscaling on, then it downscales rectagle and arches.
        if (isDownscaled())
        {
            aX = (aX * iOnScreenWidth) / iWidth;
            aY = (aY * iOnScreenHeight) / iHeight;
            aWidth = (aWidth * iOnScreenWidth) / iWidth;
            aHeight = (aHeight * iOnScreenHeight) / iHeight;
            aArcWidth = (aArcWidth * iOnScreenWidth) / iWidth;
            aArcHeight = (aArcHeight * iOnScreenHeight) / iHeight;
        }
        synchronized (iBuffer)
        {
            // Send rectagle and arches to native graphics.
            iBuffer.write(iHandle, OP_DRAW_ROUNDRECT, aX, aY, aWidth, aHeight, aArcWidth, aArcHeight);
        }
    }

    public void fillRoundRect(int aX, int aY, int aWidth, int aHeight, int aArcWidth, int aArcHeight)
    {
        // If is downscaling on, then it downscales rectagle and arches.
        if (isDownscaled())
        {
            aX = (aX * iOnScreenWidth) / iWidth;
            aY = (aY * iOnScreenHeight) / iHeight;

            /* Prevents bad drawing of thin rect.
               Rounding up the final rect after downscaling.
            */
            if (aWidth > aHeight)
            {
                aWidth = (aWidth * iOnScreenWidth) / iWidth;
                aHeight = (int)Math.ceil(((double)aHeight * iOnScreenHeight) / iHeight);
            }
            else if (aHeight > aWidth)
            {
                aWidth = (int)Math.ceil(((double)aWidth * iOnScreenWidth) / iWidth);
                aHeight = (aHeight * iOnScreenHeight) / iHeight;
            }
            else
            {
                aWidth = (int)Math.ceil(((double)aWidth * iOnScreenWidth) / iWidth);
                aHeight = (int)Math.ceil(((double)aHeight * iOnScreenHeight) / iHeight);
            }
            aArcWidth = (aArcWidth * iOnScreenWidth) / iWidth;
            aArcHeight = (aArcHeight * iOnScreenHeight) / iHeight;
        }
        synchronized (iBuffer)
        {
            // Send rectagle and arches to native graphics.
            iBuffer.write(iHandle, OP_FILL_ROUNDRECT, aX, aY, aWidth, aHeight, aArcWidth, aArcHeight);
        }
    }

    public void fillArc(int aX, int aY, int aWidth, int aHeight, int aStartAngle, int aArcAngle)
    {
        // If is downscaling on, then it downscales centre and measures.
        if (isDownscaled())
        {
            aX = (aX * iOnScreenWidth) / iWidth;
            aY = (aY * iOnScreenHeight) / iHeight;
            /* Prevents bad drawing of thin rect.
               Rounding up the final rect after downscaling.
            */
            if (aWidth > aHeight)
            {
                aWidth = (aWidth * iOnScreenWidth) / iWidth;
                aHeight = (int)Math.ceil(((double)aHeight * iOnScreenHeight) / iHeight);
            }
            else if (aHeight > aWidth)
            {
                aWidth = (int)Math.ceil(((double)aWidth * iOnScreenWidth) / iWidth);
                aHeight = (aHeight * iOnScreenHeight) / iHeight;
            }
            else
            {
                aWidth = (int)Math.ceil(((double)aWidth * iOnScreenWidth) / iWidth);
                aHeight = (int)Math.ceil(((double)aHeight * iOnScreenHeight) / iHeight);
            }
        }
        synchronized (iBuffer)
        {
            // Send arch to native graphics.
            iBuffer.write(iHandle, OP_FILL_ARC, aX, aY, aWidth, aHeight, aStartAngle, aArcAngle);
        }
    }

    public void drawArc(int aX, int aY, int aWidth, int aHeight, int aStartAngle, int aArcAngle)
    {
        // If is downscaling on, then it downscales centre and measures.
        if (isDownscaled())
        {
            aX = (aX * iOnScreenWidth) / iWidth;
            aY = (aY * iOnScreenHeight) / iHeight;
            aWidth = (aWidth * iOnScreenWidth) / iWidth;
            aHeight = (aHeight * iOnScreenHeight) / iHeight;
        }
        synchronized (iBuffer)
        {
            // Send arch to native graphics.
            iBuffer.write(iHandle, OP_DRAW_ARC, aX, aY, aWidth, aHeight, aStartAngle, aArcAngle);
        }
    }

    public void drawString(String aString, int aX, int aY, int aAnchor)
    {
        // If is downscaling on, then it downscales drawing point.
        if (isDownscaled())
        {
            aX = (aX * iOnScreenWidth) / iWidth;
            aY = (aY * iOnScreenHeight) / iHeight;
            setScalableFont();
        }
        final int length = aString.length();
        synchronized (iBuffer)
        {
            // checking of anchor or setting default
            aAnchor = checkAnchor(aAnchor, STRING_ANCHOR_MASK);
            // set a current font for use
            UseFont();
            // Send text, drawing point and anchor to native graphics.
            iBuffer.writeStr(iHandle, OP_DRAW_STRING, aString, 0, length, aX, aY, aAnchor);
        }
    }

    public void drawSubstring(String aString, int aOffset, int aLength, int aX, int aY, int aAnchor)
    {
        // If is downscaling on, then it downscales drawing point.
        if (isDownscaled())
        {
            aX = (aX * iOnScreenWidth) / iWidth;
            aY = (aY * iOnScreenHeight) / iHeight;
            setScalableFont();
        }
        synchronized (iBuffer)
        {
            // checking of anchor or setting default
            aAnchor = checkAnchor(aAnchor, STRING_ANCHOR_MASK);

            // Checking if offset and length are correct.
            final int stringLength = aString.length();
            final int beg = aOffset;
            final int end = aOffset + aLength;
            if ((beg < 0) || (end > stringLength) || (end < beg))
            {
                throw new StringIndexOutOfBoundsException();
            }

            // set a current font for use
            UseFont();
            // Send text, drawing point and anchor to native graphics.
            iBuffer.writeStr(iHandle, OP_DRAW_STRING, aString, aOffset, aLength, aX, aY, aAnchor);
        }
    }

    public void drawChar(char aChar, int aX, int aY, int aAnchor)
    {
        if (isDownscaled())
        {
            aX = (aX * iOnScreenWidth) / iWidth;
            aY = (aY * iOnScreenHeight) / iHeight;
        }
        // Calling of drawChars with array with size 1.
        iChar[0] = aChar;
        DrawChars(iChar, 0, 1, aX, aY, aAnchor);
    }

    public void drawChars(char[] aChars,int aOffset,int aLength,int aX,int aY,int aAnchor)
    {
        // If is downscaling on, then it downscales drawing point.
        if (isDownscaled())
        {
            aX = (aX * iOnScreenWidth) / iWidth;
            aY = (aY * iOnScreenHeight) / iHeight;
        }

        synchronized (iBuffer)
        {
            // Checking if offset and length are correct.
            final int charLength = aChars.length; // throws NullPointerException
            final int beg = aOffset;
            final int end = aOffset + aLength;
            if ((beg < 0) || (end > charLength) || (end < beg))
            {
                throw new ArrayIndexOutOfBoundsException();
            }
            // Calling of DrawChars.
            DrawChars(aChars, aOffset, aLength, aX, aY, aAnchor);
        }
    }

    private void DrawChars(char[] aChars,int aOffset,int aLength,int aX,int aY,int aAnchor)
    {
        // checking of anchor or setting default
        aAnchor = checkAnchor(aAnchor, STRING_ANCHOR_MASK);
        if (isDownscaled())
        {
            setScalableFont();
        }
        // set a current font for use
        UseFont();
        // Send text, drawing point and anchor to native graphics.
        iBuffer.writeChars(iHandle, OP_DRAW_STRING, aChars, aOffset, aLength, aX, aY, aAnchor);
    }

    public void drawImage(Image aImage, int aX, int aY, int aAnchor)
    {
        // check if iTarget is downscaled
        if (isDownscaled())
        {
            // It is downscaled.
            // downscaling drawing point
            aX = (aX * iOnScreenWidth) / iWidth;
            aY = (aY * iOnScreenHeight) / iHeight;

            // getting size of aImage
            int widthL = aImage.getWidth();
            int heightL = aImage.getHeight();

            // create RGB array for aImage.
            int rgbL[] = new int[widthL * heightL];

            // getting a RGB data from aImage
            aImage.getRGB(rgbL, 0, widthL, 0, 0, widthL, heightL);

            // downscaling size of aImage
            int widthSL = (widthL * iOnScreenWidth) / iWidth;
            int heightSL = (heightL * iOnScreenHeight) / iHeight;

            // create RGB array for downscaled image
            int rgbSL[] = new int[widthSL * heightSL];

            // getting downscaled rgb data
            getDownscaledRGB(rgbSL, rgbL, widthL, heightL);

            // creating a image from downscaled RGB data
            Image image = Image.createRGBImage(rgbSL, widthSL, heightSL, true);
            synchronized (iBuffer)
            {
                // checking of anchor
                aAnchor = checkAnchor(aAnchor, IMAGE_ANCHOR_MASK);
                // send downscaled image with downscaled drawing point
                // to native side
                iBuffer.write(iHandle, OP_DRAW_IMAGE, image.iHandle, aX, aY, aAnchor);
            }
        }
        else
        {
            // It is not downscaled.
            synchronized (iBuffer)
            {
                // checking of anchor
                aAnchor = checkAnchor(aAnchor, IMAGE_ANCHOR_MASK);

                // Prevent aImage to be disposed when in use
                synchronized (aImage)
                {
                    // Check that aImage haven't been disposed yet
                    if (aImage.iHandle != 0)
                    {
                        // send image with drawing point to native side
                        iBuffer.write(iHandle, OP_DRAW_IMAGE, aImage.iHandle, aX, aY, aAnchor);
                    }
                }
            }
        }
    }

    /**
     * Sets the copy-graphics command into the command buffer.
     * @param aSrcGraphics The source graphics
     * @since s60 9.2
     */
    void copyGraphics(Graphics aSrcGraphics)
    {
        synchronized (iBuffer)
        {
            iBuffer.write(iHandle, OP_COPY_GRAPHICS, aSrcGraphics.iHandle);
        }
    }

    public void setStrokeStyle(int aStyle)
    {
        if ((aStyle != SOLID) && (aStyle != DOTTED))
        {
            throw new IllegalArgumentException();
        }
        synchronized (iBuffer)
        {
            iStrokeStyle = aStyle;
            iBuffer.write(iHandle, OP_SET_STROKE, aStyle);
        }
    }

    public int getStrokeStyle()
    {
        return iStrokeStyle;
    }

    /**
     * Check that aAnchor contains a valid anchor spec.
     * Using zero for the anchor point value gives results identical to using TOP | LEFT.
     * The definition of the anchor point must be one of the horizontal constants
     * (LEFT, HCENTER, RIGHT) combined with one of the vertical constants (TOP, BASELINE, BOTTOM)
     */
    private static int checkAnchor(int aAnchor, int aMask)
    {
        if (0 == aAnchor)
        {
            aAnchor = LEFT|TOP;
            return aAnchor;
        }

        final boolean excess = (aAnchor & ~aMask) != 0;
        final int horz = aAnchor & HMASK;
        final int vert = aAnchor & VMASK;

        if (excess || multipleBits(vert) || multipleBits(horz))
        {
            throw new IllegalArgumentException();
        }
        if (horz == 0 || vert == 0)
        {
            throw new IllegalArgumentException("anchor is not a legal value");
        }

        return aAnchor;
    }

    /**
     *@return true if aAnchor contains more than one bit set.
     */
    private static boolean multipleBits(int aAnchor)
    {
        return (aAnchor & (aAnchor-1)) != 0;
    }

    // New in MIDP 2.0
    public int getDisplayColor(int aColor)
    {
        int i = 0;
        synchronized (iToolkit)
        {
            i = _getDisplayColor(iToolkit.getHandle(), iHandle, aColor);
        }
        return i;
    }

    //
    // New in MIDP 2.0
    //
    // Semantics of negative width/height are not defined in the spec
    // but could be used to implement reflections.
    //
    public void drawRGB(
        int[]   aRgbData,
        int     aOffset,
        int     aScanLength,
        int     aX,
        int     aY,
        int     aWidth,
        int     aHeight,
        boolean aProcessAlpha)
    {
        // setting of help variables
        int len = aRgbData.length;
        int width = aWidth >= 0 ? aWidth : -aWidth;
        int height = aHeight >= 0 ? aHeight : -aHeight;

        // get RGB array for drawing
        int rgbSL[] = aRgbData;

        // This checks if downscaling is on.
        if (isDownscaled())
        {
            // dowscaling of drawing point and size of image
            aX = (aX * iOnScreenWidth) / iWidth;
            aY = (aY * iOnScreenHeight) / iHeight;
            int widthSL = (width * iOnScreenWidth) / iWidth;
            int heightSL = (height * iOnScreenHeight) / iHeight;

            // set drawing array to new array with downscaled size
            rgbSL = new int[widthSL * heightSL];

            // preparing of aRgbData to use getDownscaledRGB method
            aRgbData = cleanRGB(aRgbData, aOffset, aScanLength, width, height);

            // downscaling of RGB data
            getDownscaledRGB(rgbSL, aRgbData, width, height);

            //setting parameters do downscaled values
            aWidth = widthSL;
            aHeight = heightSL;
            aOffset = 0;
            aScanLength = widthSL;
        }
        int scanLength;
        int transform;

        long end64       = (long)len - 1;
        long offset64;
        long scanLength64;


        if (aScanLength >= 0)
        {
            // If aScanLength is positive, we can use RGB data forward.
            scanLength = aScanLength;
            transform  = Sprite.TRANS_NONE;
            offset64   = (long)aOffset;
        }
        else
        {
            // If aScanlength is negative, we have to use RGB data backward.
            scanLength = -aScanLength;
            transform  = Sprite.TRANS_MIRROR_ROT180;  // relection in horizontal axis
            offset64 = (long)aOffset - (long)scanLength * ((long)height - 1);
        }

        scanLength64 = (long)scanLength;
        long min64 = offset64;
        long max64 = offset64 + scanLength64 * ((long)height - 1) + (long)width - 1;

        // They are given incorrect data.
        if ((min64 < 0) || (max64 > end64))
        {
            throw new ArrayIndexOutOfBoundsException();
        }


        //
        // IMPROVEMENT: clip down the data to transfer.
        //
        int x1 = aX;
        int y1 = aY;
        int x2 = aX + aWidth;
        int y2 = aY + aHeight;

        int clipX2 = iClipX+iClipWidth;
        int clipY2 = iClipY+iClipHeight;

        x1 = iClipX > x1 ? iClipX : x1;
        y1 = iClipY > y1 ? iClipY : y1;
        x2 = clipX2 < x2 ? clipX2 : x2;
        y2 = clipY2 < y2 ? clipY2 : y2;

        width = x2 - x1;
        height = y2 - y1;

        //
        // This will also clip out any attempt
        // to draw with negative width/height.
        //
        if ((width<=0) || (height<=0))
        {
            return;
        }

        //
        // move offset to start of clipped region, note we use top-left corner here
        // because we have already regularized the scanlength : the tranform takes
        // care of vertical flip for negative scan case.
        //
        offset64 += scanLength64 * (long)(y1 - aY) + (long)(x1 - aX);
        //
        // adjust length
        //
        long length64 = ((long)height - 1L) * scanLength64 + (long)width;
        final int alpha = aProcessAlpha ? 1 : 0;

        synchronized (iBuffer)
        {
            //
            // ensure pending drawing commands are complete first
            //
            iBuffer.sync();

            //
            // Blocking server call to do Bitblt server side:
            //
            NativeError.check(_drawPixels(
                                  iBuffer.getHandle(),
                                  iHandle,
                                  TYPE_INT_ARGB_8888,
                                  rgbSL,
                                  (int)offset64,
                                  (int)length64,
                                  (int)scanLength64,
                                  aProcessAlpha,
                                  x1,
                                  y1,
                                  width,
                                  height,
                                  transform
                              ));
        }
    }

    public void fillTriangle(int aX1, int aY1, int aX2, int aY2, int aX3, int aY3)
    {
        if (isDownscaled())
        {
            // If is downscaling on, then it downscales triangle.
            aX1 = (aX1 * iOnScreenWidth) / iWidth;
            aY1 = (aY1 * iOnScreenHeight) / iHeight;
            aX2 = (aX2 * iOnScreenWidth) / iWidth;
            aY2 = (aY2 * iOnScreenHeight) / iHeight;
            aX3 = (aX3 * iOnScreenWidth) / iWidth;
            aY3 = (aY3 * iOnScreenHeight) / iHeight;
        }
        synchronized (iBuffer)
        {
            // Send triangle to native graphics.
            iBuffer.write(iHandle, OP_FILL_TRIANGLE, aX1, aY1, aX2, aY2, aX3, aY3);
        }
    }

    // New in MIDP 2.0
    public void copyArea(int aXsrc,int aYsrc,int aWidth,int aHeight,int aXdest,int aYdest,int aAnchor)
    {
        synchronized (iBuffer)
        {
            if (null == iImage)
            {
                throw new IllegalStateException("cannot copyArea on display");
            }

            /*
             * Silently ignore - even if the source point is out of bounds.
             */
            if (aWidth <= 0 || aHeight <= 0)
            {
                return;
            }

            final int sx1 = aXsrc;
            final int sy1 = aYsrc;
            final int sx2 = aXsrc + aWidth;
            final int sy2 = aYsrc + aHeight;

            //
            // (sx1,sy1,sx2,sy2) source rectangle is inclusive-exclusive
            //
            // Top left coordinate must be contained within drawing surface.
            //
            // Bottom right coordinate may extend one pixel to right or below
            // drawing surface bottom right coordinate.
            //
            if ((sx1<0) || (sx1>=iWidth) || (sy1<0) || (sy1>=iHeight) ||
                    (sx2<0) || (sx2>iWidth)  || (sy2<0) || (sy2>iHeight))
            {
                throw new IllegalArgumentException("copyArea source rect out of bounds");
            }

            checkAnchor(aAnchor, IMAGE_ANCHOR_MASK);


            iBuffer.write(
                iHandle,
                OP_COPY_AREA,
                aXsrc,
                aYsrc,
                aWidth,
                aHeight,
                aXdest,
                aYdest,
                aAnchor
            );
        }
    }

    public void drawRegion(
        Image aImage,
        int aXsrc,
        int aYsrc,
        int aWidth,
        int aHeight,
        int aTransform,
        int aXdest,
        int aYdest,
        int aAnchor)
    {
        // giving a size of aImage
        int srcWidth = aImage.getWidth();
        int srcHeight = aImage.getHeight();

        // If drawing area is empty, then return.
        if ((aWidth <= 0) || (aHeight <= 0))
        {
            return;
        }

        // Drawing from image to the same image is not allowed.
        if (iTarget == aImage)
        {
            throw new IllegalArgumentException("Cannot draw image onto itself");
        }

        // checking if drawing area is correct
        int sx1 = aXsrc;
        int sy1 = aYsrc;
        int sx2 = aXsrc + aWidth;
        int sy2 = aYsrc + aHeight;
        if (sx1 < 0 || sy1 < 0 || sx2 > srcWidth || sy2 > srcHeight)
        {
            throw new IllegalArgumentException();
        }

        // checking of anchor
        checkAnchor(aAnchor, IMAGE_ANCHOR_MASK);

        // checking of legality of trasformation
        if (aTransform < TRANS_MIN || aTransform > TRANS_MAX)
        {
            throw new IllegalArgumentException("Transform");
        }

        // checking if downscaling is on
        if (isDownscaled())
        {
            // downscaling of drawing point and area
            aXdest = (aXdest * iOnScreenWidth) / iWidth;
            aYdest = (aYdest * iOnScreenHeight) / iHeight;
            aWidth = (aWidth * iOnScreenWidth) / iWidth;
            aHeight = (aHeight * iOnScreenHeight) / iHeight;

            // getting of size of non-scaled image
            int widthL = aImage.getWidth();
            int heightL = aImage.getHeight();

            // array for RGB data from non-scaled image
            int rgbL[] = new int[widthL * heightL];

            // getting RGB data
            aImage.getRGB(rgbL, 0, widthL, 0, 0, widthL, heightL);

            // downscaling of size of image
            int widthSL = (widthL * iOnScreenWidth) / iWidth;
            int heightSL = (heightL * iOnScreenHeight) / iHeight;

            // array for RGB data for scaled image
            int rgbSL[] = new int[widthSL * heightSL];

            // downscaling RGB data
            getDownscaledRGB(rgbSL, rgbL, widthL, heightL);

            // creating image from scaled RGB data
            Image image = Image.createRGBImage(rgbSL, widthSL, heightSL, true);

            // scaling  of size of size of image
            aXsrc = ((widthSL * aXsrc) / widthL);
            aYsrc = ((heightSL * aYsrc) / heightL);

            //
            // finding of new drawing start point in source image
            // This is the same algorithm like in getDownscaledRGB method.
            //
            int srcXL = widthSL;
            int srcYL = heightSL;
            int residueWidth = widthL;
            int residueHeight = heightL;
            for (int i = 0; i < widthSL; i++)
            {
                residueWidth -= (int)((double)((float)residueWidth /
                                               (float)(widthSL - i)) + 0.5D);
                if (widthL - residueWidth > aXsrc)
                    break;
            }

            for (int j = 0; j < heightSL; j++)
            {
                residueHeight -= (int)((double)((float)residueHeight /
                                                (float)(heightSL - j)) + 0.5D);
                if (heightL - residueHeight > aYsrc)
                    break;
            }

            synchronized (iBuffer)
            {
                // sending data to native side
                iBuffer.write(iHandle,
                              OP_DRAW_REGION,
                              image.iHandle,
                              widthL - residueWidth,
                              heightL - residueHeight,
                              aWidth, aHeight,
                              aTransform,
                              aXdest,
                              aYdest,
                              aAnchor);
            }
        }
        else
        {
            synchronized (iBuffer)
            {
                // Prevent aImage to be disposed when in use
                synchronized (aImage)
                {
                    // Check that aImage haven't been disposed yet
                    if (aImage.iHandle != 0)
                    {
                        // If downscaling off, 
                        // then it only send data to native side
                        iBuffer.write(iHandle,
                                      OP_DRAW_REGION,
                                      aImage.iHandle,
                                      aXsrc,
                                      aYsrc,
                                      aWidth,
                                      aHeight,
                                      aTransform,
                                      aXdest,
                                      aYdest,
                                      aAnchor);
                    }
                }
            }
        }
    }

    void flush(int aX, int aY, int aWidth, int aHeight)
    {
        if (isDownscaled())
        {
            // If downscaling on, then it downscales flushed area.
            aX = (aX * iOnScreenWidth) / iWidth;
            aY = (aY * iOnScreenHeight) / iHeight;
            aWidth = (aWidth * iOnScreenWidth) / iWidth;
            aHeight = (aHeight * iOnScreenHeight) / iHeight;
        }
        synchronized (iBuffer)
        {
            // sending data to native side
            iBuffer.write(iHandle, OP_FLUSH, aX, aY, aWidth, aHeight);
            iBuffer.sync();
        }
    }

    /**
     * Send draw background command to native side
     * @param aDisplayable The target Displayable object
     * @param aX X position
     * @param aY Y position
     * @param aWidth Width
     * @param aHeight Height
     * @since S60 5.0
     */
    void drawBackground(Displayable aDisplayable,
                        int aX,
                        int aY,
                        int aWidth,
                        int aHeight)
    {
        // If aDisplayable is not Canvas or a Displayable is null,
        // then drawBackround do nothing.
        if (aDisplayable == null || !(aDisplayable instanceof Canvas))
            return;
        if (isDownscaled())
        {
            // If downscaling on, then it downscales flushed area.
            aX = (aX * iOnScreenWidth) / iWidth;
            aY = (aY * iOnScreenHeight) / iHeight;
            aWidth = (aWidth * iOnScreenWidth) / iWidth;
            aHeight = (aHeight * iOnScreenHeight) / iHeight;
        }
        synchronized (iBuffer)
        {
            // sending data to native side
            iBuffer.write(iHandle, 20, aDisplayable.getContentHandle(), aX, aY, aWidth, aHeight);
        }
    }

    /**
     *this function blocking downscaled when M3G drawing some content    
     **/    
    void M3Gdraw(int aM3Gdraw)
    {
       if (aM3Gdraw == 0) 
          {
             iM3Gdraw = false;
          }
       else
       { 
          iM3Gdraw = true; 
       }
    }
    /**
     * This function return flag if Graphics is downscaled.
     * Graphics is downscaled on if we have set original size, this size is bigger then screen
     * (at least on one measure) and iTarget is Canvas in full screen Canvas or a target size is
     * smaller than original size.
     * @return true if Graphics is downscaled, false otherwise
     */ 
    boolean isDownscaled()
    {
       // If M3G is drawnig then downscaling is turn off.
       if (iM3Gdraw)
          { 
             return false;
          }
        if ((iTarget instanceof Canvas) && ((Canvas)iTarget).getFullScreenMode()
                && iIsSetOriginalSize)
        {
            return iOnScreenHeight < iHeight || iOnScreenWidth < iWidth ;
        }
        return false;
    }

    /**
     * This function returns downscaled GRB data from input RGB data. It scales
     * according iOnScreenWidth, iOnScreenHeight, iWidth and iHeight.
     * @param aRgbS array for output RGB data. This have to be defined with needed lenght.
     * @param aRgb input data
     * @param aWidth width of aRgb
     * @param aHeight height of aRgb
     */
    private void getDownscaledRGB(int aRgbS[], int aRgb[], int aWidth, int aHeight)
    {
        // downscaling of size of aRgb
        int widthS = (aWidth * iOnScreenWidth) / iWidth;
        int heightS = (aHeight * iOnScreenHeight) / iHeight;

        // help variables for remaining columns and rows
        int residueWidth = aWidth;
        int residueHeight = aHeight;

        // help variables for count of columns and rows of non-scaled RGB data
        // scaled to one pixel of downscaled image
        int columnWidths[] = new int[widthS];
        int rowHeights[] = new int[heightS];

        // If aRgbS has lenght shorter then needed.
        if (aRgbS.length < widthS * heightS)
            throw new IllegalArgumentException();

        // computing of count of columns of non-scaled RGB data
        // scaled to one column of downscaled image
        for (int i = 0; i < widthS; i++)
        {
            columnWidths[i] = (int)((double)((float)residueWidth /
                                             (float)(widthS - i)) + 0.5D);
            residueWidth -= columnWidths[i];
        }

        // computing of count of rows of non-scaled RGB data
        // scaled to one row of downscaled image
        for (int i = 0; i < heightS; i++)
        {
            rowHeights[i] = (int)((double)((float)residueHeight /
                                           (float)(heightS - i)) + 0.5D);
            residueHeight -= rowHeights[i];
        }

        // It goes through all columns of output image.
        int posX = 0;
        for (int i = 0; i < widthS; i++)
        {
            // getting of number of columns which will be scaled to one column
            int w = columnWidths[i];
            int posY = 0;
            // It goes through all rows of output image.
            for (int j = 0; j < heightS; j++)
            {
                // getting of number of rows which will be scaled to one row
                int h = rowHeights[j];

                if (w == 1 && h == 1)
                {
                    // The recalculating of this pixel is not needed.
                    aRgbS[j * widthS + i] = aRgb[posY * aWidth + posX];
                    posY++;
                }
                else
                {
                    // The recalculating of this pixel is needed.

                    // We have to use char because char is only one
                    // unsigned integer type in Java.
                    char sumRed = '\0';
                    char sumGreen = '\0';
                    char sumBlue = '\0';
                    char sumAlpha = '\0';

                    // It goes through columns of output image,
                    // what will be downscaled to one column.
                    for (int k = 0; k < w; k++)
                    {
                        // It goes through rows of output image,
                        // what will be downscaled to one row.
                        for (int l = 0; l < h; l++)
                        {
                            // getting of current pixel
                            int pixel = aRgb[(posY + l) * aWidth + (posX + k)];

                            // adding values of one pixel
                            sumRed += (pixel & 0xff0000) >> 16;
                            sumGreen += (pixel & 0xff00) >> 8;
                            sumBlue += pixel & 0xff;
                            sumAlpha += (pixel & 0xff000000) >> 24;
                        }

                    }

                    // calculating of new pixel
                    aRgbS[j * widthS + i] = sumRed / (w * h) << 16 |
                                            sumGreen / (w * h) << 8 |
                                            sumBlue / (w * h)|
                                            sumAlpha / (w * h) << 24;

                    // new set of rows
                    posY += h;
                }
            }

            // new set of columns
            posX += w;
        }

    }

    /**
     * This function returns array with the same RGB data as input,
     * but with scanlenght > 0 and offset = 0.
     * @param aRgb input RGB data
     * @param aOffset offset of aRgb
     * @param aScanlenght scalenght of aRgb
     * @param aWidth width of aRgb
     * @param aHeight height of aRgb
     */
    private int[] cleanRGB(int aRgb[],
                           int aOffset,
                           int aScanlenght,
                           int aWidth,
                           int aHeight)
    {
        // creating output array
        int output[] = new int[aWidth * aHeight];

        // resorting of input array to output array
        for (int i = 0; i < aHeight; i++)
        {
            for (int j = 0; j < aWidth; j++)
            {
                output[i * aWidth + j] = aRgb[aOffset + i * aScanlenght + j];
            }
        }

        // return of resorted array
        return output;
    }

    private native int _create(int i, int j);

    private static native int _drawPixels
    (
        int aBuffer,
        int aHandle,
        int aType,
        int[] aArray,
        int aOffset,
        int aLength,
        int aScanLength,
        boolean aProcessAlpha,
        int aX,
        int aY,
        int aWidth,
        int aHeight,
        int aTransform
    );

    private static native int _getDisplayColor(int aToolkit, int aHandle, int aRgb);
}
