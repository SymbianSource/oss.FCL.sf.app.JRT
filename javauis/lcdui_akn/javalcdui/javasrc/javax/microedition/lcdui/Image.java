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


import java.io.InputStream;
import java.io.IOException;
import javax.microedition.lcdui.game.Sprite;

import com.nokia.mj.impl.rt.legacy.MemoryUtil;

import com.nokia.mj.impl.rt.legacy.NativeError;
import com.nokia.mj.impl.rt.support.Finalizer;

/**
 * Image API class.
 */
public class Image
{
    private static final int ATTRIB_WIDTH  = 0;
    private static final int ATTRIB_HEIGHT = 1;
    private static final int ATTRIB_TYPE   = 2;
    private static final int ATTRIB_SIZE   = 3;
    private static final int ATTRIB_COUNT  = 4;

    static final int TRANSPARENCY_NONE  = 0;
    static final int TRANSPARENCY_MASK  = 1;
    static final int TRANSPARENCY_ALPHA = 2;

    /**
     * Image width, 0 if unknown
     */
    private int     iWidth;

    /**
     * Image height 0 if unknown
     */
    private int     iHeight;

    /**
     * true if image is mutable
     */
    private boolean iMutable;

    /**
     * true if image has transparency
     */
    private int     iTransparency;

    /**
     *
     */
    private int     iSizeBytes;

    /**
     * Peer handle
     */
    int iHandle;

    /**
     * The owning MIDlets toolkit
     */
    Toolkit iToolkit;

    private Finalizer mFinalizer = new Finalizer()
    {
        public void finalizeImpl()
        {
            doFinalize();
        }
    };

    private Image()
    {
        iToolkit = Toolkit.getToolkit();
    }

    /**
     * Create an image wrapping a canvas native frame buffer.
     */
    Image(Canvas aCanvas)
    {
        this();
        checkToolkit(aCanvas.iToolkit);
        final int[] attribs = new int[ATTRIB_COUNT];
        synchronized (iToolkit)
        {
            final int toolkit = iToolkit.getHandle();
            iHandle = Toolkit.checkHandle(_createFromCanvas(toolkit, aCanvas.getContentHandle(), attribs));
        }
        setAttributes(attribs, true);
    }

    private Image(int aWidth, int aHeight, int aColor)
    {
        this();
        final int[] attribs = new int[ATTRIB_COUNT];
        synchronized (iToolkit)
        {
            final int toolkit = iToolkit.getHandle();
            iHandle = Toolkit.checkHandle(_createMutable(toolkit, aWidth, aHeight, aColor, attribs));
        }
        setAttributes(attribs, true);
    }

    private Image(ImageLoader aLoader)
    {
        this();
        checkToolkit(aLoader.iToolkit);
        final int[] attribs = new int[ATTRIB_COUNT];
        synchronized (iToolkit)
        {
            final int toolkit = iToolkit.getHandle();
            iHandle = Toolkit.checkHandle(_createImmutable(toolkit, aLoader.getHandle(), attribs));
        }
        setAttributes(attribs, false);
    }

    /**
     *
     */
    private Image(Image aSource, int aX, int aY, int aWidth, int aHeight, int aTransform)
    {
        this();
        checkToolkit(aSource.iToolkit);
        final int[] attribs = new int[ATTRIB_COUNT];
        synchronized (iToolkit)
        {
            final int toolkit = iToolkit.getHandle();
            final int source  = aSource.getHandle(true);
            iHandle = Toolkit.checkHandle(_createRegion(toolkit, source, aX, aY, aWidth, aHeight, aTransform,attribs));
        }
        setAttributes(attribs, false);
    }

    private Image(int[] aRgbArray, int aWidth, int aHeight, boolean aProcessAlpha)
    {
        this();
        final int[] attribs = new int[ATTRIB_COUNT];
        synchronized (iToolkit)
        {
            final int toolkit = iToolkit.getHandle();
            iHandle = Toolkit.checkHandle(_createRGB(toolkit, aRgbArray, aWidth, aHeight, aProcessAlpha, attribs));
        }
        setAttributes(attribs, false);
    }

