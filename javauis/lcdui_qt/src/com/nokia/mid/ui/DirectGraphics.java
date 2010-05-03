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

import javax.microedition.lcdui.Image;

public interface DirectGraphics {

    /**
     * Constant for horizontal image flipping.
     *
     * @see #drawImage(Image img, int x, int y, int anchor, int manipulation)
     * @since 1.0
     */
    public static final int FLIP_HORIZONTAL = 8192;

    /**
     * Constant for vertical image flipping.
     *
     * @see #drawImage(Image img, int x, int y, int anchor, int manipulation)
     * @since 1.0
     */
    public static final int FLIP_VERTICAL = 16384;

    /**
     * Constant for rotating image 90 degrees counter-clockwise. ROTATE_90 can
     * be multiplied using "(ROTATE_90 + ROTATE_90)". If rotate constant is
     * combined with flip use brackets: "(ROTATE_90 * 2) | FLIP_VERTICAL".
     * <p>
     * (ROTATE_90 * 2) results 180 degree rotate. <br>
     * (ROTATE_90 * 3) results 270 degree rotate.
     *
     * @see #drawImage(Image img, int x, int y, int anchor, int manipulation)
     * @since 1.0
     */
    public static final int ROTATE_90 = 90;

    /**
     * Constant for rotating image 180 degrees counter-clockwise. ROTATE_180 =
     * (2 * ROTATE_90).
     *
     * @see #drawImage(Image img, int x, int y, int anchor, int manipulation)
     * @since 1.0
     */
    public static final int ROTATE_180 = 180;

    /**
     * Constant for rotating image 270 degrees counter-clockwise. ROTATE_180 =
     * (3 * ROTATE_90).
     *
     * @see #drawImage(Image img, int x, int y, int anchor, int manipulation)
     * @since 1.0
     */
    public static final int ROTATE_270 = 270;

    /**
     * 1 bit format, 2 distinct color values (on/off), stored as a byte. 8 pixel
     * values in a single byte, packed as close as possible. The pixel data is
     * presented horizontally from left to right. The scanlength and offset
     * parameters for drawPixels and getPixels are given in number of pixels,
     * not as indices to pixels array.
     * <p>
     * All implementations must support this pixel format.
     * <p>
     *
     * <pre>
     *  Consider following 9x9 graphics context with '+' indicating
     *  pixels set to &quot;on&quot; position:
     *  &quot;MSB&quot; = Most significant bit in a byte,
     *  &quot;LSB&quot; = Least significant bit in a byte,
     *  &quot;|&quot; = Byte bounds,
     *      &quot;Bit#&quot; = bits in first row.
     *         MSB         LSB
     *  Bit#-&gt; 1 2 3 4 5 6 7 8 1
     *  ------------------------
     *         + +           +|+
     *         + + +        |+ +
     *         + + + +    |  + +
     *         + + + + +|    + +
     *         + +   +|+ +   + +
     *         + +  |  + + + + +
     *         + +|      + + + +
     *         +|+         + + +|
     *         + +           +|+
     *  byte[] byteArray = new byte[18];
     *  getPixels(byteArray, null, 0, 8, 0, 0, 9, 9,
     *            TYPE_BYTE_1_GRAY);
     *  byteArray[0]  = (byte) 0xC1; // binary 11000001
     *  byteArray[1]  = (byte) 0xF0; // binary 11110000
     *  byteArray[2]  = (byte) 0xFC; // binary 11111100
     *  byteArray[3]  = (byte) 0x7F; // binary 01111111
     *  byteArray[4]  = (byte) 0x3D; // binary 00111101
     *  byteArray[5]  = (byte) 0xDE; // binary 11011110
     *  byteArray[6]  = (byte) 0x7F; // binary 01111111
     *  byteArray[7]  = (byte) 0x1F; // binary 00011111
     *  byteArray[8]  = (byte) 0x87; // binary 10000111
     *  byteArray[9]  = (byte) 0xC1; // binary 11000001
     *  byteArray[10] = (byte) 0x80; // binary 10000000
     *  Note that only one bit was defined for byte stored in
     *  index 10 of byteArray, all other bits in this byte were
     *  left unchanged.
     * </pre>
     *
     * @since 1.0
     */
    public static final int TYPE_BYTE_1_GRAY = 1;

