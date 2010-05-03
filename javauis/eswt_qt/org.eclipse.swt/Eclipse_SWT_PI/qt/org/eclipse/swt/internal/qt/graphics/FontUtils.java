/*******************************************************************************
 * Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 * 
 * Contributors:
 *     Nokia Corporation - initial implementation 
 *******************************************************************************/
package org.eclipse.swt.internal.qt.graphics;

/**
 * This is a helper class for getting font related information 
 * ouside GraphicsContext bind-release cycle, i.e. these utilities must be used for getting
 * font metrics when GraphicsContext does not have a target and it's methods cannot be used.
 * These utilities do not have any dependency to GraphicsContext or it's state.
 */
public final class FontUtils {
    
    /**
     * handle of native QFont
     */
    private int handle;
        
    /**
     * Constructs FontUtils with specified font handle
     * 
     * @param fontHandle Handle of native QFont
     * @throws IllegalArgumentException if given font handle is 0
     */
    public FontUtils(int fontHandle) {
    	checkState();
        setFont(fontHandle);
    }

    /**
     * Returns the ascent of the font. 
     * The ascent of a font is the distance from baseline to the highest position characters extent to.
     * 
     * @return The ascent for this font
     */
    public int getAscent() {
    	checkState();
        return OS.fontUtils_getAscent(handle);
    }
    
    /** 
     * Returns the bounding rectangle that is covered by given text if drawn at origo, 
     * i.e. coordinates (0,0). The vertical base point is based on the text baseline, i.e.
     * y-coordinate is always negative as text extends ascent number of pixel up from baseline.
     * Bounding rectangle height is based on font height not the actual height that the text would occupy.  
     * 
     * Note that the bounding rectangle might extent to left of (0,0), e.g. for italized fonts.
     * 
     * Values are stored in boundingBox array as follows: 
     * <ul>
     * <li>rect[GraphicsContect.RECT_X] - the x coordinate of rect</li>
     * <li>rect[GraphicsContect.RECT_Y] - the y coordinate of rect
     * <li>rect[GraphicsContect.RECT_WIDTH] - the width of rect
     * <li>rect[GraphicsContect.RECT_HEIGHT] - the height of rect
     * </ul>
     * 
     * @param boundingBox The array to where populate rect, size must not be < 4
     * @param str The text to measure
     * @return bounding rectangle coordinates x, y, width and height 
     * 
     * @throws IllegalArgumentException if size of <code>rect</code> is < 4
     * @throws NullPointerException if <code>rect</code>  or <code>str</code> is null
     * 
     */
    public void getBoundingRect(int[] boundingBox, String str) {
    	checkState();
        if(boundingBox == null || str == null) {
            throw new NullPointerException("boundingBox or str is null");
        }
        if(boundingBox.length < 4) {
            throw new IllegalArgumentException("boundingBox size is < 4");
        }

        if(str.length() == 0) {
            for(int i=0; i<4; i++)
            {
            	boundingBox[i]=0;
            }
            return;
        }
        OS.fontUtils_getBoundingRect(handle, boundingBox, str);
    }
    
    /**
     * Returns the descent of the font. 
     * The descent is of a font is the distance from baseline to the lowest position characters extent to.
     * 
     * @return The ascent for this font
     */
    public int getDescent() {
    	checkState();
        return OS.fontUtils_getDescent(handle);
    }
    
    /**
     * Sets new font to this instance. 
     * All subsequent measurements are based on the font set with this method.
     * 
     * @param fontHandle Handle of native QFont
     * @throws IllegalArgumentException if given font handle is 0
     */
    public void setFont(int fontHandle) {
    	checkState();
        if(fontHandle != 0) {
            handle = fontHandle;
        } else {
            throw new IllegalArgumentException("fontHandle is 0");
        }
    }
    
    /**
     * Gets the advance width of the specified string when using the currently set font.
     * The width specifies the distance where next character should ne drawn, note that 
     * the last character might partially be drawn beyond the right side of the returned width. 
     * 
     * @param str The characters to measure
     * @return The total advance width or 0 if size of str is 0
     * @throws NullPointerException if <code>str</code> is null
     */
    public int stringWidth(String str) {
    	checkState();
        if(str == null) {
            throw new NullPointerException("str is null");
        }
        if(str.length() == 0) {
            return 0;
        }
        return OS.fontUtils_getStringWidth(handle, str);
    }
    
    
    /**
     * private method for checking the state of this instance.
     */
    private void checkState() {
    	Utils.validateUiThread();
    }
    
}