    //
    // API
    //
    public static Image createImage(byte[] aData,int aOffset,int aLength)
    {
        final int space = aData.length - aOffset; // throws NullPointerException

        if ((aOffset < 0) || (space < aLength) || (aLength < 0))
        {
            throw new ArrayIndexOutOfBoundsException();
        }

        final Toolkit toolkit = Toolkit.getToolkit();
        ImageLoader loader = new ImageLoader(toolkit);
        try
        {
            loader.append(aData, aOffset, aLength);
            return loadImage(loader);
        }
        catch (Throwable t)
        {
            if (t instanceof OutOfMemoryError)
            {
                throw(OutOfMemoryError)t;
            }
            throw new IllegalArgumentException("bad image format");
        }
        finally
        {
            loader.dispose();
        }
    }

    //
    // API
    //
    public static Image createImage(Image aImage)
    {
        return createImage(aImage, 0, 0, aImage.getWidth(), aImage.getHeight(), 0);
    }

    //
    // API
    //
    public static Image createImage(Image aImage, int aX, int aY, int aWidth, int aHeight, int aTransform)
    {
        final int width  = aImage.getWidth();   // throws NullPointerException
        final int height = aImage.getHeight();

        if (aWidth <= 0 || aHeight <= 0)
        {
            throw new IllegalArgumentException("invalid width or height");
        }

        final int sx1  = aX;            // left column
        final int sx2  = aX + aWidth;   // right column

        final int sy1  = aY;            // top row (inclusive)
        final int sy2  = aY + aHeight;  // bottom row (exclusive)

        //
        // Check source x range lies within source image
        //
        final boolean xRangeError = (sx1 < 0) || (sx1 >= width) || (sx2 < 0) || (sx2 > width);
        final boolean yRangeError = (sy1 < 0) || (sy1 >= height) || (sy2 < 0) || (sy2 > height);

        if (xRangeError || yRangeError)
        {
            final String info = "args=("+aX+','+aY+','+aWidth+','+aHeight+"), rect=("+sx1 +','+sy1+','+sx2+','+sy2+"), image width="+width+",height="+height;
            throw new IllegalArgumentException("createImage: Exceeding bounds of source image: " + info);
        }

        if (aTransform < Sprite.TRANS_NONE || aTransform > Sprite.TRANS_MIRROR_ROT90)
        {
            throw new IllegalArgumentException("invalid transform");
        }

        //
        // Check for trival case for immutable images.
        //
        if ((!aImage.isMutable()) && (aWidth == width) && (aHeight == height) && (aTransform == Sprite.TRANS_NONE))
        {
            return aImage;
        }

        try
        {
            return new Image(aImage, aX, aY, aWidth, aHeight, aTransform);
        }
        catch (OutOfMemoryError oom)
        {
            MemoryUtil.freeNativeMemory();
            return new Image(aImage, aX, aY, aWidth, aHeight, aTransform);
        }
    }

    //
    // API
    //
    public static Image createImage(InputStream aStream) throws java.io.IOException
    {
        if (null == aStream)
        {
            throw new NullPointerException();
        }

        return createImageFromStream(aStream);
    }

    //
    // API
    //
    public static Image createImage(int aWidth,int aHeight)
    {
        if (aWidth <= 0 || aHeight <= 0)
        {
            throw new IllegalArgumentException();
        }

        try
        {
            return new Image(aWidth, aHeight, 0xffffffff);
        }
        catch (OutOfMemoryError oom)
        {
            MemoryUtil.freeNativeMemory();
            return new Image(aWidth, aHeight, 0xffffffff);
        }
    }

    //
    // API
    //
    public static Image createImage(String aName) throws java.io.IOException
    {
        if (aName == null)
        {
            throw new NullPointerException();
        }

        final Toolkit toolkit = Toolkit.getToolkit();

        InputStream stream = toolkit.getResourceAsStream(aName);
        if (stream == null)
        {
            // Retry that allows using relative name when image is in root level
            stream = toolkit.getResourceAsStream("/"+aName);
            if (stream == null)
            {
                throw new IOException();
            }
        }

        try
        {
            return createImageFromStream(stream);
        }
        finally
        {
            stream.close();
        }
    }

