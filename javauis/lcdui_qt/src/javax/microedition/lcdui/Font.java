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

import java.util.Enumeration;
import java.util.Hashtable;

import org.eclipse.ercp.swt.mobile.MobileShell;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.internal.qt.graphics.FontUtils;
import org.eclipse.swt.widgets.*;

/**
 * Implementation of LCDUI <code>Font</code> class.
 */
public final class Font
{
	/*
	 * Point size value for the Medium LCDUI fonts. All
	 * font sizes are relative to this value;
	 */
    private static final int MEDIUM_FONT_POINT_SIZE = 8;

	/**
     * System font face.
     *
     * @value for FACE_SYSTEM is 0.
     */
    public static final int FACE_SYSTEM = 0;

    /**
     * Monospace font face.
     *
     * @value for FACE_MONOSPACE is 32.
     */
    public static final int FACE_MONOSPACE = 32;

    /**
     * Proportional font face.
     *
     * @value for FACE_PROPORTIONAL is 64.
     */
    public static final int FACE_PROPORTIONAL = 64;

    /**
     * Plain font style.
     *
     * @value for STYLE_PLAIN is 0.
     */
    public static final int STYLE_PLAIN = 0;

    /**
     * Bold font style.
     *
     * @value for STYLE_BOLD is 1.
     */
    public static final int STYLE_BOLD = 1;

    /**
     * Italic font style.
     *
     * @value for STYLE_ITALIC is 2.
     */
    public static final int STYLE_ITALIC = 2;

    /**
     * Underlined font style.
     *
     * @value for STYLE_UNDERLINED is 4.
     */
    public static final int STYLE_UNDERLINED = 4;

    /**
     * Small font size.
     *
     * @value for SIZE_SMALL is 8.
     */
    public static final int SIZE_SMALL = 8;

    /**
     * Medium font size.
     *
     * @value for SIZE_MEDIUM is 0.
     */
    public static final int SIZE_MEDIUM = 0;

    /**
     * Large font size.
     *
     * @value for SIZE_LARGE is 16.
     */
    public static final int SIZE_LARGE = 16;

    /**
     * Font specifier for default input text drawing.
     *
     * @value for FONT_INPUT_TEXT is 1.
     */
    public static final int FONT_INPUT_TEXT = 1;

    /**
     * Font specifier for default Item and Screen drawing.
     *
     * @value for FONT_STATIC_TEXT is 0.
     */
    public static final int FONT_STATIC_TEXT = 0;

    private static final int STYLE_ALL = STYLE_PLAIN | STYLE_BOLD
                                         | STYLE_ITALIC | STYLE_UNDERLINED;

    private static final String FONTDATA_CONST = "|-1|-1|-1|-1|-1|-1|1|";

    /**
     * Lookup table which stores LCDUI fonts based on FontData keys.
     */
    private static Hashtable fontTable = new Hashtable();
    private static FontUtils fontUtils;
    private static int[] tempFontRect = new int[4];
    private static FontData eswtTempFontData;
    private static FontData eswtSystemFontData;
    private int eswtTempFontAscent;
    private int eswtTempFontHeight;
    private int eswtTempFontWidth;

    // Instance members.

    private org.eclipse.swt.graphics.Font eswtFont;
    private int face;
    private int style;
    private int size;

    /**
     * Disposes all fonts created.
     */
    static void disposeFonts()
    {
        for(Enumeration e = fontTable.elements(); e.hasMoreElements();)
        {
            Font font = (Font) e.nextElement();
            font.eswtFont.dispose();
        }
        fontTable.clear();
    }

    /**
     * LCDUI Font --> eSWT Font
     */
    static org.eclipse.swt.graphics.Font getESWTFont(Font font)
    {
        if(font != null)
        {
            return font.eswtFont;
        }
        return null;
    }

    /**
     * eSWT Font --> LCDUI Font
     */
    static Font getFont(final org.eclipse.swt.graphics.Font font)
    {
        if(font != null)
        {
            FontData fd = getFontData(font);
            return getFont(fd);
        }
        return null;
    }

    /**
     * Returns default font.
     *
     * @return default system font.
     */
    public static Font getDefaultFont()
    {
        return getFont(Font.FACE_SYSTEM, Font.STYLE_PLAIN, Font.SIZE_MEDIUM);
    }

    /**
     * Request specified Font.
     *
     * @param specifier font specifier
     * @return the font
     */
    public static Font getFont(int specifier)
    {
        if(specifier == FONT_INPUT_TEXT || specifier == FONT_STATIC_TEXT)
        {
            return getFont(getFontData(specifier));
        }
        else
        {
            throw new IllegalArgumentException(
                MsgRepository.FONT_EXCEPTION_INVALID_SPECIFIER);
        }
    }