    /**
     * 1 bit format, 2 distinct color values (on/off), stored as a byte. 8 pixel
     * values in a single byte. The pixel data is presented in bytes in which
     * each byte defines 8 vertically packed pixels. The byte packed pixels are
     * arranged in columns from left to right.
     * <p>
     * The scanlength and offset parameters for drawPixels and getPixels are
     * given in number of pixels, not as indices to pixels array. If the height
     * of a given graphic context is not divisible by 8, there are overflow bits
     * in bytes that are used to store last pixels of a column. These bits must
     * be taken into account when using drawPixels and getPixels methods by
     * using proper offset and scanlength arguments.
     * <p>
     * All implementations must support this pixel format.
     * <p>
     *
     * <pre>
     *  Consider following 9x9 graphics context with '+' indicating
     *  pixels set to &quot;on&quot; position:
     *  Byte#  0 1 2 3 4 5 6 7 8  Bit#
     *  ------------------------------
     *  LSB    + +           + +   1
     *         + + +         + +   2
     *         + + + +       + +   3
     *         + + + + +     + +   4
     *         + +   + + +   + +   5
     *         + +     + + + + +   6
     *         + +       + + + +   7
     *  MSB    + +         + + +   8
     *         + +           + +   1
     *  byte[] byteArray = new byte[18];
     *  getPixels(byteArray, null, 0, 16, 0, 0, 9, 9,
     *            TYPE_BYTE_1_GRAY_VERTICAL);
     *  byteArray[0]  = (byte) 0xFF; // binary 11111111
     *  byteArray[1]  = (byte) 0xFF; // binary 11111111
     *  byteArray[2]  = (byte) 0x0E; // binary 00001110
     *  byteArray[3]  = (byte) 0x1C; // binary 00011100
     *  byteArray[4]  = (byte) 0x38; // binary 00111000
     *  byteArray[5]  = (byte) 0x70; // binary 01110000
     *  byteArray[6]  = (byte) 0xE0; // binary 11100000
     *  byteArray[7]  = (byte) 0xFF; // binary 11111111
     *  byteArray[8]  = (byte) 0xFF; // binary 11111111
     *  byteArray[9]  = (byte) 0x01; // binary 00000001
     *  byteArray[10] = (byte) 0x01; // binary 00000001
     *  byteArray[11] = (byte) 0x00; // binary 00000000
     *  byteArray[12] = (byte) 0x00; // binary 00000000
     *  byteArray[13] = (byte) 0x00; // binary 00000000
     *  byteArray[14] = (byte) 0x00; // binary 00000000
     *  byteArray[15] = (byte) 0x00; // binary 00000000
     *  byteArray[16] = (byte) 0x01; // binary 00000001
     *  byteArray[17] = (byte) 0x01; // binary 00000001
     *  Note that only one bit was defined
     *  for bytes stored in indices 9-17 of byteArray,
     *  all other bits in those bytes were left unchanged.
     * </pre>
     *
     * @since 1.1
     */
    public static final int TYPE_BYTE_1_GRAY_VERTICAL = -1;

    /**
     * 2 bit format, 4 gray scale colors. Stored as a byte, 4 pixel values in a
     * single byte. The pixel data is presented horizontally from left to right.
     *
     * @since 1.0
     */
    public static final int TYPE_BYTE_2_GRAY = 2;

    /**
     * 4 bit format, 16 gray scale colors. Stored as a byte, 2 pixel values in a
     * single byte. The pixel data is presented horizontally from left to right.
     *
     * @since 1.0
     */
    public static final int TYPE_BYTE_4_GRAY = 4;

