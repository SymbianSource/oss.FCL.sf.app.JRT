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
package javax.microedition.lcdui;

import java.io.*;

import javax.microedition.lcdui.game.Sprite;

import org.eclipse.swt.graphics.Internal_GfxPackageSupport;

/**
 * Class containing graphical data of image.<br>
 * <br>
 * Image path handling is not implemented correctly, for example "standard"
 * LCDUI accepts path "/TestMIDlets/images/100x100.png" but corresponding path
 * with this image-class must be "/100x100.png".
 */
public class Image
{

    private static Image createImageReturnValue;

    private org.eclipse.swt.graphics.Image eswtImage;
    private int eswtImageWidth;
    private int eswtImageHeight;
    private boolean mutable;
    private com.nokia.mj.impl.rt.support.Finalizer finalizer;

    // buffer has package visibility so that it can be used as
    // a lock in other classes
    ImageBuffer graphicsBuffer;
    
    // Graphics for transferring instance
    // between application and UI thread
    Graphics tempGraphics;

    /**
     * Constructor.
     */
    Image(org.eclipse.swt.graphics.Image eswtImage, boolean isMutable)
    {
        this.eswtImage = eswtImage;
        this.mutable = isMutable;
        eswtImageWidth = eswtImage.getBounds().width;
        eswtImageHeight = eswtImage.getBounds().height;
        graphicsBuffer = new ImageBuffer(this);
        finalizer = ((finalizer != null) ? finalizer
                     : new com.nokia.mj.impl.rt.support.Finalizer()
        {
            public void finalizeImpl()
            {
                if(finalizer != null)
                {
                    finalizer = null;
                    if(!ESWTUIThreadRunner.isDisposed())
                    {
                        dispose();
                    }
                }
            }
        });
        ESWTUIThreadRunner.update(getClass().getName(), 1);
    }

    /**
     * Called by finalizer when Image is ready to free its resources.
     */
    void dispose()
    {
        ESWTUIThreadRunner.update(getClass().getName(), -1);
        ESWTUIThreadRunner.safeSyncExec(new Runnable()
        {
            public void run()
            {
                if(eswtImage != null)
                {
                    eswtImage.dispose();
                    eswtImage = null;
                }
                graphicsBuffer.dispose();
            }
        });
    }

    /**
     * Return eSWT Image object.
     */
    org.eclipse.swt.graphics.Image getESWTImage()
    {
        return eswtImage;
    }

    /**
     * Return eSWT Image object.
     *
     * @param img Image
     * @return eSWT Image
     */
    static org.eclipse.swt.graphics.Image getESWTImage(Image img)
    {
        if(img != null)
        {
            return img.eswtImage;
        }
        return null;
    }

    /**
     * Creates new mutable image where every pixel is white.
     *
     * @param w The width of the image to be created.
     * @param h The height of the image to be created.
     * @return Image created.
     * @throws IllegalArgumentException if width or height are zero or less.
     */
    public static Image createImage(int w, int h)
    {
        if(w <= 0 || h <= 0)
        {
            throw new IllegalArgumentException(
                MsgRepository.IMAGE_EXCEPTION_INVALID_DIMENSIONS);
        }

        final int width = w;
        final int height = h;
        ESWTUIThreadRunner.safeSyncExec(new Runnable()
        {
            public void run()
            {
                org.eclipse.swt.graphics.Image eswtImg =
                    eswtCreateImage(width, height);

                if(eswtImg != null)
                {
                    createImageReturnValue = new Image(eswtImg, true);
                }
                else
                {
                    createImageReturnValue = null;
                    throw new OutOfMemoryError();
                }
            }
        });
        return createImageReturnValue;
    }