    /**
     * Request Font based on parameters.
     *
     * @param face font face
     * @param style font style
     * @param size font size
     * @return the font
     */
    public static Font getFont(int face, int style, int size)
    {
        if(face != FACE_SYSTEM && face != FACE_MONOSPACE
                && face != FACE_PROPORTIONAL)
        {
            throw new IllegalArgumentException(
                MsgRepository.FONT_EXCEPTION_INVALID_FACE);
        }
        if(style < STYLE_PLAIN || style > STYLE_ALL)
        {
            throw new IllegalArgumentException(
                MsgRepository.FONT_EXCEPTION_INVALID_STYLE);
        }
        if(size != SIZE_SMALL && size != SIZE_MEDIUM && size != SIZE_LARGE)
        {
            throw new IllegalArgumentException(
                MsgRepository.FONT_EXCEPTION_INVALID_SIZE);
        }
        FontData fd = getFontData(face, style, size);
        Font font = getFont(fd);
        // We must handle underlining separately because it is not part of
        // eSWT's style.
        if((style & Font.STYLE_UNDERLINED) == Font.STYLE_UNDERLINED)
        {
            font.style |= Font.STYLE_UNDERLINED;
        }
        return font;

    }

    /**
     * Request freesized Font.
     *
     * @param face font face
     * @param style font style
     * @param height font height
     * @return the font
     */
    static Font getFreeSizedFont(int face, int style, int height)
    {
        if(face != FACE_SYSTEM && face != FACE_MONOSPACE
                && face != FACE_PROPORTIONAL)
        {
            throw new IllegalArgumentException(
                MsgRepository.FONT_EXCEPTION_INVALID_FACE);
        }
        if(style < STYLE_PLAIN || style > STYLE_ALL)
        {
            throw new IllegalArgumentException(
                MsgRepository.FONT_EXCEPTION_INVALID_STYLE);
        }
        if(height < 0)
        {
            throw new IllegalArgumentException(
                MsgRepository.FONT_EXCEPTION_INVALID_SIZE);
        }
        FontData fd = getFontData(face, style, height);
        Font font = getFont(fd);
        // We must handle underlining separately because it is not part of
        // eSWT's style.
        if((style & Font.STYLE_UNDERLINED) == Font.STYLE_UNDERLINED)
        {
            font.style |= Font.STYLE_UNDERLINED;
        }
        return font;
    }

    /**
     * Lookup or create a font based on given FontData.
     *
     * @param fontdata font's data
     * @return a font
     */
    private static Font getFont(FontData fontdata)
    {
        Font ret = null;
        if(fontdata != null)
        {
            if(fontTable.containsKey(fontdata))
            {
                ret = (Font) fontTable.get(fontdata);
            }
            else
            {
                ret = new Font(fontdata);
            }
        }
        return ret;
    }

    /**
     * Constructor (private).<br>
     *
     * @param reqFD the requested eSWT FontData
     */
    private Font(final FontData reqFD)
    {
        ESWTUIThreadRunner.syncExec(new Runnable()
        {
            public void run()
            {
                eswtFont = new org.eclipse.swt.graphics.Font(ESWTUIThreadRunner
                        .getInstance().getDisplay(), reqFD);
            }
        });
        // get FontData info from resulted Font, because resulting FontData
        // might be different than the requested one
        FontData resFD = getFontData(eswtFont);
        this.face = mapFontNameToFace(resFD.getName());
        this.style = mapESWTStyleToStyle(resFD.getStyle());
        this.size = mapHeightToSize(resFD.getHeight());
        fontTable.put(resFD, this);
    }

    /**
     * Return font's style.
     *
     * @return style
     */
    public int getStyle()
    {
        return style;
    }

    /**
     * Return font's size.
     *
     * @return size
     */
    public int getSize()
    {
        return size;
    }

    /**
     * Return font's face.
     *
     * @return face
     */
    public int getFace()
    {
        return face;
    }

    /**
     * Checks if the font is plain styled.
     *
     * @return true if the font is plain, false otherwise
     */
    public boolean isPlain()
    {
        return (style == Font.STYLE_PLAIN);
    }

    /**
     * Checks if the font is bold styled.
     *
     * @return true if the font is bold, false otherwise
     */
    public boolean isBold()
    {
        return ((style & Font.STYLE_BOLD) == Font.STYLE_BOLD);
    }

    /**
     * Checks if the font is italic styled.
     *
     * @return true if the font is italic, false otherwise
     */
    public boolean isItalic()
    {
        return ((style & Font.STYLE_ITALIC) == Font.STYLE_ITALIC);
    }