    /**
     * 8 bit format, 256 gray scale colors. Stored as a byte, 1 pixel value in a
     * single byte. The pixel data is presented horizontally from left to right.
     *
     * @since 1.0
     */
    public static final int TYPE_BYTE_8_GRAY = 8;

    /**
     * 3 bits for red, 3 bits for green, and 2 bits for blue component in a
     * pixel, stored as a byte. The pixel data is presented horizontally from
     * left to right.
     *
     * @since 1.0
     */
    public static final int TYPE_BYTE_332_RGB = 332;

    /**
     * 4 bits for alpha, red, green and blue component in a pixel, stored as a
     * short (0xARGB).
     *
     * @since 1.0
     */
    public static final int TYPE_USHORT_4444_ARGB = 4444;

    /**
     * 4 bits for red, green and blue component in a pixel, stored as a short
     * (0x0RGB).
     *
     * @since 1.0
     */
    public static final int TYPE_USHORT_444_RGB = 444;

    /**
     * 5 bits for red, green and blue component in a pixel.
     *
     * @since 1.0
     */
    public static final int TYPE_USHORT_555_RGB = 555;

    /**
     * 1 bit for alpha, 5 bits for red, green and blue component in a pixel.
     *
     * @since 1.0
     */
    public static final int TYPE_USHORT_1555_ARGB = 1555;

    /**
     * 5 bits for red, 6 bits for green and 5 bits for blue component in a
     * pixel.
     *
     * @since 1.0
     */
    public static final int TYPE_USHORT_565_RGB = 565;

    /**
     * 8 bits for red, green and blue component in a pixel (0x00RRGGBB). The
     * high order byte (alpha) is ignored by the implementation.
     *
     * @since 1.0
     */
    public static final int TYPE_INT_888_RGB = 888;

    /**
     * 8 bits for alpha, red, green and blue component in a pixel (0xAARRGGBB).
     *
     * @since 1.0
     */
    public static final int TYPE_INT_8888_ARGB = 8888;

    /**
     * Draws image to the graphics context. Does common image manipulations
     * during drawing of an image. Manipulation can be 0 if no manipulation is
     * done. Draws the specified image by using the anchor point - the anchor
     * point is applied after the manipulation. Anchor values are defined in
     * {@link javax.microedition.lcdui.Graphics}. The image can be drawn in
     * different positions relative to the anchor point by passing the
     * appropriate position constants.
     *
     * @param img the specified image to be drawn
     * @param x the x coordinate of the anchor point
     * @param y the y coordinate of the anchor point
     * @param anchor the anchor point for positioning the image
     * @param manipulation flip or rotate value or a combination of values, 0 if
     *            none
     * @throws java.lang.IllegalArgumentException if anchor is not a legal value
     *             or manipulation is not supported
     * @throws java.lang.NullPointerException if img is null
     * @since 1.0
     */
    public void drawImage(Image img, int x, int y, int anchor,
            int manipulation);

