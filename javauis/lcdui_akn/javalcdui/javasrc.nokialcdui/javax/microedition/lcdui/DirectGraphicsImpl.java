/*
* Copyright (c) 2000-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides graphics extensions to MIDP Graphics.
*
*/


package javax.microedition.lcdui;

import com.nokia.mj.impl.rt.support.Finalizer;

/**
 * DirectGraphicsImpl contains some graphics extensions to MIDP Graphics:
 * drawing triangles, drawing manipulated images (rotated, flipped) and drawing
 * raw pixel array directly to graphics context.
 * <p>
 * {@link DirectUtils#getDirectGraphicsImpl(Graphics g)} method can be used to
 * convert standard Graphics to proprietary DirectGraphicsImpl object:
 * <pre>
 * DirectGraphicsImpl dg = DirectUtils.getDirectGraphicsImpl(g);
 * </pre>
 * The operations to DirectGraphicsImpl reference will affect also original graphics
 * context, for example, setting color of DirectGraphicsImpl will change current
 * painting color of Graphics and vice versa. In fact developer can see
 * DirectGraphicsImpl as a new way to do call to Graphics.
 * DirectGraphicsImpl doesn't inherit standard Graphics because of API dependency
 * reasons.
 * <p>
 * ARGB values used with some methods of this interface are interpreted with the least
 * significant eight bits giving the blue component, the next eight more
 * significant bits giving the green component, the next eight more significant
 * bits giving the red component and the next eight more significant
 * bits giving the alpha component. That is to say, the color component is
 * specified in the form of 0xAARRGGBB. This correspons to native format specified by
 * {@link #TYPE_INT_8888_ARGB}.
 * <p>
 * Manipulation parameters to drawPixels are formed by bitwise ORring FLIP_HORIZONTAL
 * or FLIP_VERTICAL with a degree value: ROTATE_90, ROTATE_180 or ROTATE_270. Currently
 * only these fixed rotation values are supported. The image is rotated counter-clockwise.
 */

class DirectGraphicsImpl implements com.nokia.mid.ui.DirectGraphics
{
    private int iNativePixelFormat;
    private int iAlphaComponent = 0;

    // primitives
    private static final int OP_SET_COLOR    = 0;
    private static final int DRAW_TRIANGLE = 102;
    private static final int FILL_TRIANGLE = 103;
    private static final int DIRECTDRAWIMAGE = 104;

    private static final int IMAGE_MASK = Graphics.HCENTER | Graphics.VCENTER |
                                          Graphics.LEFT | Graphics.RIGHT |
                                          Graphics.TOP | Graphics.BOTTOM;
    private static final int HMASK = Graphics.HCENTER | Graphics.LEFT |
                                     Graphics.RIGHT;
    private static final int VMASK = Graphics.VCENTER | Graphics.TOP |
                                     Graphics.BOTTOM | Graphics.BASELINE;

    // error codes, same values must be in CMIDDirectGraphics.h
    private static final int ERR_ILLEGAL_ARGUMENT_EXCEPTION = -11;
    private static final int ERR_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION = -12;
    private static final int ERR_NULL_POINTER_EXCEPTION = -13;
    private static final int ERROR = -3;
    private static final int NO_ERROR = 0;

    // this is native bitmap format which uses 32 bit separate alpha channel
    private static final int TYPE_INT_888A_RGB = 32888;

    private int iToolkitHandle;
    private Toolkit iToolkit;
    private int iBufferProcessor; // processor for buffered commands
    private int iImageHandle; // image where we draw to, or NULL if canvas
    Graphics iGraphics; // Graphics, which this object is based on
    Buffer iBuffer;     // Buffer where we write commands to

    private Finalizer mFinalizer = new Finalizer()
    {
        public void finalizeImpl()
        {
            doFinalize();
        }
    };