    /**
     * Checks if the font is underlined styled.
     *
     * @return true if the font is underlined, false otherwise.
     */
    public boolean isUnderlined()
    {
        return ((style & Font.STYLE_UNDERLINED) == Font.STYLE_UNDERLINED);
    }

    /**
     * Return font's height.
     *
     * @return height
     */
    public int getHeight()
    {
        if(eswtTempFontHeight == 0)
        {
            ESWTUIThreadRunner.safeSyncExec(new Runnable()
            {
                public void run()
                {
                    FontUtils fu = eswtGetFontUtils(eswtFont);
                    eswtTempFontHeight = fu.getAscent() + fu.getDescent();
                }
            });
        }
        return eswtTempFontHeight;
    }

    /**
     * The position of the baseline from the top of the font in pixels.
     *
     * @return the baseline position
     */
    public int getBaselinePosition()
    {
        if(eswtTempFontAscent == 0)
        {
            ESWTUIThreadRunner.safeSyncExec(new Runnable()
            {
                public void run()
                {
                    eswtTempFontAscent = eswtGetFontUtils(eswtFont).getAscent();
                }
            });
        }
        return eswtTempFontAscent;
    }

    /**
     * Returns the width of the given character in pixels.
     *
     * @param c the character
     * @return the width
     */
    public int charWidth(char c)
    {
        return stringWidth(String.valueOf(c));
    }

    /**
     * Returns the width of the given character array's region in pixels.
     *
     * @param c characters
     * @param offset starting position
     * @param length the length
     * @return the width
     */
    public int charsWidth(char[] c, int offset, int length)
    {
        try
        {
           final String string = new String(c, offset, length);
           return stringWidth(string);
        }catch (StringIndexOutOfBoundsException ex)
        {
            throw new ArrayIndexOutOfBoundsException();
        }
    }

    /**
     * Returns string width in pixels.
     *
     * @param string the string
     * @return the width
     */
    public int stringWidth(String string)
    {
        if(string == null)
        {
            throw new NullPointerException(
                MsgRepository.FONT_EXCEPTION_NULL_STRING);
        }
        final String finalString = string;
        eswtTempFontWidth = 0;
        ESWTUIThreadRunner.safeSyncExec(new Runnable()
        {
            public void run()
            {
                eswtGetFontUtils(eswtFont).getBoundingRect(tempFontRect, finalString);
                eswtTempFontWidth = tempFontRect[2];
            }
        });
        return eswtTempFontWidth;
    }

    /**
     * Get the static FontUtils instance setup with given font.
     */
    private static FontUtils eswtGetFontUtils(org.eclipse.swt.graphics.Font font)
    {
        if(fontUtils == null)
        {
            fontUtils = new FontUtils(font.handle);
        }
        else
        {
            fontUtils.setFont(font.handle);
        }
        return fontUtils;
    }

    /**
     * Returns sub-string width in pixels.
     *
     * @param string the string
     * @param offset starting position
     * @param length the length
     * @return the width of the specified region
     */
    public int substringWidth(String string, int offset, int length)
    {
        return stringWidth(string.substring(offset, offset + length));
    }

    /**
     * Get FontData of a eSWT control.
     *
     * @param control eSWT control
     * @return FontData
     */
    private static FontData getFontData(final int specifier)
    {
        ESWTUIThreadRunner.syncExec(new Runnable()
        {
            public void run()
            {
                Control control = null;
                MobileShell shell = new MobileShell(ESWTUIThreadRunner
                                                    .getInstance().getDisplay(), SWT.NONE);
                if(specifier == FONT_INPUT_TEXT)
                {
                    control = new Text(shell, SWT.NONE);
                }
                else
                {
                    control = new Label(shell, SWT.NONE);
                }
                eswtTempFontData = control.getFont().getFontData()[0];
                control.dispose();
                shell.dispose();
            }
        });
        return eswtTempFontData;
    }

    /**
     * Get FontData of a eSWT font.
     *
     * @param font eSWT font
     * @return FontData
     */
    private static FontData getFontData(
        final org.eclipse.swt.graphics.Font font)
    {
        ESWTUIThreadRunner.syncExec(new Runnable()
        {
            public void run()
            {
                eswtTempFontData = font.getFontData()[0];
            }
        });
        return eswtTempFontData;
    }