    public static Image createRGBImage(int[] aRgb, int aWidth, int aHeight, boolean aProcessAlpha)
    {
        if (aRgb.length < (aWidth * aHeight))
        {
            throw new ArrayIndexOutOfBoundsException();
        }

        if (aWidth <= 0 || aHeight <= 0)
        {
            throw new IllegalArgumentException();
        }

        try
        {
            return new Image(aRgb, aWidth, aHeight, aProcessAlpha);
        }
        catch (OutOfMemoryError oom)
        {
            MemoryUtil.freeNativeMemory();
            return new Image(aRgb, aWidth, aHeight, aProcessAlpha);
        }
    }


    private static Image createImageFromStream(InputStream aStream) throws java.io.IOException
    {
        //
        // maximum size of blocks to read from stream
        //
        final int    blocksize = 4096;
        final byte[] buffer    = new byte[blocksize];

        //
        // create image loader to accept encoded image data.
        //
        final ImageLoader loader = new ImageLoader(Toolkit.getToolkit());
        try
        {
            int bytesRead = 0;

            //
            // read blocks from stream and append to decoder buffer
            //
            while ((bytesRead = aStream.read(buffer, 0, buffer.length)) != -1)
            {
                if (loader.append(buffer, 0, bytesRead) == false)
                    throw new IOException("loader.iHandle null: can't append more data");
            }

            return loadImage(loader);
        }
        catch (Throwable t)
        {
            if (t instanceof OutOfMemoryError)
            {
                throw(OutOfMemoryError)t;
            }
            throw new IOException();
        }
        finally
        {
            loader.dispose();
        }
    }

    private static Image loadImage(ImageLoader aLoader)
    {
        //
        // This is where the image is first decoded and a native representation allocated.
        // It is more likely to fail than the image instantiation, but more risky to retry.
        //

        //
        // REMOVED  RETRY around the following line as it caused performance unit test
        // to panic.
        //
        aLoader.decode();

        //
        // This is where device specific bitmaps are created if the image was decoded in
        // the wrong format. This is less likely to OOM than the decode above - but can
        // be retried.
        //
        try
        {
            return new Image(aLoader);
        }
        catch (OutOfMemoryError oom)
        {
            MemoryUtil.freeNativeMemory();
            return new Image(aLoader);
        }
    }

    public void getRGB(int[] aRgbData,int aOffset,int aScanLength,int aX,int aY,int aWidth,int aHeight)
    {
        //
        // Image bounds
        //
        if (aWidth <= 0 || aHeight <= 0)
        {
            return;
        }

        //
        // Array bounds
        //
        final long limit = (long)(aRgbData.length) - 1; // throws NullPointerException

        //
        // aScanLength may be negative in which case rows are written
        // into the buffer in reverse order.
        //
        long start;
        long end;

        if (aScanLength > 0)
        {
            start = (long)aOffset;
            end   = (long)aOffset + (long)aScanLength*((long)aHeight-1) + (long)aWidth - 1;
        }
        else
        {
            end   = (long)aOffset + (long)aWidth - 1;
            start = (long)aOffset + (long)aScanLength*((long)aHeight-1);
        }

        //
        // aScanLength may be < 0, hence end < start is valid.
        //
        if ((start < 0) || (start > limit) || (end < 0) || (end > limit))
        {
            throw new ArrayIndexOutOfBoundsException("destination range exceeds array bounds");
        }

        final int width  = iWidth;
        final int height = iHeight;

        final int sx1  = aX;            // left column
        final int sx2  = aX + aWidth;   // right column

        final int sy1  = aY;            // top row (exclusive)
        final int sy2  = aY + aHeight;  // bottom row (exclusive)

        //
        // Check source x range lies within source image
        //
        final boolean xRangeError = (sx1 < 0) || (sx1 >= width) || (sx2 < 0) || (sx2 > width);
        final boolean yRangeError = (sy1 < 0) || (sy1 >= height) || (sy2 < 0) || (sy2 > height);

        if (xRangeError || yRangeError)
        {
            final String info = "args=("+aX+','+aY+','+aWidth+','+aHeight+"), rect=("+sx1 +','+sy1+','+sx2+','+sy2+"), image width="+width+",height="+height;
            throw new IllegalArgumentException("getRGB: Exceeding bounds of source image: " + info);
        }
        final int scanLength = Math.abs(aScanLength);

        if (scanLength < aWidth)
        {
            throw new IllegalArgumentException("abs(Scanlength) should be >= width");
        }
        synchronized (iToolkit)
        {
            final int toolkit = iToolkit.getHandle();
            final int handle  = getHandle(true);

            NativeError.check(_getPixels(toolkit, handle, aRgbData, aOffset, aScanLength, aX, aY, aWidth, aHeight));
        }
    }