    /**
     * Copies or draws the pixel data directly to graphics context to specific
     * location. The pixels are passed in the format defined by the format
     * parameter. If an implementation doesn't support the format an
     * IllegalArgumentException is thrown. Note that it is highly possible that
     * only the native format is supported via appropriate method.
     * <p>
     * This method accepts only byte based formats. Passing all other formats
     * will result an IllegalArgumentException.
     * <p>
     * The operation is subject to the current clip region and translation for
     * this Graphics object. Bytes in pixels and transparencyMask arrays will be
     * passed in the same format. The transparencyMask can be null. If
     * transparency mask is null the image is considered fully opaque.
     * <p>
     * Note that scanlength and offset parameters indicate the scanlength and
     * offset in number of pixels. This is not necessarily the same as array
     * indices since multiple pixels may be stored in a byte. Note also that for
     * some format types there may be overflow bits in some bytes. These bits
     * must be taked into account in scanlength and offset parameters.
     * <p>
     *
     * @param pixels an array of pixel values
     * @param transparencyMask an array defining transparency mask
     * @param offset the index of the first pixel and mask value
     * @param scanlength the relative array offset between the corresponding
     *            pixels and mask value in consecutive rows
     * @param x the horizontal rendering location on graphics context
     * @param y the vertical rendering location on graphics context
     * @param width the width of the region to the rendered
     * @param height the height of the region to the rendered
     * @param manipulation the manipulation done to the image before the draw, 0
     *            means none.
     * @param format the format in which the pixels are provided in
     * @throws java.lang.NullPointerException if pixels is null
     * @throws java.lang.ArrayIndexOutOfBoundsException if requested to access
     *             beyond the length of array or with negative index
     * @throws java.lang.IllegalArgumentException if manipulation is unknown,
     *             width or height is negative, or format is wrong or
     *             unsupported
     * @since 1.0
     */
    public void drawPixels(byte[] pixels, byte[] transparencyMask, int offset,
            int scanlength, int x, int y, int width, int height,
            int manipulation, int format);

    /**
     * Copies or draws the pixel data directly to graphics context to specific
     * location from the array starting from specified offset. The pixels are
     * passed in the format defined by the format parameter. If an
     * implementation doesn't support the format an IllegalArgumentException is
     * thrown. Note that it is highly possible that only the native format is
     * supported via appropriate method.
     * <p>
     * This method accepts only int based formats. Passing all other formats
     * will result an IllegalArgumentException.
     * <p>
     * The operation is subject to the current clip region and translation for
     * this Graphics object.
     * <p>
     * The boolean value transparency will indicate whether the pixel's
     * transparency value will be checked. If the transparency argument is false
     * the pixels are set to the graphics context without comparing the
     * transparency values. If transparency argument is true pixel's
     * transparency value is checked and it will affect the drawing of a pixel.
     * <p>
     * The drawPixels paints the pixel data into the graphics context in
     * following fashion:
     *
     * <pre>
     * P(x1, y1) = pixels[offset + (x1 - x) + (y1 - y) * scanlength],
     * for each P(x1, y1), where (x &lt;= x1 &lt; x + width) and (y &lt;= y1 &lt; y + height).
     * </pre>
     *
     * @param pixels an array of pixel values (0xAARRGGBB)
     * @param offset the index of the first pixel value
     * @param scanlength the relative array offset between the corresponding
     *            pixels in consecutive rows
     * @param x the horizontal rendering location on graphics context
     * @param y the vertical rendering location on graphics context
     * @param width the width of the region to the rendered
     * @param height the height of the region to the rendered
     * @param transparency true if the pixel's transparency value will be
     *            checked
     * @param manipulation the manipulation done to the image before the draw, 0
     *            means none.
     * @param format the format in which the pixels are provided in
     * @throws java.lang.NullPointerException if pixels is null
     * @throws java.lang.ArrayIndexOutOfBoundsException if requested to access
     *             pixels beyond the length of array or with negative index
     * @throws java.lang.IllegalArgumentException if manipulation is unknown,
     *             width or height is negative, or format is wrong or
     *             unsupported
     * @since 1.0
     */
    public void drawPixels(int[] pixels, boolean transparency, int offset,
            int scanlength, int x, int y, int width, int height,
            int manipulation, int format);