    public DirectGraphicsImpl(Graphics aGraphics)
    {
        iGraphics = aGraphics;
        iToolkit = Toolkit.getToolkit();
        iBuffer = iToolkit.iBuffer;
        iToolkitHandle = iToolkit.getHandle();
        ToolkitInvoker invoker =
            (ToolkitInvoker)ToolkitInvoker.getToolkitInvoker();
        try
        {
            iImageHandle = invoker.graphicsGetMutableImageHandle(aGraphics);
        }
        catch (RuntimeException e)
        {
            // if the graphics does not have an image, the invoker will throw
            // RuntimeException. We can ignore it and use default (null)
            // for image handle
        } // end of try-catch

        iBufferProcessor = _createBufferProcessor(iToolkitHandle,
                           iGraphics.iHandle,
                           iImageHandle);
        if (iBufferProcessor < 0)
        {
            throw new OutOfMemoryError();
        }
        iGraphics.iDirectGraphicsHandle = iBufferProcessor;
        // getNativePixelFormat may return negative values and it is not
        // an error.
        iNativePixelFormat = _getNativePixelFormat(iBufferProcessor,
                             iToolkitHandle);
        iGraphics.clipRect(iGraphics.getClipX(), iGraphics.getClipY(),
                           iGraphics.getClipWidth(),
                           iGraphics.getClipHeight());

        // Also Graphics class uses the buffer, so sync() needs to
        // be synchronized.
        synchronized (iBuffer)
        {
            iBuffer.sync();
        }
    }

    public void setARGBColor(int aArgbColor)
    {
        iGraphics.setColor(aArgbColor);
        synchronized (iBuffer)
        {
            iBuffer.write(iGraphics.iHandle, OP_SET_COLOR,
                          aArgbColor);
        }
        iAlphaComponent = aArgbColor >>> 24;
    }

    public void drawImage(Image aImage, int aX, int aY, int aAnchor,
                          int aManipulation)
    {
        // We don't have to test whether aImage is null or not, as the VM
        // will throw NullPointerException when we try to access aImage.iHandle
        // while calling _drawImage.
        // aAnchor is checked here, rest of the parameters are checked in the
        // native side by the bitmap drawing code.
        checkAnchor(aAnchor, IMAGE_MASK);

        checkManipulation(aManipulation);

        synchronized (iBuffer)
        {
            iBuffer.write(iBufferProcessor,
                          DIRECTDRAWIMAGE,
                          aImage.iHandle,
                          aX + iGraphics.getTranslateX(),
                          aY + iGraphics.getTranslateY(),
                          aAnchor,
                          aManipulation);
        }

    }


    public void drawTriangle(int aX1, int aY1, int aX2, int aY2, int aX3,
                             int aY3,
                             int aArgbColor)
    {
        int transX = iGraphics.getTranslateX();
        int transY = iGraphics.getTranslateY();
        iBuffer.write(
            iBufferProcessor,
            DRAW_TRIANGLE,
            aX1 + transX,
            aY1 + transY,
            aX2 + transX,
            aY2 + transY,
            aX3 + transX,
            aY3 + transY,
            aArgbColor
        );

    }

    public void fillTriangle(int aX1, int aY1, int aX2, int aY2, int aX3,
                             int aY3,
                             int aArgbColor)
    {
        int transX = iGraphics.getTranslateX();
        int transY = iGraphics.getTranslateY();
        iBuffer.write(
            iBufferProcessor,
            FILL_TRIANGLE,
            aX1 + transX,
            aY1 + transY,
            aX2 + transX,
            aY2 + transY,
            aX3 + transX,
            aY3 + transY,
            aArgbColor
        );
    }

    public void drawPolygon(int[] aXPoints,
                            int aXOffset,
                            int[] aYPoints,
                            int aYOffset,
                            int aNPoints,
                            int aArgbColor)
    {
        if ((null == aXPoints) || (null == aYPoints))
        {
            throw new java.lang.NullPointerException();
        }
        flush();
        int err = _drawPolygon(aXPoints, aXOffset, iGraphics.getTranslateX(),
                               aYPoints, aYOffset, iGraphics.getTranslateY(),
                               aNPoints, aArgbColor, iBufferProcessor, false,
                               iToolkitHandle);
        handlePolygonError(err);
    }


    public void fillPolygon(int[] aXPoints,
                            int aXOffset,
                            int[] aYPoints,
                            int aYOffset,
                            int aNPoints,
                            int aArgbColor)
    {
        if ((null == aXPoints) || (null == aYPoints))
        {
            throw new java.lang.NullPointerException();
        }
        flush();
        int err = _drawPolygon(aXPoints, aXOffset, iGraphics.getTranslateX(),
                               aYPoints, aYOffset, iGraphics.getTranslateY(),
                               aNPoints, aArgbColor, iBufferProcessor, true,
                               iToolkitHandle);
        handlePolygonError(err);
    }