    /**
     * Creates an immutable image with the specified size from the existing
     * image source.
     *
     * @param srcImage the source of image.
     * @param width the width of the new image.
     * @param height the height of the new image.
     * @return the resized Image.
     * @throws NullPointerException if aImage is null.
     */
    static Image createImage(Image srcImage, int width, int height)
    {
        if(srcImage == null)
        {
            throw new NullPointerException(
                MsgRepository.IMAGE_EXCEPTION_IS_NULL);
        }
        final Image finalImage = srcImage;
        final int finalWidth = width;
        final int finalHeight = height;
        ESWTUIThreadRunner.safeSyncExec(new Runnable()
        {
            public void run()
            {
                org.eclipse.swt.graphics.Image eswtImg =
                    eswtCreateImage(finalImage.eswtImage, finalWidth, finalHeight);

                if(eswtImg != null)
                {
                    createImageReturnValue = new Image(eswtImg, false);
                }
                else
                {
                    createImageReturnValue = null;
                    throw new OutOfMemoryError();
                }
            }
        });
        return createImageReturnValue;
    }

    /**
     * Creates immutable image from existing image source.
     *
     * @param srcImage The source of image.
     * @return Image created.
     * @throws NullPointerException if source image is null.
     */
    public static Image createImage(Image srcImage)
    {
        if(srcImage == null)
        {
            throw new NullPointerException(
                MsgRepository.IMAGE_EXCEPTION_IS_NULL);
        }

        if(!srcImage.isMutable())
        {
            return srcImage;
        }

        final Image finalImage = srcImage;
        ESWTUIThreadRunner.safeSyncExec(new Runnable()
        {
            public void run()
            {
                org.eclipse.swt.graphics.Image eswtImg =
                    eswtCreateImage(finalImage.eswtImage);

                if(eswtImg != null)
                {
                    createImageReturnValue = new Image(eswtImg, false);
                }
                else
                {
                    createImageReturnValue = null;
                    throw new OutOfMemoryError();
                }
            }
        });
        return createImageReturnValue;
    }

    /**
     * Creates an immutable image from named resource.
     *
     * @param name The name of the resource.
     * @return Image created.
     * @throws NullPointerException if name is null.
     * @throws IOException If resource doesn't exist, data cannot be loaded or
     *             data is invalid.
     */
    public static Image createImage(String name) throws IOException
    {
        if(name == null)
        {
            throw new NullPointerException(
                MsgRepository.IMAGE_EXCEPTION_FILE_NAME_IS_NULL);
        }

        String fileName = name;
        if(!fileName.startsWith("/"))
        {
            fileName = "/" + fileName;
        }

        InputStream stream = Image.class.getResourceAsStream(fileName);
        if(stream == null)
        {
            throw new IOException(MsgRepository.IMAGE_EXCEPTION_IO_ERROR);
        }
        Image image = createImage(stream);
        try
        {
            stream.close();
        }
        catch(IOException e)
        {
            // ignore
        }
        return image;
    }

    /**
     * Creates immutable image from stream. Thread's execution is blocked until
     * this method is finished.
     *
     * @param stream The stream.
     * @return Image created.
     * @throws NullPointerException if stream is null.
     * @throws java.io.IOException If I/O error occurs or data from stream is
     *             invalid.
     */
    public static Image createImage(InputStream stream) throws java.io.IOException
    {
        if(stream == null)
        {
            throw new NullPointerException(
                MsgRepository.IMAGE_EXCEPTION_FILE_STREAM_IS_NULL);
        }
        final InputStream finalStream = stream;
        ESWTUIThreadRunner.safeSyncExec(new Runnable()
        {
            public void run()
            {
                org.eclipse.swt.graphics.Image eswtImg = eswtCreateImage(finalStream);

                if(eswtImg != null)
                {
                    createImageReturnValue = new Image(eswtImg, false);
                }
                else
                {
                    createImageReturnValue = null;
                }
            }
        });
        if(createImageReturnValue == null)
        {
            throw new IOException(MsgRepository.IMAGE_EXCEPTION_IO_ERROR);
        }
        return createImageReturnValue;
    }

