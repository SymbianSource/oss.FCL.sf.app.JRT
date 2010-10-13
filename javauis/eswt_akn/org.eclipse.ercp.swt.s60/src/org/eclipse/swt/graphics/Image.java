/*******************************************************************************
 * Copyright (c) 2000, 2004 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *     Lynne Kues (IBM Corp) - modified to reflect eSWT API subset
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/
package org.eclipse.swt.graphics;


import java.io.*;

import org.eclipse.swt.*;
import org.eclipse.swt.internal.symbian.*;
import org.eclipse.swt.internal.FileCompatibility;
import org.eclipse.swt.widgets.Display;


/**
 * Instances of this class are graphics which have been prepared
 * for display on a specific device. That is, they are ready
 * to paint using methods such as <code>GC.drawImage()</code>
 * and display on widgets with, for example, <code>Button.setImage()</code>.
 * <p>
 * If loaded from a file format that supports it, an
 * <code>Image</code> may have transparency, meaning that certain
 * pixels are specified as being transparent when drawn. Examples
 * of file formats that support transparency are GIF and PNG.
 * </p><p>
 * There are two primary ways to use <code>Images</code>.
 * The first is to load a graphic file from disk and create an
 * <code>Image</code> from it. This is done using an <code>Image</code>
 * constructor, for example:
 * <pre>
 *    Image i = new Image(device, "C:\\graphic.png");
 * </pre>
 * A graphic file may contain a color table specifying which
 * colors the image was intended to possess. In the above example,
 * these colors will be mapped to the closest available color in
 * SWT. It is possible to get more control over the mapping of
 * colors as the image is being created, using code of the form:
 * <pre>
 *    ImageData data = new ImageData("C:\\graphic.png");
 *    RGB[] rgbs = data.getRGBs();
 *    // At this point, rgbs contains specifications of all
 *    // the colors contained within this image. You may
 *    // allocate as many of these colors as you wish by
 *    // using the Color constructor Color(RGB), then
 *    // create the image:
 *    Image i = new Image(device, data);
 * </pre>
 * <p>
 * Application code must explicitely invoke the <code>Image.dispose()</code>
 * method to release the operating system resources managed by each instance
 * when those instances are no longer required.
 * </p>
 *
 * @see Color
 * @see ImageData
 */

public final class Image implements Drawable
{

    /**
     * specifies whether the receiver is a bitmap or an icon
     * (one of <code>SWT.BITMAP</code>, <code>SWT.ICON</code>)
     */
    public int type;

    /**
     * the handle to the OS image resource
     * (Warning: This field is platform dependent)
     */
    public int handle;

    /**
     * the device where this image was created
     */
    Device device;

    /**
     * specifies the transparent pixel
     * (Warning: This field is platform dependent)
     */
    Color transparentPixel;

    /**
     * Whether this image is currently selected in a GC
     * (Warning: This field is platform dependent)
     */
    boolean owningGc = false;

    /**
     * the global alpha value to be used for every pixel
     * (Warning: This field is platform dependent)
     */
    int alpha = -1;

    /**
     * specifies the default scanline padding
     * (Warning: This field is platform dependent)
     */
    static final int DEFAULT_SCANLINE_PAD = 4;

    /**
     * Prevents uninitialized instances from being created outside the package.
     */
    Image()
    {
    }