    public void drawPixels(int aPixels[], boolean aTransparency, int aOffset,
                           int aScanlength, int aX, int aY,
                           int aWidth, int aHeight,
                           int aManipulation, int aFormat)
    {
        // null pointer is tested here, as it would not get through the
        // native interface
        if (aPixels == null)
        {
            //java.lang.NullPointerException - if pixels is null
            throw new java.lang.NullPointerException();
        }
        // These tests are here, because the same DrawBitmap method is
        // used for every drawPixels and drawImage method calls on the
        // native side, and they would not make sense there. Rest of
        // the errors are same for all the methods and therefore it is
        // better to check them on the native side.
        if (aFormat != TYPE_INT_888_RGB &&
                aFormat != TYPE_INT_8888_ARGB)
        {
            throw new java.lang.IllegalArgumentException();
        }
        if (aWidth < 0 || aHeight < 0)
        {
            throw new java.lang.IllegalArgumentException();
        }
        if (aScanlength < 0)
        {
            throw new java.lang.ArrayIndexOutOfBoundsException();
        }

        checkManipulation(aManipulation);

        flush();

        int err = _drawBitmapInt(aPixels, aTransparency, aOffset, aScanlength,
                                 aX + iGraphics.getTranslateX(),
                                 aY + iGraphics.getTranslateY(),
                                 aWidth, aHeight,
                                 aManipulation, aFormat, iBufferProcessor,
                                 iToolkitHandle);

        if (err < NO_ERROR)
        {
            handlePixelError(err);
        }
    }

    public void drawPixels(byte aPixels[], byte[] aTransparencyMask,
                           int aOffset,
                           int aScanlength, int aX, int aY, int aWidth,
                           int aHeight, int aManipulation, int aFormat)
    {
        if (aPixels == null)
        {
            //java.lang.NullPointerException - if pixels is null
            throw new java.lang.NullPointerException();
        }
        if (aFormat != TYPE_BYTE_1_GRAY &&
                aFormat != TYPE_BYTE_1_GRAY_VERTICAL)
        {
            throw new java.lang.IllegalArgumentException();
        }
        if (aWidth < 0 || aHeight < 0)
        {
            throw new java.lang.IllegalArgumentException();
        }
        if (aScanlength < 0)
        {
            throw new java.lang.ArrayIndexOutOfBoundsException();
        }

        checkManipulation(aManipulation);

        flush();
        int err = _drawBitmapByte(aPixels, aTransparencyMask, aOffset,
                                  aScanlength,
                                  aX + iGraphics.getTranslateX(),
                                  aY + iGraphics.getTranslateY() ,
                                  aWidth, aHeight,
                                  aManipulation, aFormat, iBufferProcessor,
                                  iToolkitHandle);
        if (err < NO_ERROR)
        {
            handlePixelError(err);
        }
    }



    public void drawPixels(short aPixels[], boolean aTransparency, int aOffset,
                           int aScanlength,
                           int aX, int aY, int aWidth, int aHeight,
                           int aManipulation, int aFormat)
    {
        if (aPixels == null)
        {
            //java.lang.NullPointerException - if pixels is null
            throw new java.lang.NullPointerException();
        }
        if (aFormat != TYPE_USHORT_444_RGB  &&
                aFormat != TYPE_USHORT_4444_ARGB &&
                aFormat != TYPE_USHORT_565_RGB)
        {
            throw new java.lang.IllegalArgumentException();
        }
        if (aWidth < 0 || aHeight < 0)
        {
            throw new java.lang.IllegalArgumentException();
        }
        if (aScanlength < 0)
        {
            throw new java.lang.ArrayIndexOutOfBoundsException();
        }

        checkManipulation(aManipulation);

        flush();
        int err = _drawBitmapShort(aPixels, aTransparency, aOffset, aScanlength,
                                   aX + iGraphics.getTranslateX(),
                                   aY + iGraphics.getTranslateY(),
                                   aWidth, aHeight,
                                   aManipulation, aFormat, iBufferProcessor ,
                                   iToolkitHandle);

        if (err < NO_ERROR)
        {
            handlePixelError(err);
        }
    }