    /**
     * Creates new immutable image from image data provided.
     *
     * @param imgData Image data.
     * @param imgOffset Start offset in image data.
     * @param imgLength Length of data used.
     * @return Image created.
     * @throws ArrayIndexOutOfBoundsException if imgOffset and imgLength
     *             specifies invalid range.
     * @throws NullPointerException if imgData is null.
     * @throws IllegalArgumentException if imgData content is invalid.
     */
    public static Image createImage(byte[] imgData, int imgOffset, int imgLength)
    {
        if(imgData == null)
        {
            throw new NullPointerException(
                MsgRepository.IMAGE_EXCEPTION_DATA_IS_NULL);
        }
        if(imgOffset + imgLength > imgData.length)
        {
            throw new ArrayIndexOutOfBoundsException(
                MsgRepository.IMAGE_EXCEPTION_INVALID_BOUNDS);
        }
        if(imgOffset < 0 || imgLength < 0)
        {
            throw new ArrayIndexOutOfBoundsException(
                MsgRepository.IMAGE_EXCEPTION_INVALID_BOUNDS);
        }

        InputStream inputStream = new ByteArrayInputStream(imgData, imgOffset, imgLength);
        try
        {
            createImageReturnValue = createImage(inputStream);
        }
        catch(IOException e)
        {
            throw new IllegalArgumentException(
                MsgRepository.IMAGE_EXCEPTION_INVALID_DATA);
        }
        finally
        {
            try
            {
                inputStream.close();
            }
            catch(IOException e)
            {
                // ignore
            }
        }
        return createImageReturnValue;
    }

    /**
     * Creates an immutable image from specified region of existing source image
     * with specified transform.
     *
     * @param srcImage Source image.
     * @param x Region's x.
     * @param y Region's y.
     * @param w Regions width.
     * @param h Regions height.
     * @param transform Transform, one of the transforms specified in
     *            Sprite-class.
     * @return Image created.
     * @throws NullPointerException if img is null.
     * @throws IllegalArgumentException if Region specified exceeds the source
     *             image or if w or h is zero or less or if transform is not one
     *             defined in Sprite-class.
     */
    public static Image createImage(Image aImage,
                                    int aX,
                                    int aY,
                                    int aWidth,
                                    int aHeight,
                                    int aTransform)
    {
        if(aImage == null)
        {
            throw new NullPointerException(
                MsgRepository.IMAGE_EXCEPTION_IS_NULL);
        }
        if(aWidth <= 0 || aHeight <= 0)
        {
            throw new IllegalArgumentException(
                MsgRepository.IMAGE_EXCEPTION_INVALID_DIMENSIONS);
        }
        if(!validateTransform(aTransform))
        {
            throw new IllegalArgumentException(
                MsgRepository.IMAGE_EXCEPTION_INVALID_TRANSFORM);
        }

        if(!validateRegion(aImage.getWidth(), aImage.getHeight(), aX, aY, aWidth, aHeight))
        {
            throw new IllegalArgumentException(
                MsgRepository.IMAGE_EXCEPTION_INVALID_REGION);
        }

        final Image fImage = aImage;
        final int fx = aX;
        final int fy = aY;
        final int fw = aWidth;
        final int fh = aHeight;
        final int fTransform = aTransform;
        ESWTUIThreadRunner.safeSyncExec(new Runnable()
        {
            public void run()
            {
                org.eclipse.swt.internal.qt.graphics.Image srcCgImage =
                    Internal_GfxPackageSupport.getImage(fImage.getESWTImage());

                org.eclipse.swt.internal.qt.graphics.Image cgImage =
                    new org.eclipse.swt.internal.qt.graphics.Image(
                    srcCgImage, fx, fy, fw, fh);

                cgImage.transform(getCgTransformValue(fTransform));

                org.eclipse.swt.graphics.Image eswtImg =
                    Internal_GfxPackageSupport.new_Image(
                        ESWTUIThreadRunner.getInstance().getDisplay(),
                        cgImage);

                if(eswtImg != null)
                {
                    createImageReturnValue = new Image(eswtImg, false);
                }
                else
                {
                    createImageReturnValue = null;
                    throw new OutOfMemoryError();
                }
            }
        });
        return createImageReturnValue;
    }