    /**
     * Constructs an empty instance of this class with the
     * specified width and height. The result may be drawn upon
     * by creating a GC and using any of its drawing operations,
     * as shown in the following example:
     * <pre>
     *    Image i = new Image(device, width, height);
     *    GC gc = new GC(i);
     *    gc.drawRectangle(0, 0, 50, 50);
     *    gc.dispose();
     * </pre>
     * <p>
     * Note: Some platforms may have a limitation on the size
     * of image that can be created (size depends on width, height,
     * and depth). For example, Windows 95, 98, and ME do not allow
     * images larger than 16M.
     * </p>
     *
     * @param device the device on which to create the image
     * @param width the width of the new image
     * @param height the height of the new image
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if device is null and there is no current device</li>
     *    <li>ERROR_INVALID_ARGUMENT - if either the width or height is negative or zero</li>
     * </ul>
     * @exception SWTError <ul>
     *    <li>ERROR_NO_HANDLES if a handle could not be obtained for image creation</li>
     * </ul>
     */
    public Image(Device device, int width, int height)
    {
        if (device == null) device = Device.getDevice();
        if (device == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        init(device, width, height);
    }

    /**
     * Constructs an empty instance of this class with the
     * width and height of the specified rectangle. The result
     * may be drawn upon by creating a GC and using any of its
     * drawing operations, as shown in the following example:
     * <pre>
     *    Image i = new Image(device, boundsRectangle);
     *    GC gc = new GC(i);
     *    gc.drawRectangle(0, 0, 50, 50);
     *    gc.dispose();
     * </pre>
     * <p>
     * Note: Some platforms may have a limitation on the size
     * of image that can be created (size depends on width, height,
     * and depth). For example, Windows 95, 98, and ME do not allow
     * images larger than 16M.
     * </p>
     *
     * @param device the device on which to create the image
     * @param bounds a rectangle specifying the image's width and height (must not be null)
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if device is null and there is no current device</li>
     *    <li>ERROR_NULL_ARGUMENT - if the bounds rectangle is null</li>
     *    <li>ERROR_INVALID_ARGUMENT - if either the rectangle's width or height is negative</li>
     * </ul>
     * @exception SWTError <ul>
     *    <li>ERROR_NO_HANDLES if a handle could not be obtained for image creation</li>
     * </ul>
     */
    public Image(Device device, Rectangle bounds)
    {
        if (device == null) device = Device.getDevice();
        if (device == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        if (bounds == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        init(device, bounds.width, bounds.height);
    }

    /**
     * Constructs an instance of this class from the given
     * <code>ImageData</code>.
     *
     * @param device the device on which to create the image
     * @param data the image data to create the image from (must not be null)
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if device is null and there is no current device</li>
     *    <li>ERROR_NULL_ARGUMENT - if the image data is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_UNSUPPORTED_DEPTH - if the depth of the ImageData is not supported</li>
     * </ul>
     * @exception SWTError <ul>
     *    <li>ERROR_NO_HANDLES if a handle could not be obtained for image creation</li>
     * </ul>
     */
    public Image(Device device, ImageData data)
    {
        if (device == null) device = Device.getDevice();
        if (device == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        init(device, data);
    }

    /**
     * Constructs an instance of this class by loading its representation
     * from the specified input stream. Throws an error if an error
     * occurs while loading the image, or if the result is an image
     * of an unsupported type.  Application code is still responsible
     * for closing the input stream.
     * <p>
     * This constructor may be used to load a resource as follows:
     * </p>
     * <pre>
     *     new Image(device, clazz.getResourceAsStream("file.png"));
     * </pre>
     *
     * @param device the device on which to create the image
     * @param stream the input stream to load the image from
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if device is null and there is no current device</li>
     *    <li>ERROR_NULL_ARGUMENT - if the stream is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_INVALID_IMAGE - if the image file contains invalid data </li>
     *    <li>ERROR_IO - if an IO error occurs while reading data</li>
     *    <li>ERROR_UNSUPPORTED_DEPTH - if the InputStream describes an image with an unsupported depth</li>
     *    <li>ERROR_UNSUPPORTED_FORMAT - if the image file contains an unrecognized format</li>
     *  * </ul>
     * </ul>
     * @exception SWTError <ul>
     *    <li>ERROR_NO_HANDLES if a handle could not be obtained for image creation</li>
     * </ul>
     */
    public Image(Device device, InputStream stream)
    {
        if (device == null) device = Device.getDevice();
        if (device == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        
        try
        {
            init(device, new ImageData(stream));
        }
        catch (SWTException ex)
        {
            if (ex.code == SWT.ERROR_UNSUPPORTED_FORMAT)
            {
                Point size = (Point)(Display.getCurrent().getData("org.eclipse.swt.internal.image.loadSize"));
                if (size == null)
                {
                    size = new Point(88, 88);
                }
                try
                {
                    stream.reset();
                }
                catch (IOException e)
                {
                }
                init(device, OS.Image_NewFromSvgBuf(device.handle, Image.readImageStream(stream), size.x, size.y));
            }
            else
            {
                throw ex;
            }
        }
    }

    /**
     * Constructs an instance of this class by loading its representation
     * from the file with the specified name. Throws an error if an error
     * occurs while loading the image, or if the result is an image
     * of an unsupported type.
     * <p>
     * This constructor is provided for convenience when loading
     * a single image only. If the specified file contains
     * multiple images, only the first one will be used.
     *
     * @param device the device on which to create the image
     * @param filename the name of the file to load the image from
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if device is null and there is no current device</li>
     *    <li>ERROR_NULL_ARGUMENT - if the file name is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_INVALID_IMAGE - if the image file contains invalid data </li>
     *    <li>ERROR_IO - if an IO error occurs while reading data</li>
     *    <li>ERROR_UNSUPPORTED_DEPTH - if the image file has an unsupported depth</li>
     *    <li>ERROR_UNSUPPORTED_FORMAT - if the image file contains an unrecognized format</li>
     * </ul>
     * @exception SWTError <ul>
     *    <li>ERROR_NO_HANDLES if a handle could not be obtained for image creation</li>
     * </ul>
     */
    public Image(Device device, String filename)
    {
        if (device == null) device = Device.getDevice();
        if (device == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        try
        {
            init(device, new ImageData(filename));
        }
        catch (SWTException ex)
        {
            if (ex.code == SWT.ERROR_UNSUPPORTED_FORMAT)
            {
                Point size = (Point)(Display.getCurrent().getData("org.eclipse.swt.internal.image.loadSize"));
                if (size == null)
                {
                    size = new Point(88, 88);
                }
                init(device, OS.Image_NewFromSvgFile(device.handle, filename, size.x, size.y));                
            }
            else
            {
                throw ex;
            }
        }
    }
    
    /**
     * Disposes of the operating system resources associated with
     * the image. Applications must dispose of all images which
     * they allocate.
     */
    public void dispose()
    {
        if (handle == 0) return;
        if (device.isDisposed()) return;
        OS.Image_Dispose(device.handle, handle);
        handle = 0;
        if (transparentPixel != null) transparentPixel.dispose();
        if (device.tracking) device.dispose_Object(this);
        device = null;
    }

    /**
     * Compares the argument to the receiver, and returns true
     * if they represent the <em>same</em> object using a class
     * specific comparison.
     *
     * @param object the object to compare with this object
     * @return <code>true</code> if the object is the same as this object and <code>false</code> otherwise
     *
     * @see #hashCode
     */
    public boolean equals(Object object)
    {
        if (object == this) return true;
        if (!(object instanceof Image)) return false;
        Image image = (Image) object;
        return device == image.device && handle == image.handle;
    }

    /**
     * Returns the bounds of the receiver. The rectangle will always
     * have x and y values of 0, and the width and height of the
     * image.
     *
     * @return a rectangle specifying the image's bounds
     *
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_INVALID_IMAGE - if the image is not a bitmap or an icon</li>
     * </ul>
     */
    public Rectangle getBounds()
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        switch (type)
        {
        case SWT.BITMAP:
        case SWT.ICON:
            return OS.Image_GetBounds(handle);
        default:
            SWT.error(SWT.ERROR_INVALID_IMAGE);
            return null;
        }
    }

    /**
     * Returns an <code>ImageData</code> based on the receiver
     * Modifications made to this <code>ImageData</code> will not
     * affect the Image.
     *
     * @return an <code>ImageData</code> containing the image's data and attributes
     *
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_INVALID_IMAGE - if the image is not a bitmap or an icon</li>
     * </ul>
     *
     * @see ImageData
     */
    public ImageData getImageData()
    {
        if (isDisposed()) SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        switch (type)
        {
        case SWT.BITMAP:
        case SWT.ICON:
            return OS.Image_GetData(handle);
        default:
            SWT.error(SWT.ERROR_INVALID_IMAGE);
            return null;
        }
    }

    /**
     * Returns an integer hash code for the receiver. Any two
     * objects which return <code>true</code> when passed to
     * <code>equals</code> must return the same value for this
     * method.
     *
     * @return the receiver's hash
     *
     * @see #equals
     */
    public int hashCode()
    {
        return handle;
    }
    
    void init(Device device, int handle)
    {
        if (device == null) device = Device.getDevice();
        if (device == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        if (handle == 0) SWT.error(SWT.ERROR_NO_HANDLES, null, device.getLastError());
        
        this.device = device;
        this.handle = handle;
        this.type = SWT.BITMAP;
        
        try
        {
            if (device.tracking) device.new_Object(this);
        }
        catch (Error e)
        {
            OS.Image_Dispose(device.handle, handle);
            throw e;
        }
    }

    void init(Device device, int width, int height)
    {
        if (width <= 0 || height <= 0)
        {
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        }
        this.device = device;
        type = SWT.BITMAP;
        handle = OS.Image_New(device.handle, width, height);
        if (handle == 0)
        {
            SWT.error(SWT.ERROR_NO_HANDLES, null, device.getLastError());
        }
        try
        {
            if (device.tracking) device.new_Object(this);
        }
        catch (Error e)
        {
            OS.Image_Dispose(device.handle, handle);
            throw e;
        }
    }

    static void init(Device device, Image image, ImageData i)
    {
        if (image != null) image.device = device;
        image.handle = OS.Image_NewFromData(device.handle, i);
        image.alpha = i.alpha;

        try
        {
            if (i.transparentPixel != -1)
            {
                image.transparentPixel = new Color(device, i.palette.getRGB(i.transparentPixel));
            }
            if (device.tracking) device.new_Object(image);
        }
        catch (Error e)
        {
            OS.Image_Dispose(device.handle, image.handle);
            throw e;
        }
    }

    static void init(Device device, Image image, ImageData source, ImageData mask)
    {
        /* Create a temporary image and locate the black pixel */
        ImageData imageData;
        int blackIndex = 0;
        if (source.palette.isDirect)
        {
            imageData = new ImageData(source.width, source.height, source.depth, source.palette);
        }
        else
        {
            RGB black = new RGB(0, 0, 0);
            RGB[] rgbs = source.getRGBs();
            if (source.transparentPixel != -1)
            {
                /*
                 * The source had transparency, so we can use the transparent pixel
                 * for black.
                 */
                RGB[] newRGBs = new RGB[rgbs.length];
                System.arraycopy(rgbs, 0, newRGBs, 0, rgbs.length);
                if (source.transparentPixel >= newRGBs.length)
                {
                    /* Grow the palette with black */
                    rgbs = new RGB[source.transparentPixel + 1];
                    System.arraycopy(newRGBs, 0, rgbs, 0, newRGBs.length);
                    for (int i = newRGBs.length; i <= source.transparentPixel; i++)
                    {
                        rgbs[i] = new RGB(0, 0, 0);
                    }
                }
                else
                {
                    newRGBs[source.transparentPixel] = black;
                    rgbs = newRGBs;
                }
                blackIndex = source.transparentPixel;
                imageData = new ImageData(source.width, source.height, source.depth, new PaletteData(rgbs));
            }
            else
            {
                while (blackIndex < rgbs.length)
                {
                    if (rgbs[blackIndex].equals(black)) break;
                    blackIndex++;
                }
                if (blackIndex == rgbs.length)
                {
                    /*
                     * We didn't find black in the palette, and there is no transparent
                     * pixel we can use.
                     */
                    if ((1 << source.depth) > rgbs.length)
                    {
                        /* We can grow the palette and add black */
                        RGB[] newRGBs = new RGB[rgbs.length + 1];
                        System.arraycopy(rgbs, 0, newRGBs, 0, rgbs.length);
                        newRGBs[rgbs.length] = black;
                        rgbs = newRGBs;
                    }
                    else
                    {
                        /* No room to grow the palette */
                        blackIndex = -1;
                    }
                }
                imageData = new ImageData(source.width, source.height, source.depth, new PaletteData(rgbs));
            }
        }
        if (blackIndex == -1)
        {
            /* There was no black in the palette, so just copy the data over */
            System.arraycopy(source.data, 0, imageData.data, 0, imageData.data.length);
        }
        else
        {
            /* Modify the source image to contain black wherever the mask is 0 */
            int[] imagePixels = new int[imageData.width];
            int[] maskPixels = new int[mask.width];
            for (int y = 0; y < imageData.height; y++)
            {
                source.getPixels(0, y, imageData.width, imagePixels, 0);
                mask.getPixels(0, y, mask.width, maskPixels, 0);
                for (int i = 0; i < imagePixels.length; i++)
                {
                    if (maskPixels[i] == 0) imagePixels[i] = blackIndex;
                }
                imageData.setPixels(0, y, source.width, imagePixels, 0);
            }
        }
        imageData.maskPad = mask.scanlinePad;
        imageData.maskData = mask.data;
        init(device, image, imageData);
    }

    void init(Device device, ImageData i)
    {
        if (i == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        init(device, this, i);
    }

    /**
     * Invokes platform specific functionality to allocate a new GC handle.
     * <p>
     * <b>IMPORTANT:</b> This method is <em>not</em> part of the public
     * API for <code>Image</code>. It is marked public only so that it
     * can be shared within the packages provided by SWT. It is not
     * available on all platforms, and should never be called from
     * application code.
     * </p>
     *
     * @param data the platform specific GC data
     * @return the platform specific GC handle
     */
    public int internal_new_GC(GCData data)
    {
        synchronized (this)
        {
            if (owningGc)
            {
                SWT.error(SWT.ERROR_INVALID_ARGUMENT);
            }
            owningGc = true;
        }

        if ((type != SWT.BITMAP) && (type != SWT.ICON))
        {
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        }

        return OS.Drawable_NewGc(handle);
    }

    /**
     * Invokes platform specific functionality to dispose a GC handle.
     * <p>
     * <b>IMPORTANT:</b> This method is <em>not</em> part of the public
     * API for <code>Image</code>. It is marked public only so that it
     * can be shared within the packages provided by SWT. It is not
     * available on all platforms, and should never be called from
     * application code.
     * </p>
     *
     * @param hDC the platform specific GC handle
     * @param data the platform specific GC data
     */
    public void internal_dispose_GC(int gcHandle, GCData data)
    {
        OS.GC_Dispose(gcHandle);

        synchronized (this)
        {
            owningGc = false;
        }
    }

    /**
     * Returns <code>true</code> if the receiver is visible, and
     * <code>false</code> otherwise.
     * <p>
     * <b>IMPORTANT:</b> This method is <em>not</em> part of the public
     * API for <code>Drawable</code>. It is marked public only so that it
     * can be shared within the packages provided by SWT. It is not
     * available on all platforms, and should never be called from
     * application code.
     * </p>
     *
     * @return the receiver's visibility state
     */
    public boolean internal_isVisible()
    {
        return true;
    }

    /**
     * Returns <code>true</code> if the receiver has been disposed,
     * and <code>false</code> otherwise.
     * <p>
     * <b>IMPORTANT:</b> This method is <em>not</em> part of the public
     * API for <code>Drawable</code>. It is marked public only so that it
     * can be shared within the packages provided by SWT. It is not
     * available on all platforms, and should never be called from
     * application code.
     * </p>
     *
     * @return <code>true</code> when the receiver is disposed and <code>false</code> otherwise
     */
    public boolean internal_isDisposed()
    {
        return isDisposed();
    }

    /**
     * Returns <code>true</code> if the image has been disposed,
     * and <code>false</code> otherwise.
     * <p>
     * This method gets the dispose state for the image.
     * When an image has been disposed, it is an error to
     * invoke any other method using the image.
     *
     * @return <code>true</code> when the image is disposed and <code>false</code> otherwise
     */
    public boolean isDisposed()
    {
        return handle == 0;
    }

    /**
     * Returns a string containing a concise, human-readable
     * description of the receiver.
     *
     * @return a string representation of the receiver
     */
    public String toString()
    {
        if (isDisposed()) return "Image {*DISPOSED*}";
        return "Image {" + handle + "}";
    }

    /**
     * The following functions are not part of the eSWT public api
     * @param fileName
     * @return
     */
    static int imageFormat(String fileName)
    {
        InputStream stream = null;
        try
        {
            stream = FileCompatibility.getInputStreamFromFile(fileName);
            byte[] buffer = new byte[32];
            stream.read(buffer, 0, buffer.length);
            stream.close();
            return Image.imageFormat(buffer);
        }
        catch (IOException e)
        {
            SWT.error(SWT.ERROR_IO, e);
        }
        finally
        {
            try
            {
                if (stream != null) stream.close();
            }
            catch (IOException e)
            {
                // Ignore error
            }
        }
        return SWT.IMAGE_UNDEFINED;
    }
    
    static int imageFormat(byte[] streamBytes)
    {
        if (isPNGFormat(streamBytes)) return SWT.IMAGE_PNG;
        if (isGIFFormat(streamBytes)) return SWT.IMAGE_GIF;
        if (isJPEGFormat(streamBytes)) return SWT.IMAGE_JPEG;
        return SWT.IMAGE_UNDEFINED;
    }
    
    static boolean isPNGFormat(byte[] streamBytes)
    {
        if (streamBytes.length < 8) return false;
        if ((streamBytes[0] & 0xFF) != 137) return false; //137
        if ((streamBytes[1] & 0xFF) != 80) return false; //P
        if ((streamBytes[2] & 0xFF) != 78) return false; //N
        if ((streamBytes[3] & 0xFF) != 71) return false; //G
        if ((streamBytes[4] & 0xFF) != 13) return false; //<RETURN>
        if ((streamBytes[5] & 0xFF) != 10) return false; //<LINEFEED>
        if ((streamBytes[6] & 0xFF) != 26) return false; //<CTRL/Z>
        if ((streamBytes[7] & 0xFF) != 10) return false; //<LINEFEED>
        return true;
    }
    
    static boolean isGIFFormat(byte[] streamBytes)
    {
        if (streamBytes.length < 3) return false;
        if (streamBytes[0] != 'G') return false;
        if (streamBytes[1] != 'I') return false;
        if (streamBytes[2] != 'F') return false;
        return true;
    }
    
    static boolean isJPEGFormat(byte[] streamBytes)
    {
        if (streamBytes.length < 2) return false;
        if ((streamBytes[0] & 0xFF) != 0xFF) return false;
        if ((streamBytes[1] & 0xFF) != 0xD8) return false;
        return true;
    }
    
    /**
     * Reads the specified stream and returns the contents as a byte
     * array.
     *
     * @param stream the raw image data stream
     * @return image data buffer
     */
    static byte[] readImageStream(InputStream stream)
    {
        byte[] buffer = new byte[512];
        ByteArrayOutputStream baoStream = new ByteArrayOutputStream();
        if (buffer == null || baoStream == null)
        {
            SWT.error(SWT.ERROR_IO);
        }
        try
        {
            boolean done = false;
            while (!done)
            {
                int length = stream.read(buffer);
                if (-1 == length)
                {
                    done = true;
                }
                else
                {
                    baoStream.write(buffer,0,length);
                }
            }
        }
        catch (IOException e)
        {
            SWT.error(SWT.ERROR_IO, e);
        }
        finally
        {
            try
            {
                baoStream.close();
            }
            catch (IOException e)
            {
                // ignore for the close
            }
        }

        return baoStream.toByteArray();
    }
    
    /**
     * Invokes platform specific functionality to allocate a new image
     * which has handle to a system image.
     * <p>
     * <b>IMPORTANT:</b> This method is <em>not</em> part of the public
     * API for <code>Font</code>. It is marked public only so that it
     * can be shared within the packages provided by SWT. It is not
     * available on all platforms, and should never be called from
     * application code.
     * </p>
     *
     * @param device the device on which to allocate the image
     * @param handle the native handle for the image
     * @return a new image object containing the specified device and
     *         system image handle
     */
    public static Image internal_new(Device device, int handle)
    {
        Image image = new Image();
        image.init(device, handle);
        return image;
    }
}