    public void getPixels(int[] aPixels, int aOffset, int aScanlength, int aX,
                          int aY, int aWidth, int aHeight, int aFormat)
    {
        if (aPixels == null)
        {
            // throws java.lang.NullPointerException if pixels is null
            throw new java.lang.NullPointerException();
        }
        if (aFormat != TYPE_INT_888_RGB &&
                aFormat != TYPE_INT_8888_ARGB)
        {
            throw new java.lang.IllegalArgumentException();
        }


        flush();

        int err = _getPixelsInt(aPixels, aOffset, aScanlength,
                                aX + iGraphics.getTranslateX(),
                                aY + iGraphics.getTranslateY(),
                                aWidth, aHeight, aFormat, iBufferProcessor,
                                iToolkitHandle);

        if (err < NO_ERROR)
        {
            handlePixelError(err);
        }
    }


    public void getPixels(byte[] aPixels, byte[] aTransparencyMask, int aOffset,
                          int aScanlength, int aX, int aY, int aWidth, int aHeight,
                          int aFormat)
    {
        if (aPixels == null)
        {
            // throws java.lang.NullPointerException if pixels is null
            throw new java.lang.NullPointerException();
        }
        if (aFormat != TYPE_BYTE_1_GRAY &&
                aFormat != TYPE_BYTE_1_GRAY_VERTICAL)
        {
            throw new java.lang.IllegalArgumentException();
        }

        flush();

        int err = _getPixelsByte(aPixels, aTransparencyMask, aOffset,
                                 aScanlength, aX + iGraphics.getTranslateX(),
                                 aY + iGraphics.getTranslateY(),
                                 aWidth, aHeight, aFormat, iBufferProcessor,
                                 iToolkitHandle);

        if (err < NO_ERROR)
        {
            handlePixelError(err);
        }
    }


    public void getPixels(short[] aPixels, int aOffset, int aScanlength,
                          int aX, int aY, int aWidth, int aHeight, int aFormat)
    {
        if (aPixels == null)
        {
            // throws java.lang.NullPointerException if pixels is null
            throw new java.lang.NullPointerException();
        }
        if (aFormat != TYPE_USHORT_444_RGB  &&
                aFormat != TYPE_USHORT_4444_ARGB &&
                aFormat != TYPE_USHORT_565_RGB)
        {
            throw new java.lang.IllegalArgumentException();
        }
        flush();

        int err = _getPixelsShort(aPixels, aOffset, aScanlength,
                                  aX + iGraphics.getTranslateX(),
                                  aY + iGraphics.getTranslateY(),
                                  aWidth, aHeight, aFormat, iBufferProcessor,
                                  iToolkitHandle);

        if (err < NO_ERROR)
        {
            handlePixelError(err);
        }
    }

    public int getNativePixelFormat()
    {
        if (iNativePixelFormat == TYPE_INT_888A_RGB)
        {
            return TYPE_INT_888_RGB;
        }
        return iNativePixelFormat;
    }

    public int getAlphaComponent()
    {
        return iAlphaComponent;
    }

    private void checkAnchor(int aAnchor, int aMask)
    {
        if ((aAnchor & ~aMask) != 0 || !isSingleBit(aAnchor&VMASK)
                || !isSingleBit(aAnchor&HMASK))
        {
            throw new IllegalArgumentException();
        }
    }

    // This method returns false if there are more than one 1-bits.
    // Note: 0 returns true
    private boolean isSingleBit(int aAnchor)
    {
        // when one is substracted from a number, the rightmost
        // 1-bit is changed to zero, and the 0-bits from that bit to the right
        // are changed to ones. All the bits which are left from the
        // rightmost 1-bit are left unchanged. if there are more than
        // one 1-bit in the value, all but the rightmost are the same
        // after the substraction, and the and -operation returns something
        // else than zero.
        return((aAnchor & (aAnchor - 1)) == 0);
    }

    private void flush()
    {
        // Also Graphics class uses the buffer, so sync() needs to
        // be synchronized.
        synchronized (iBuffer)
        {
            iBuffer.sync();
        }
    }

