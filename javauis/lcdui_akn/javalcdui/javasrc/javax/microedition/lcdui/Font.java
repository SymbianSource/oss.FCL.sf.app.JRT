/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

import java.util.Hashtable;
import com.nokia.mj.impl.rt.legacy.NativeError;

public final class Font
{
    //
    // Font are displayed in this order, FACE,STYLE,SIZE
    //
    public static final int FACE_SYSTEM = 0;
    public static final int FACE_MONOSPACE = 32;
    public static final int FACE_PROPORTIONAL = 64;
    //
    public static final int STYLE_PLAIN = 0;
    public static final int STYLE_BOLD = 1;
    public static final int STYLE_ITALIC = 2;
    public static final int STYLE_UNDERLINED = 4;
    //
    public static final int SIZE_SMALL = 8;
    public static final int SIZE_MEDIUM = 0;
    public static final int SIZE_LARGE = 16;
    //
    public static final int FONT_INPUT_TEXT = 1;
    public static final int FONT_STATIC_TEXT = 0;
    //
    private static final int STYLE_TOTAL = STYLE_BOLD + STYLE_ITALIC + STYLE_UNDERLINED;
    //
    private int iFace;
    private int iStyle;
    private int iSize;
    //
    Toolkit iToolkit;
    int iHandle;
    //
    boolean iIsFreeSizeFont;

    private Font(Toolkit aToolkit, int aFace,int aStyle,int aSize,boolean aIsFreeSizeFont)
    {
        iFace = aFace;
        iStyle = aStyle;
        iSize = aSize;
        iToolkit = aToolkit;
        iIsFreeSizeFont = aIsFreeSizeFont;
        synchronized (iToolkit)
        {
            iHandle = NativeError.check(_create(iToolkit.getHandle(),aFace,aStyle,aSize,aIsFreeSizeFont));
        }
    }

    public static Font getDefaultFont()
    {
        return getFont(FACE_SYSTEM,STYLE_BOLD,SIZE_MEDIUM);
    }

    public static Font getFont(int aFontSpecifier)
    {
        if ((aFontSpecifier != FONT_INPUT_TEXT) && (aFontSpecifier != FONT_STATIC_TEXT))
            throw new IllegalArgumentException();
        //
        final Toolkit toolkit = Toolkit.getToolkit();
        final int fontSpec;
        synchronized (toolkit)
        {
            fontSpec = _getFontSpec(toolkit.getHandle(), aFontSpecifier);
        }
        final int face = (fontSpec>>16 & 0x000000ff);
        final int style = (fontSpec>>8 & 0x000000ff);
        final int size = (fontSpec & 0x000000ff);
        return getFont(face,style,size);
    }

    public static Font getFont(int aFace,int aStyle,int aSize)
    {
        final Toolkit toolkit = Toolkit.getToolkit();
        final Hashtable cache = toolkit.getFontCache();
        if ((aFace != FACE_SYSTEM && aFace != FACE_MONOSPACE && aFace != FACE_PROPORTIONAL) ||
                aStyle < STYLE_PLAIN || aStyle > STYLE_TOTAL ||
                (aSize != SIZE_MEDIUM && aSize != SIZE_SMALL && aSize != SIZE_LARGE))
        {
            throw new IllegalArgumentException();
        }
        final Integer key = new Integer(aFace|aStyle|aSize);
        Font font = (Font)cache.get(key);
        if (font == null)
        {
            font = new Font(toolkit,aFace,aStyle,aSize,false);
            cache.put(key,font);
        }
        return font;
    }

    static Font getFreeSizeFont(int aFace, int aStyle, int aHeight)
    {
        final Toolkit toolkit = Toolkit.getToolkit();
        if ((aFace != FACE_SYSTEM && aFace != FACE_MONOSPACE && aFace != FACE_PROPORTIONAL) ||
                aStyle < STYLE_PLAIN || aStyle > STYLE_TOTAL ||
                (aHeight < 0))
        {
            throw new IllegalArgumentException();
        }
        // we don't use font cache for fonts with custom height
        // because too many different fonts stored in cache
        // could consume too much memeory
        return new Font(toolkit, aFace, aStyle, aHeight, true);
    }

    public int getStyle()
    {
        return iStyle;
    };

    public int getSize()
    {
        return iSize;
    }

    public int getFace()
    {
        return iFace;
    }

    public boolean isPlain()
    {
        return iStyle == STYLE_PLAIN;
    }

    public boolean isBold()
    {
        return (iStyle & STYLE_BOLD) == STYLE_BOLD;
    }

    public boolean isItalic()
    {
        return (iStyle & STYLE_ITALIC) == STYLE_ITALIC;
    }

    public boolean isUnderlined()
    {
        return (iStyle & STYLE_UNDERLINED) == STYLE_UNDERLINED;
    }

    public int getHeight()
    {
        synchronized (iToolkit)
        {
            return NativeError.check(_height(iHandle,iToolkit.getHandle()));
        }
    }

    public int getBaselinePosition()
    {
        synchronized (iToolkit)
        {
            return NativeError.check(_baseline(iHandle, iToolkit.getHandle()));
        }
    }

    public int charWidth(char aChar)
    {
        synchronized (iToolkit)
        {
            return NativeError.check(_width(iHandle, iToolkit.getHandle(), new Character(aChar).toString()));
        }
    }

    public int charsWidth(char[] aChars,int aOffset,int aLength)
    {
        try
        {
            final String string = new String(aChars,aOffset,aLength);
            synchronized (iToolkit)
            {
                return NativeError.check(_width(iHandle,iToolkit.getHandle(),string));
            }
        }
        catch (StringIndexOutOfBoundsException ex)
        {
            throw new ArrayIndexOutOfBoundsException();
        }
    }

    public int stringWidth(String aString)
    {
        if (aString == null)
        {
            throw new NullPointerException();
        }
        synchronized (iToolkit)
        {
            return _width(iHandle,iToolkit.getHandle(),aString);
        }
    }

    public int substringWidth(String aString,int aOffset,int aLength)
    {
        synchronized (iToolkit)
        {
            return _width(iHandle,iToolkit.getHandle(),aString.substring(aOffset,aOffset+aLength));
        }
    }

    private native int _create(int aToolkit,int aFace,int aStyle,int aSize,boolean aIsFreeSizeFont);
    private native int _width(int aFont,int aToolkit,String aString);
    private native int _height(int aFont,int aToolkit);
    private native int _baseline(int aFont,int aToolkit);
    private static native int _getFontSpec(int aToolkit,int aFontSpecifier);
}