    /**
     * Validates that a specified region is fully located within the image
     * bounds. Method is package-private, used by Graphics.drawRegion as well.
     */
    static boolean validateRegion(int aSrcWidth,
                                  int aSrcHeight,
                                  int aX,
                                  int aY,
                                  int aWidth,
                                  int aHeight)
    {
        boolean result = true;

        final int width  = aSrcWidth;
        final int height = aSrcHeight;
        
        final int sx1  = aX;            // left column
        final int sx2  = aX + aWidth;   // right column

        final int sy1  = aY;            // top row 
        final int sy2  = aY + aHeight;  // bottom row (exclusive)

        //
        // Check source x range lies within source image
        //
        final boolean xRangeError = (sx1 < 0) || (sx1 >= width) || (sx2 < 0) || 
            (sx2 > width);
        final boolean yRangeError = (sy1 < 0) || (sy1 >= height) || (sy2 < 0) || 
            (sy2 > height);

        if (xRangeError || yRangeError)
        {
            result = false;
        }
        return result;
    }
        
    /**
     * Validates if transform has a valid value. Method is package-private, used
     * by Graphics.drawRegion as well.
     */
    static boolean validateTransform(int transform)
    {
        return transform == Sprite.TRANS_NONE
               || transform == Sprite.TRANS_MIRROR
               || transform == Sprite.TRANS_MIRROR_ROT90
               || transform == Sprite.TRANS_MIRROR_ROT180
               || transform == Sprite.TRANS_MIRROR_ROT270
               || transform == Sprite.TRANS_ROT90
               || transform == Sprite.TRANS_ROT180
               || transform == Sprite.TRANS_ROT270;
    }

    /**
     *  Maps LCDUI transform constants to Common Graphics. Method is package-private, used
     *  by Graphics.drawRegion as well.
     */
    static int getCgTransformValue(int transform)
    {
        int retVal = org.eclipse.swt.internal.qt.graphics.Image.TRANS_NONE;
        switch(transform)
        {
        case Sprite.TRANS_NONE:
            retVal = org.eclipse.swt.internal.qt.graphics.Image.TRANS_NONE;
            break;
        case Sprite.TRANS_ROT90:
            retVal = org.eclipse.swt.internal.qt.graphics.Image.TRANS_ROT90;
            break;
        case Sprite.TRANS_ROT180:
            retVal =
                org.eclipse.swt.internal.qt.graphics.Image.TRANS_ROT180;
            break;
        case Sprite.TRANS_ROT270:
            retVal =
                org.eclipse.swt.internal.qt.graphics.Image.TRANS_ROT270;
            break;
        case Sprite.TRANS_MIRROR:
            retVal =
                org.eclipse.swt.internal.qt.graphics.Image.TRANS_MIRROR;
            break;
        case Sprite.TRANS_MIRROR_ROT90:
            retVal = org.eclipse.swt.internal.qt.graphics.
                     Image.TRANS_MIRROR_ROT90;
            break;
        case Sprite.TRANS_MIRROR_ROT180:
            retVal = org.eclipse.swt.internal.qt.graphics.
                     Image.TRANS_MIRROR_ROT180;
            break;
        case Sprite.TRANS_MIRROR_ROT270:
            retVal = org.eclipse.swt.internal.qt.graphics.
                     Image.TRANS_MIRROR_ROT270;
            break;
        default:
            break;
        }
        return retVal;
    }

    /**
     * Creates eSWT image as a copy of existing image.
     *
     * @param image - image to be copied.
     * @return new eSWT image.
     */
    private static org.eclipse.swt.graphics.Image eswtCreateImage(
        org.eclipse.swt.graphics.Image srcImage)
    {
        org.eclipse.swt.graphics.Image ret = null;
        try
        {
            org.eclipse.swt.internal.qt.graphics.Image cgImage =
                Internal_GfxPackageSupport.getImage(srcImage);

            org.eclipse.swt.internal.qt.graphics.Image cgImage2 =
                new org.eclipse.swt.internal.qt.graphics.Image(cgImage);

            ret = Internal_GfxPackageSupport.new_Image(
                      ESWTUIThreadRunner.getInstance().getDisplay(), cgImage2);
        }
        catch(IllegalArgumentException ex)
        {
            // Device is null or there's no current device.
            ret = null;
        }
        catch(org.eclipse.swt.SWTException ex)
        {
            ret = null;
        }
        catch(org.eclipse.swt.SWTError err)
        {
            ret = null;
        }
        return ret;
    }