    private void checkManipulation(int aManipulation)
    {
        int rotates = aManipulation & 0x0FFF;

        if ((aManipulation < 0) ||
                ( // rotates
                    (rotates != ROTATE_90) &&
                    (rotates != ROTATE_180) &&
                    (rotates != ROTATE_270) &&
                    (rotates != 0)) ||
                // flips
                ((aManipulation &
                  (0x0FFF + FLIP_HORIZONTAL + FLIP_VERTICAL)) != aManipulation))
        {
            throw new IllegalArgumentException("Illegal manipulation");
        }
    }

    private void handlePolygonError(int aErrorCode)
    {
        if (aErrorCode == ERR_ILLEGAL_ARGUMENT_EXCEPTION)
        {
            throw new java.lang.IllegalArgumentException();
        }
        if (aErrorCode == ERR_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION)
        {
            throw new java.lang.ArrayIndexOutOfBoundsException();
        }
        if (aErrorCode == ERR_NULL_POINTER_EXCEPTION)
        {
            throw new java.lang.NullPointerException();
        }
    }

    private void handlePixelError(int aErrorCode)
    {
        if (aErrorCode == ERR_ILLEGAL_ARGUMENT_EXCEPTION ||
                aErrorCode == ERROR)
        {
            //java.lang.IllegalArgumentException - if manipulation is unknown,
            //width or height is negative, or format is wrong or unsupported
            throw new java.lang.IllegalArgumentException();
        }
        if (aErrorCode == ERR_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION)
        {
            //java.lang.ArrayIndexOutOfBoundsException -
            //if requested to access beyond the length of array or with
            //negative index
            throw new java.lang.ArrayIndexOutOfBoundsException();
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

    final protected void registeredFinalize()
    {
        iToolkit.disposeObject(iBufferProcessor);
    }

    void setColor(int aColor, Image aImage)
    {
        _setColor(aColor, iBufferProcessor, iToolkitHandle, aImage.iHandle);
    }

    private native int _setColor(int aColor, int aBufferProcessorHandle,
                                 int aToolkitHandle, int aImageHandle);

    private native int _getNativePixelFormat(int aBufferProcessorHandle,
            int aToolkitHandle);

    private native int _drawPolygon(int[] aXPoints, int aXOffset, int aTransX,
                                    int[] aYPoints, int aYOffset, int aTransY,
                                    int aNPoints, int aARGBColor,
                                    int aBufferProcessorHandle,
                                    boolean aFill, int aToolkitHandle);

    private native int _drawBitmapInt(int aPixels[], boolean aTransparency,
                                      int aOffset,
                                      int aScanlength, int aX, int aY,
                                      int aWidth, int aHeight,
                                      int aManipulation, int aFormat,
                                      int aBufferProcessorHandle ,
                                      int aToolkitHandle);

    private native int _drawBitmapByte(byte aPixels[],
                                       byte aTransparencyMask[], int aOffset,
                                       int aScanlength, int aX, int aY,
                                       int aWidth, int aHeight,
                                       int aManipulation, int aFormat,
                                       int aBufferProcessorHandle ,
                                       int aToolkitHandle);

    private native int _drawBitmapShort(short aPixels[],
                                        boolean aTransparency, int aOffset,
                                        int aScanlength, int aX, int aY,
                                        int aWidth, int aHeight,
                                        int aManipulation, int aFormat,
                                        int aBufferProcessorHandle ,
                                        int aToolkitHandle);


    private native int _getPixelsInt(int[] aPixels, int aOffset,
                                     int aScanlength, int aX, int aY,
                                     int aWidth, int aHeight, int aFormat,
                                     int aBufferProcessorHandle,
                                     int aToolkitHandle);

    private native int _getPixelsByte(byte aPixels[], byte aTranparencyMask[],
                                      int aOffset,
                                      int aScanlength, int aX, int aY,
                                      int aWidth, int aHeight, int aFormat,
                                      int aBufferProcessorHandle,
                                      int aToolkitHandle);

    private native int _getPixelsShort(short[] aPixels, int aOffset,
                                       int aScanlength, int aX, int aY,
                                       int aWidth, int aHeight, int aFormat,
                                       int aBufferProcessorHandle,
                                       int aToolkitHandle);

    private native int _createBufferProcessor(int aToolkitHandle,
            int aGraphicsHandle,
            int aImageHandle);
}