    /**
     * Copies or draws the pixel data directly to graphics context to specific
     * location. The pixels are passed in the format defined by the format
     * parameter. If an implementation doesn't support the format an
     * IllegalArgumentException is thrown. Note that it is highly possible that
     * only the native format is supported via appropriate method.
     * <p>
     * This method accepts only short based formats. Passing all other formats
     * will result an IllegalArgumentException.
     * <p>
     * The operation is subject to the current clip region and translation for
     * this Graphics object. The boolean value transparency will indicate
     * whether the pixel's transparency value will be checked if any. If the
     * transparency argument is false the pixels are set to the graphics context
     * without comparing the transparency values. If transparency argument is
     * true pixel's transparency value is checked and it will affect the drawing
     * of a pixel.
     * <p>
     * The drawPixels paints the pixel data into the graphics context in
     * following fashion:
     *
     * <pre>
     * P(x1, y1) = pixels[offset + (x1 - x) + (y1 - y) * scanlength],
     * for each P(x1, y1), where (x &lt;= x1 &lt; x + width) and (y &lt;= y1 &lt; y + height).
     * </pre>
     *
     * @param pixels an array of pixel values
     * @param offset the index of the first pixel value
     * @param scanlength the relative array offset between the corresponding
     *            pixels in consecutive rows
     * @param x the horizontal rendering location on graphics context
     * @param y the vertical rendering location on graphics context
     * @param width the width of the region to the rendered
     * @param height the height of the region to the rendered
     * @param transparency true if the pixel's transparency value will be
     *            checked
     * @param manipulation the manipulation done to the image before the draw, 0
     *            means none.
     * @param format the format in which the pixels are provided in
     *
     * @throws java.lang.NullPointerException if pixels is null
     * @throws java.lang.ArrayIndexOutOfBoundsException if requested to access
     *             beyond the length of array or with negative index
     * @throws java.lang.IllegalArgumentException if manipulation is unknown,
     *             width or height is negative, or format is wrong or
     *             unsupported
     * @since 1.0
     */
    public void drawPixels(short[] pixels, boolean transparency, int offset,
            int scanlength, int x, int y, int width, int height,
            int manipulation, int format);

    /**
     * Draws a closed polygon defined by arrays of x and y coordinates. Each
     * pair of (x, y) coordinates defines a point.
     * <p>
     * This method draws the polygon defined by nPoint line segments, where the
     * first nPoint - 1 line segments are line segments from (xPoints[xOffset +
     * i - 1], yPoints[yOffset + i - 1]) to (xPoints[xOffset + i],
     * yPoints[yOffset + i]), for 1 <= i <= nPoints. The figure is automatically
     * closed by drawing a line connecting the final point to the first point,
     * if those points are different.
     *
     * @param xPoints an array of x coordinates
     * @param xOffset an offset to first x point in xPoints
     * @param yPoints an array of y coordinates
     * @param yOffset an offset to first y point in yPoints
     * @param nPoints the total number of points
     * @param argbColor the ARGB color value used painting the polygon
     * @since 1.0
     */
    public void drawPolygon(int[] xPoints, int xOffset, int[] yPoints,
            int yOffset, int nPoints, int argbColor);

    /**
     * Draws closed triangle defined by coordinates.
     * <p>
     * Note that the method uses the color passed as a parameter for drawing the
     * triangle and <i>not</i> the current active color of graphics context.
     * This is due to efficiency reasons as it is expected that the method may
     * be called several times in each repaint cycle, thus separate calls to
     * setColor would create unnecessary overhead. The call won't change the
     * current active drawing color of graphics context.
     *
     * @param x1 the x coordinate of first vertex
     * @param y1 the y coordinate of first vertex
     * @param x2 the x coordinate of second vertex
     * @param y2 the y coordinate of second vertex
     * @param x3 the x coordinate of third vertex
     * @param y3 the y coordinate of third vertex
     * @param argbColor the ARGB color value used painting the triangle
     * @since 1.0
     */
    public void drawTriangle(int x1, int y1, int x2, int y2, int x3,
            int y3, int argbColor);