    /**
     * Creates eSWT image of the specified size from the given image.
     */
    static org.eclipse.swt.graphics.Image eswtCreateImage(
        org.eclipse.swt.graphics.Image image, int newWidth, int newHeight)
    {
        org.eclipse.swt.graphics.Image ret = null;
        try
        {
            ret = new org.eclipse.swt.graphics.Image(
                ESWTUIThreadRunner.getInstance().getDisplay(),
                image.getImageData().scaledTo(newWidth, newHeight));
        }
        catch(IllegalArgumentException ex)
        {
            // Device is null or there's no current device.
            ret = null;
        }
        catch(org.eclipse.swt.SWTException ex)
        {
            ret = null;
        }
        catch(org.eclipse.swt.SWTError err)
        {
            ret = null;
        }
        return ret;
    }

    /**
     * Creates eSWT image of the specified size.
     */
    static org.eclipse.swt.graphics.Image eswtCreateImage(int width, int height)
    {
        org.eclipse.swt.graphics.Image ret = null;
        try
        {
            ret = new org.eclipse.swt.graphics.Image(
                ESWTUIThreadRunner.getInstance().getDisplay(), width, height);
        }
        catch(IllegalArgumentException ex)
        {
            // Device is null or there's no current device.
            ret = null;
        }
        catch(org.eclipse.swt.SWTException ex)
        {
            ret = null;
        }
        catch(org.eclipse.swt.SWTError err)
        {
            ret = null;
        }
        return ret;
    }

    /**
     * Creates and eSWT image out of the InputStream.
     */
    static org.eclipse.swt.graphics.Image eswtCreateImage(InputStream inputStream)
    {
        org.eclipse.swt.graphics.Image ret = null;
        try
        {
            ret = new org.eclipse.swt.graphics.Image(
                ESWTUIThreadRunner.getInstance().getDisplay(), inputStream);
        }
        catch(IllegalArgumentException ex)
        {
            // Device is null or there's no current device.
            ret = null;
        }
        catch(org.eclipse.swt.SWTException ex)
        {
            ret = null;
        }
        catch(org.eclipse.swt.SWTError err)
        {
            ret = null;
        }
        return ret;
    }

    /**
     * Synchronizes any pending draw commands to this image. The buffer sync 
     * must be executed in UI thread and if this method is not requested to switch to
     * UI thread, the caller must take care of serializing the call over the graphicsBuffer
     * of this instance.
     * 
     * @param switchToUIThread If true the sync is run in UI thread, oherwise
     *        caller must take care of switching to UI thread
     */
    void sync(boolean switchToUIThread)
    {
    	if(switchToUIThread) 
		{
    	    synchronized(graphicsBuffer)
            {
                ESWTUIThreadRunner.safeSyncExec(new Runnable()
                {
                    public void run()
                    {
                        graphicsBuffer.sync();
                    }
                });
            }
         } 
    	else 
    	{
    		graphicsBuffer.sync();
    	}
    }