    /**
     *@returns a newly allocated thread-safe Graphics instance.
     */
    public Graphics getGraphics()
    {
        if (!isMutable())
        {
            throw new IllegalStateException();
        }
        return new Graphics(iToolkit, this, getHandle(false), iWidth, iHeight);
    }

    public int getWidth()
    {
        return iWidth;
    }

    public int getHeight()
    {
        return iHeight;
    }

    public boolean isMutable()
    {
        return iMutable;
    }

    /**
     *@return true if this image has transparent pixels.
     *
     * May be used in game package to avoid native calls in collision
     * detection.
     */
    final boolean hasTransparency()
    {
        return (TRANSPARENCY_NONE != iTransparency);
    }

    /**
     * Set the image transparency type.
     *
     * Added to support invalid requirement of Nokia UI API - infact API only
     * requires construction of mutable images with transparency - not after
     * the fact transformation of image transparency type.
     */
    void setTransparency(int aTransparency)
    {
        if (aTransparency != TRANSPARENCY_ALPHA)
        {
            throw new IllegalArgumentException();
        }
        synchronized (iToolkit)
        {
            NativeError.check(_setTransparencyType(iToolkit.getHandle(), getHandle(true), aTransparency));
            iTransparency = aTransparency;
        }
    }

    /**
     *@return the peer handle, optionally flushing and syncing the graphics buffer.
     *
     * If pixel data from mutable images is accessed natively without syncing the
     * graphics buffer there is a chance that commands that would effect the
     * pixels read have not yet been executed.
     */
    final int getHandle(boolean aSync)
    {
        if (aSync && isMutable())
        {
            iToolkit.sync();
        }
        return iHandle;
    }