    /**
     * Fills a closed polygon defined by arrays of x and y coordinates.
     * <p>
     * This method draws the polygon defined by nPoint line segments, where the
     * first nPoint - 1 line segments are line segments from (xPoints[xOffset +
     * i - 1], yPoints[yOffset + i - 1]) to (xPoints[xOffset + i],
     * yPoints[yOffset + i]), for 1 <= i <= nPoints. The figure is automatically
     * closed by drawing a line connecting the final point to the first point,
     * if those points are different.
     * <p>
     * The area inside the polygon is defined using an even-odd fill rule, also
     * known as the alternating rule.
     *
     * @param xPoints an array of x coordinates
     * @param xOffset an offset to first x point in xPoints
     * @param yPoints an array of y coordinates
     * @param yOffset an offset to first y point in yPoints
     * @param nPoints the total number of points
     * @param argbColor the ARGB color value used painting the polygon
     * @since 1.0
     */
    public void fillPolygon(int[] xPoints, int xOffset, int[] yPoints,
            int yOffset, int nPoints, int argbColor);

    /**
     * Fills closed triangle defined by coordinates.
     * <p>
     * Note that the method uses the color passed as a parameter for drawing the
     * triangle and <i>not</i> the current active color of graphics context.
     * This is due to efficiency reasons as it is expected that the method may
     * be called several times in each repaint cycle, thus separate calls to
     * setColor would create unnecessary overhead. The call won't change the
     * current active drawing color of graphics context.
     *
     * @param x1 the x coordinate of first vertex
     * @param y1 the y coordinate of first vertex
     * @param x2 the x coordinate of second vertex
     * @param y2 the y coordinate of second vertex
     * @param x3 the x coordinate of third vertex
     * @param y3 the y coordinate of third vertex
     * @param argbColor the ARGB color value used painting the triangle
     * @since 1.0
     */
    public void fillTriangle(int x1, int y1, int x2, int y2, int x3,
            int y3, int argbColor);

    /**
     * Gets the alpha component of the current color.
     *
     * @return integer value in range 0-255
     */
    public int getAlphaComponent();

    /**
     * Returns the native color format used, for example,
     * {@link DirectGraphics#TYPE_BYTE_1_GRAY} or
     * {@link DirectGraphics#TYPE_USHORT_4444_ARGB}. Application can check the
     * expected by drawPixels and returned from getPixels methods of this
     * interface.
     *
     * @return the native format used in drawPixels and getPixels array methods.
     * @since 1.0
     *
     */
    public int getNativePixelFormat();

    /**
     * Gets the pixel (including any transparency mask) values of an image to an
     * array of byte values from specific screen location. The pixels will be
     * passed in the format defined by format parameter. If an implementation
     * doesn't support the format an IllegalArgumentException is thrown. Note
     * that it is highly possible that only the native format is supported via
     * appropriate method.
     * <p>
     * This method returns only byte based formats. Requesting all other formats
     * will result an IllegalArgumentException.
     * <p>
     * Throws ArrayIndexOutOfBoundsException if array size is too small for
     * image pixels or transparency mask. The argument transparencyMask can be
     * null if caller is not interested in getting the mask.
     * <p>
     * Note that scanlength and offset parameters indicate the scanlength and
     * offset in number of pixels. This is not necessarily the same as array
     * indices since multiple pixels may be stored in a byte. Note also that for
     * some format types there may be overflow bits in some bytes. These bits
     * must be taked into account in scanlength and offset parameters.
     * <p>
     *
     * @param pixels an array in which the image pixel information will be
     *            stored
     * @param transparencyMask an array in which the transparency mask will be
     *            stored
     * @param offset offset into the array where the first pixel and mask value
     *            will be stored
     * @param scanlength the relative offset in the array between corresponding
     *            pixels and mask value in consecutive rows
     * @param x the x-coordinate of the upper left corner of the region
     * @param y the y-coordinate of the upper left corner of the region
     * @param width the width of the region
     * @param height the height of the region
     * @param format the format in which the pixels are provided in
     *
     * @throws java.lang.NullPointerException if pixels is null
     * @throws java.lang.ArrayIndexOutOfBoundsException if array size is too
     *             small for the image pixels or negative index access is
     *             attempted, contents of the array remain unchanged
     * @throws java.lang.IllegalArgumentException if x, y, width or height is
     *             negative, or format is wrong or unsupported
     * @since 1.0
     */
    public void getPixels(byte[] pixels, byte[] transparencyMask, int offset,
            int scanlength, int x, int y, int width, int height, int format);