    /**
     * Creates new image from specified RGB array data.
     *
     * @param rgbData Pixel data.
     * @param width Width of the image to be created.
     * @param height Height of the image to be created.
     * @param processAlpha true if there's alpha channel in image data.
     *            Otherwise method assumes that all pixels are fully opaque.
     * @return Image created.
     * @throws NullPointerException if rgb is null.
     * @throws IllegalArgumentException if w or h is zero or less.
     * @throws ArrayIndexOutOfBoundsException if the w and h parameters doesn't
     *             match the length of the data array provided.
     */
    public static Image createRGBImage(int[] rgbData,
                                       int width,
                                       int height,
                                       boolean processAlpha)
    {

        if(rgbData == null)
        {
            throw new NullPointerException(
                MsgRepository.IMAGE_EXCEPTION_DATA_IS_NULL);
        }
        if(width <= 0 || height <= 0)
        {
            throw new IllegalArgumentException(
                MsgRepository.IMAGE_EXCEPTION_INVALID_DIMENSIONS);
        }
        if(width * height > rgbData.length)
        {
            throw new ArrayIndexOutOfBoundsException(
                MsgRepository.IMAGE_EXCEPTION_INVALID_BOUNDS);
        }

        final int[] rgb = rgbData;
        final int w = width;
        final int h = height;
        final boolean alpha = processAlpha;
        ESWTUIThreadRunner.safeSyncExec(new Runnable()
        {
            public void run()
            {
                org.eclipse.swt.internal.qt.graphics.Image cgImage =
                    new org.eclipse.swt.internal.qt.graphics.Image(rgb, w, h, alpha);

                org.eclipse.swt.graphics.Image eswtImg =
                    Internal_GfxPackageSupport.new_Image(
                        ESWTUIThreadRunner.getInstance().getDisplay(), cgImage);

                if(eswtImg != null)
                {
                    createImageReturnValue = new Image(eswtImg, false);
                }
                else
                {
                    createImageReturnValue = null;
                    throw new OutOfMemoryError();
                }
            }
        });
        return createImageReturnValue;
    }

    /**
     * Returns ARGB data of image region to an int[] array.
     *
     * @param rgbData - array to be filled with ARGB data.
     * @param offset - first index in the array to store the data.
     * @param length - relative distance in the array between corresponding
     *            pixels of consecutive rows.
     * @param xPos - x-coordinate of the top-left corner of the region.
     * @param yPos - y-coordinate of the top-left corner of the region.
     * @param width - width of the region.
     * @param height - height of the region.
     */
    public void getRGB(int[] rgbData,
                       int offset,
                       int length,
                       int xPos,
                       int yPos,
                       int width,
                       int height)
    {

        if(rgbData == null)
        {
            throw new NullPointerException(
                MsgRepository.IMAGE_EXCEPTION_DATA_IS_NULL);
        }
        if(!validateRegion(this.getWidth(), this.getHeight(),
                           xPos, yPos, width, height))
        {
            throw new IllegalArgumentException(
                MsgRepository.IMAGE_EXCEPTION_INVALID_BOUNDS);
        }
        if(Math.abs(length) < width)
        {
            throw new IllegalArgumentException(
                MsgRepository.IMAGE_EXCEPTION_INVALID_SCANLENGTH);
        }

        synchronized(graphicsBuffer)
        {
            final int[] localRgbData = rgbData;
            final int localOffset = offset;
            final int localLength = length;
            final int localX = xPos;
            final int localY = yPos;
            final int localW = width;
            final int localH = height;
            ESWTUIThreadRunner.safeSyncExec(new Runnable()
            {
                public void run()
                {
                	graphicsBuffer.sync();
                    org.eclipse.swt.internal.qt.graphics.Image cgImage = Internal_GfxPackageSupport.getImage(eswtImage);
                    cgImage.getRGB(localRgbData, localOffset, localLength,
                                   localX, localY, localW, localH);
                }
            });
        }
    }

    /**
     * Creates a new Graphics-objects that is able to draw to this image.
     *
     * @return New Graphics-object.
     * @throws IllegalStateException if image is immutable.
     */
    public Graphics getGraphics()
    {
        if(mutable)
        {	
        	tempGraphics = null;
        	ESWTUIThreadRunner.safeSyncExec(new Runnable()
            {
                public void run()
                {
        	        tempGraphics =  graphicsBuffer.getGraphics();
                }
            });
            return tempGraphics;
        }
        throw new IllegalStateException(MsgRepository.IMAGE_EXCEPTION_IMMUTABLE);
    }

    /**
     * Gets width of the image.
     *
     * @return Image width.
     */
    public int getWidth()
    {
        return eswtImageWidth;
    }

    /**
     * Gets height of the image.
     *
     * @return Image height.
     */
    public int getHeight()
    {
        return eswtImageHeight;
    }

    /**
     * Check is it possible to modify this image.
     *
     * @return true, if image is mutable.
     */
    public boolean isMutable()
    {
        return mutable;
    }

}