    /**
     * destroy native image.
     */
    private void dispose()
    {
        boolean freed = false;
        synchronized (iToolkit)
        {
            if (iHandle != 0)
            {
                iToolkit.disposeObject(iHandle);
                iHandle = 0;
                freed   = true;
            }
        }
        if (freed)
        {
            MemoryUtil.nativeMemoryFreed(iSizeBytes);
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

    static boolean collision
    (
        Image aImage1, int aS1X1, int aS1Y1, int aS1X2, int aS1Y2, int aTransform1, int aD1X, int aD1Y,
        Image aImage2, int aS2X1, int aS2Y1, int aS2X2, int aS2Y2, int aTransform2, int aD2X, int aD2Y
    )
    {
        final int result;
        synchronized (aImage1.iToolkit)
        {
            final int toolkit = aImage1.iToolkit.getHandle();
            final int image1 = aImage1.getHandle(false);
            final int image2 = aImage2.getHandle(false);
            result = _collision(
                         toolkit,
                         image1, aS1X1, aS1Y1, aS1X2, aS1Y2, aTransform1, aD1X, aD1Y,
                         image2, aS2X1, aS2Y1, aS2X2, aS2Y2, aTransform2, aD2X, aD2Y
                     );
        }
        return (result > 0);
    }


    /**
     * copies pixels from source image to destination image, transforming as required.
     */
    private void copyRegion(Image aSource, int aX, int aY, int aWidth, int aHeight, int aTransform)
    {
        synchronized (iToolkit)
        {
            final int toolkit = iToolkit.getHandle();
            final int target = getHandle(false);
            final int source = aSource.getHandle(true);
            NativeError.check(_copyRegion(toolkit, target, source, aX, aY, aWidth, aHeight, aTransform));
        }
    }

    /**
     * writes pixels from aArray[0..(aWidth*aHeight-1)] into image, converting from
     * ARGB format to the image pixel format.
     */
    private void setRGB(int[] aRgbArray, int aOffset, int aScanLength, int aX, int aY, int aWidth, int aHeight, boolean aAlpha)
    {
        synchronized (iToolkit)
        {
            final int toolkit = iToolkit.getHandle();
            final int handle  = getHandle(false);
            NativeError.check(_setPixels(toolkit, handle, aRgbArray, aOffset, aScanLength, aX, aY ,aWidth, aHeight, aAlpha));
        }
    }

    private void setAttributes(int[] aAttribs, boolean aMutable)
    {
        iWidth        = aAttribs[ATTRIB_WIDTH];
        iHeight       = aAttribs[ATTRIB_HEIGHT];
        iTransparency = aAttribs[ATTRIB_TYPE];
        iSizeBytes    = aAttribs[ATTRIB_SIZE];
        iMutable      = aMutable;
        MemoryUtil.nativeMemoryAllocated(iSizeBytes);
    }

    private void checkToolkit(Toolkit aToolkit)
    {
        if (aToolkit != iToolkit)
        {
            throw new RuntimeException("Toolkit mismatch");
        }
    }

    /**
     * Native constructor.
     *@param aToolkit   pointer to native toolkit instance
     *@param aWidth     image width, may be zero if unknown
     *@param aHeight    image height, may be zero if unknown
     *@param aColor     image initial color, may be (semi) transparent
     *@param aAttribs returns image attribs [width, height, transparency]
     */
    private native int _createMutable(int aToolkit, int aWidth, int aHeight, int aColor, int[] aAttribs);

    /**
     *@param aToolkit handle to native toolkit instance
     *@param aLoader handle to image loader containing decoded pixel data.
     *@param aAttribs returns image attribs [width, height, transparency]
     */
    private native int _createImmutable(int aToolkit, int aLoader, int[] aAttribs);

    /**
     *@param aToolkit handle
     *@param aCanvas handle of canvas with framebuffer bitmap
     *@param aAttribs returns image attribs [width, height, transparency]
     */
    private native int _createFromCanvas(int aToolkit, int aCanvas, int[] aAttribs);

    /**
     *@param aToolkit toolkit handle
     *@param aSource  source image handle
     *@param aX       source region x
     *@param aY       source region y
     *@param aWidth   source region width
     *@param aHeight  source region height
     *@param aTransform transform to apply to source region
     *@param aAttribs returns image attribs [width, height, transparency]
     */
    private native int _createRegion(int aToolkit, int aSource, int aX, int aY, int aWidth, int aHeight, int aTransform, int[] aAttribs);

    /**
     *@param aToolkit toolkit handle
     *@param aArray   pixel data array
     *@param aWidth   pixel data width
     *@param aHeight  pixel data height
     *@param aAlpha   true if alpha component of pixel data is to be interpreted.
     *@param aAttribs returns image attribs [width, height, transparency]
     */
    private native int _createRGB(int aToolkit, int[] aArray, int aWidth, int aHeight, boolean aAlpha, int[] aAttribs);

    /**
     *
     */
    private native int _copyRegion(int aToolkit, int aTarget,  int aSource, int aX, int aY, int aWidth, int aHeight, int aTransform);

    /**
     *
     */
    private native int _setPixels(int aToolkit, int aHandle, int[] aRgbArray, int aOffset, int aScanLength, int aX, int aY, int aWidth, int aHeight, boolean aAlpha);

    /**
     *
     */
    private native int _getPixels(int aToolkit, int aHandle, int[] aRgbArray, int aOffset, int aScanlength, int aX, int aY, int aWidth, int aHeight);

    /**
     *@return -ve on error, 1 for true 0 for false.
     */
    private static native int _collision
    (
        int aToolkit,
        int aImage1, int aS1X1, int aS1Y1, int aS1X2, int aS1Y2, int aTransform1, int aD1X, int aD1Y,
        int aImage2, int aS2X1, int aS2Y1, int aS2X2, int aS2Y2, int aTransform2, int aD2X, int aD2Y
    );

    private native int _setTransparencyType(int aToolkit, int aImage, int aType);
}