    /**
     * Gets the pixel values of an image to an array defined by format
     * parameter. If an implementation doesn't support the format an
     * IllegalArgumentException is thrown. Note that it is highly possible that
     * only the native format is supported via appropriate method.
     * <p>
     * This method returns only int based formats. Requesting all other formats
     * will result an IllegalArgumentException.
     * <p>
     * Throws ArrayIndexOutOfBoundsException if array size is too small for
     * image pixels.
     * <p>
     * The getPixels methods stores the pixel data to pixels array in following
     * fashion:
     *
     * <pre>
     * pixels[offset + (x1 - x) + (y1 - y) * scanlength] = P(x1, y1),
     * for each P(x1, y1), where (x &lt;= x1 &lt; x + width) and (y &lt;= y1 &lt; y + height).
     * </pre>
     *
     * @param pixels an array in which the image pixel information will be
     *            stored
     * @param offset the index into the array where the first pixel value will
     *            be stored
     * @param scanlength the relative offset in the array between corresponding
     *            pixels in consecutive rows
     * @param x the x-coordinate of the upper left corner of the region
     * @param y the y-coordinate of the upper left corner of the region
     * @param width the width of the region
     * @param height the height of the region
     * @param format the format in which the pixels are provided in
     *
     * @throws java.lang.NullPointerException if pixels is null
     * @throws java.lang.ArrayIndexOutOfBoundsException if array size is too
     *             small for the image pixels or negative index access is
     *             attempted, contents of the array remain unchanged
     * @throws java.lang.IllegalArgumentException if x, y, width or height is
     *             negative, or format is wrong or unsupported
     * @since 1.0
     */
    public void getPixels(int[] pixels, int offset, int scanlength, int x,
            int y, int width, int height, int format);

    /**
     * Gets the pixel values of an image to an array of short values from
     * specific screen location. The pixels will be passed in the format defined
     * by format parameter. If an implementation doesn't support the format an
     * IllegalArgumentException is thrown. Note that it is highly possible that
     * only the native format is supported via appropriate method.
     * <p>
     * This method returns only short based formats. Requesting all other
     * formats will result an IllegalArgumentException.
     * <p>
     * <p>
     * Throws ArrayIndexOutOfBoundsException if array size is too small for
     * image pixels.
     * <p>
     * The getPixels methods stores the pixel data to pixels array in following
     * fashion:
     *
     * <pre>
     * pixels[offset + (x1 - x) + (y1 - y) * scanlength] = P(x1, y1),
     * for each P(x1, y1), where (x &lt;= x1 &lt; x + width) and (y &lt;= y1 &lt; y + height).
     * </pre>
     *
     * <p>
     *
     * @param pixels an array in which the image pixel information will be
     *            stored
     * @param offset the index into the array where the first pixel value will
     *            be stored
     * @param scanlength the relative offset in the array between corresponding
     *            pixels in consecutive rows
     * @param x the x-coordinate of the upper left corner of the region
     * @param y the y-coordinate of the upper left corner of the region
     * @param width the width of the region
     * @param height the height of the region
     * @param format the format in which the pixels are provided in
     *
     * @throws java.lang.NullPointerException if pixels is null
     * @throws java.lang.ArrayIndexOutOfBoundsException if array size is too
     *             small for the image pixels or negative index access is
     *             attempted, contents of the array remain unchanged
     * @throws java.lang.IllegalArgumentException if x, y, width or height is
     *             negative, or format is wrong or unsupported
     */
    public void getPixels(short[] pixels, int offset, int scanlength, int x,
            int y, int width, int height, int format);

    /**
     * Sets the current color to the specified ARGB value. All subsequent
     * rendering operations will use this specified color.
     *
     * @param argbColor the color being set
     * @since 1.0
     */
    public void setARGBColor(int argbColor);

}