    /**
     * Construct FontData with the given specifiers.
     *
     * @return FontData
     */
    private static FontData getFontData(final int face, final int style,
                                        final int size)
    {
        ESWTUIThreadRunner.syncExec(new Runnable()
        {
            public void run()
            {
                int underlined = 0;
                if((style & Font.STYLE_UNDERLINED) == Font.STYLE_UNDERLINED)
                {
                    underlined = 1;
                }
                /* FontData constructor format:
                 *  version|name|height|style|underline|
                 *  overline|strike|stretch|pitch|qt-style|weight|strategy|
                 */
                StringBuffer sb = new StringBuffer();
                sb.append("1|"); // version
                sb.append(mapFaceToFontName(face)); // family name
                sb.append("|");
                sb.append(mapSizeToHeight(size)); // height
                sb.append("|");
                sb.append(mapStyleToESWTStyle(style)); // style
                sb.append("|QT|1|");//Qt version 2 string for extra formats
                sb.append((new Integer(underlined).toString())); // underline
                sb.append(FONTDATA_CONST);
                eswtTempFontData = new FontData(sb.toString());
            }
        });
        return eswtTempFontData;
    }

    /**
     * Returns System default font's FontData instance.
     *
     * @return system FontData
     */
    private static FontData getSystemFontData()
    {
        if(eswtSystemFontData == null)
        {
            ESWTUIThreadRunner.syncExec(new Runnable()
            {
                public void run()
                {
                    eswtSystemFontData = ESWTUIThreadRunner.getInstance()
                                         .getDisplay().getSystemFont().getFontData()[0];
                }
            });
        }
        return eswtSystemFontData;
    }

    /**
     * Get eSWT font name from LCDUI face parameter.
     *
     * @param face font's face type
     * @return font name
     */
    private static String mapFaceToFontName(int face)
    {
        // TODO: how to map face values to names
        switch(face)
        {
        case Font.FACE_MONOSPACE:
        case Font.FACE_PROPORTIONAL:
        case Font.FACE_SYSTEM:
        default:
            return getSystemFontData().getName();
        }
    }

    /**
     * Get face of a given eSWT font name.
     *
     * @param eswtFontName font's name
     * @return font face
     */
    private static int mapFontNameToFace(String eswtFontName)
    {
        // TODO: how to map names to face values
        if(eswtFontName.equals(getSystemFontData().getName()))
        {
            return Font.FACE_SYSTEM;
        }
        else
        {
            // Font.FACE_MONOSPACE:
            // Font.FACE_PROPORTIONAL:
            return 0;
        }
    }

    /**
     * Get eSWT font height from LCDUI size parameter.<br>
     * 0 maps to system's default font size, others map directly.
     *
     * @param size font's size
     * @return font height
     */
    private static int mapSizeToHeight(int size)
    {
    	//maps relative to the medium font size
    	//This is a static value on LCDUI because
    	// Qt value was static and very small.
    	switch(size){
    		case Font.SIZE_SMALL:
    			return MEDIUM_FONT_POINT_SIZE - 1;
    		case Font.SIZE_LARGE:
    			return MEDIUM_FONT_POINT_SIZE + 1;
    		default:
    			return MEDIUM_FONT_POINT_SIZE;
    	}
      }

    /**
     * Get LCDUI size from eSWT font height
     *
     * @param height font's height
     * @return font size
     */
    private static int mapHeightToSize(int height)
    {
        if(height < MEDIUM_FONT_POINT_SIZE)
        {
            return Font.SIZE_SMALL;
        }
        else if(height > MEDIUM_FONT_POINT_SIZE)
        {
            return Font.SIZE_LARGE;
        }
        else
        {
            return Font.SIZE_MEDIUM;
        }
    }

    /**
     * Return eSWT style.
     *
     * @param style Font's LCDUI style.
     * @return Font's eSWT style.
     */
    private static int mapStyleToESWTStyle(int style)
    {
        int retStyle = SWT.NORMAL;
        if((style & Font.STYLE_BOLD) == Font.STYLE_BOLD)
        {
            retStyle |= SWT.BOLD;
        }
        if((style & Font.STYLE_ITALIC) == Font.STYLE_ITALIC)
        {
            retStyle |= SWT.ITALIC;
        }
        return retStyle;
    }

    /**
     * Return LCDUI style.
     *
     * @param eswtStyle Font's eSWT style.
     * @return Font's LCDUI style.
     */
    private static int mapESWTStyleToStyle(int eswtStyle)
    {
        int retStyle = Font.STYLE_PLAIN;
        if((eswtStyle & SWT.BOLD) != 0)
        {
            retStyle |= Font.STYLE_BOLD;
        }
        if((eswtStyle & SWT.ITALIC) != 0)
        {
            retStyle |= Font.STYLE_ITALIC;
        }
        return retStyle;
    }

}
