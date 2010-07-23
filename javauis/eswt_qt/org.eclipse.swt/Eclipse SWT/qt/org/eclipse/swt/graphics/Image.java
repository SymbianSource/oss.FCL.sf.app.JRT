/*******************************************************************************
 * Copyright (c) 2000, 2007 IBM Corporation and others.
 * Portion Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *     Nokia Corporation - Qt implementation
 *******************************************************************************/
package org.eclipse.swt.graphics;


import org.eclipse.swt.SWT;
import org.eclipse.swt.internal.Compatibility;
import org.eclipse.swt.internal.qt.GCData;
import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.internal.qt.graphics.GraphicsContext;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Internal_PackageSupport;
import org.eclipse.swt.graphics.Point;

import java.io.IOException;
import java.io.InputStream;

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
 *    Image i = new Image(device, "C:\\graphic.bmp");
 * </pre>
 * A graphic file may contain a color table specifying which
 * colors the image was intended to possess. In the above example,
 * these colors will be mapped to the closest available color in
 * SWT. It is possible to get more control over the mapping of
 * colors as the image is being created, using code of the form:
 * <pre>
 *    ImageData data = new ImageData("C:\\graphic.bmp");
 *    RGB[] rgbs = data.getRGBs();
 *    // At this point, rgbs contains specifications of all
 *    // the colors contained within this image. You may
 *    // allocate as many of these colors as you wish by
 *    // using the Color constructor Color(RGB), then
 *    // create the image:
 *    Image i = new Image(device, data);
 * </pre>
 * <p>
 * Applications which require even greater control over the image
 * loading process should use the support provided in class
 * <code>ImageLoader</code>.
 * </p><p>
 * Application code must explicitly invoke the <code>Image.dispose()</code>
 * method to release the operating system resources managed by each instance
 * when those instances are no longer required.
 * </p>
 *
 * @see Color
 * @see ImageData
 * @see ImageLoader
 */
public final class Image implements Drawable {

    static int getNullIconHandle() {
        if( Device.nullIconHandle == 0 ){
            Device.nullIconHandle = OS.QIcon_new();
        }
        return Device.nullIconHandle;
    }

    /*
     * Creates new Image instance.
     * <p>
     * <b>IMPORTANT:</b> This method is <em>not</em> part of the public
     * API for <code>Image</code>. It is marked public only so that it
     * can be shared within the packages provided by SWT. It is not
     * available on all platforms, and should never be called from
     * application code.
     * </p>
     * @param device the device on which to create the image
     * @param cgImage the internal image instance
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if device is null and there is no current device</li>
     *    <li>ERROR_NULL_ARGUMENT - if the iternal image is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_INVALID_IMAGE - if the given internal image is not valid</li>
     * </ul>
     * @exception SWTError <ul>
     *    <li>ERROR_NO_HANDLES if a handle could not be obtained for image creation</li>
     * </ul>
     */
    static Image new_Image(
        Device device, org.eclipse.swt.internal.qt.graphics.Image cgImage) {
        Image image = new Image(device);
        image.init(cgImage);
        return image;
    }

    /**
     * specifies whether the receiver is a bitmap or an icon
     * (one of <code>SWT.BITMAP</code>, <code>SWT.ICON</code>)
     * <p>
     * <b>IMPORTANT:</b> This field is <em>not</em> part of the SWT
     * public API. It is marked public only so that it can be shared
     * within the packages provided by SWT. It is not available on all
     * platforms and should never be accessed from application code.
     * </p>
     */
    public int type = SWT.BITMAP;

    /*
     * The alpha data of the image.
     */
    byte[] alphaData;

    /*
     * The global alpha value to be used for every pixel.
     */
    int alpha = SWT.DEFAULT;

    /*
     * Specifies the default scanline padding.
     */
    static final int DEFAULT_SCANLINE_PAD = 4;

    private org.eclipse.swt.internal.qt.graphics.Image cgImage;

    /*
     * Handle to the QIcon for this Image
     */
    private int icon = SWT.NULL;

    /*
     * specifies the transparent pixel
     * (Warning: This field is platform dependent)
     */
    int transparentPixel = -1;

    /*
     * Device
     */
    Device device;

    /**
     * Prevents uninitialized instances from being created outside the package.
     * @param device Device
     */
    Image(Device device) {
        if (device == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        this.device = device;
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
    public Image(Device device, ImageData data) {
        this(device);
        init(data);
        track();
    }

    /**
     * Constructs an instance of this class by loading its representation
     * from the specified input stream. Throws an error if an error
     * occurs while loading the image, or if the result is an image
     * of an unsupported type.  Application code is still responsible
     * for closing the input stream.
     * <p>
     * This constructor is provided for convenience when loading a single
     * image only. If the stream contains multiple images, only the first
     * one will be loaded. To load multiple images, use
     * <code>ImageLoader.load()</code>.
     * </p><p>
     * This constructor may be used to load a resource as follows:
     * </p>
     * <pre>
     *     static Image loadImage (Display display, Class clazz, String string) {
     *          InputStream stream = clazz.getResourceAsStream (string);
     *          if (stream == null) return null;
     *          Image image = null;
     *          try {
     *               image = new Image (display, stream);
     *          } catch (SWTException ex) {
     *          } finally {
     *               try {
     *                    stream.close ();
     *               } catch (IOException ex) {}
     *          }
     *          return image;
     *     }
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
     *    <li>ERROR_IO - if an IO error occurs while reading from the stream</li>
     *    <li>ERROR_INVALID_IMAGE - if the image stream contains invalid data </li>
     *    <li>ERROR_UNSUPPORTED_DEPTH - if the image stream describes an image with an unsupported depth</li>
     *    <li>ERROR_UNSUPPORTED_FORMAT - if the image stream contains an unrecognized format</li>
     * </ul>
     * @exception SWTError <ul>
     *    <li>ERROR_NO_HANDLES if a handle could not be obtained for image creation</li>
     * </ul>
     */
    public Image(Device device, InputStream stream) {
        this(device);
        if (stream == null) {
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        }
        init();
        org.eclipse.swt.internal.qt.graphics.ImageLoader loader =
            new org.eclipse.swt.internal.qt.graphics.ImageLoader();
        setLoadSize(loader);
        try {
            this.cgImage = loader.loadImage(stream);
        }
        catch(IllegalArgumentException e) {
            SWT.error (SWT.ERROR_UNSUPPORTED_FORMAT);
        }
        catch(IllegalStateException e) {
            SWT.error (SWT.ERROR_INVALID_IMAGE);
        }
        catch(IOException e) {
            SWT.error (SWT.ERROR_IO);
        }
        finally {
            loader.dispose();
        }
        track();
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
    public Image(Device device, int width, int height) {
        this(device);
        init(width, height);
        track();
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
    public Image(Device device, Rectangle bounds) {
        this(device);
        if (bounds == null) {
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        }
        init(bounds.width, bounds.height);
        track();
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
     *    <li>ERROR_IO - if an IO error occurs while reading from the file</li>
     *    <li>ERROR_INVALID_IMAGE - if the image file contains invalid data </li>
     *    <li>ERROR_UNSUPPORTED_DEPTH - if the image file describes an image with an unsupported depth</li>
     *    <li>ERROR_UNSUPPORTED_FORMAT - if the image file contains an unrecognized format</li>
     * </ul>
     * @exception SWTError <ul>
     *    <li>ERROR_NO_HANDLES if a handle could not be obtained for image creation</li>
     * </ul>
     */
    public Image(Device device, String filename) {
        this(device, filename, true);
     }
    
    private Image(Device device, String filename, boolean securityCheck) {
        this(device);
        if (filename == null) {
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        }
        init();
        
        if (securityCheck == true) {
            // Drop the "file:///" prefix
            String trimmedFileName = filename.trim();
            final String prefix = "file:///";
            if (trimmedFileName.startsWith(prefix)) {
                trimmedFileName = trimmedFileName.substring(prefix.length());
            }
            filename = trimmedFileName;

            boolean canRead = false;
            try {
                canRead = Compatibility.canOpenFile(filename);
            } catch (SecurityException e) {
                SWT.error(SWT.ERROR_IO);
            }
            if (!canRead) {
                SWT.error(SWT.ERROR_IO);
            }
        }

        org.eclipse.swt.internal.qt.graphics.ImageLoader loader = new org.eclipse.swt.internal.qt.graphics.ImageLoader();
        setLoadSize(loader);
        try {
            cgImage = loader.nativelyLoadImageFromFileNoSecurity(filename);
        } catch (IllegalArgumentException e) {
            SWT.error(SWT.ERROR_UNSUPPORTED_FORMAT);
        } catch (IllegalStateException e) {
            SWT.error(SWT.ERROR_INVALID_IMAGE);
        } catch (IOException e) {
            SWT.error(SWT.ERROR_IO);
        } finally {
            loader.dispose();
        }
        track();
     }


    /*
     * Checks validity of the image object.
     * @exception SWTException <ul>
     *    <li>ERROR_INVALID_IMAGE - if the image is not valid</li>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the image is disposed</li>
     * </ul>
     */
    void checkValidity() {
        if ((type != SWT.BITMAP) && (type != SWT.ICON)) {
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        }
        if (isDisposed()) {
            SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        }
    }
    
    static Image createImageWithoutSecurityCheck(Device device, String filename) {
        return new Image(device, filename, false);
    }
    
    /*
     * Returns the bounds of an image without creating an Image instance.
     */
    static Point getImageSize(Device device, String filename) {
        
        if (filename == null) {
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        }
        
        InputStream is = device.getClass().getResourceAsStream(filename);
        
        if (is == null) {
            SWT.error(SWT.ERROR_IO);
        }
        
        return getImageSize(is);
    }
    
    /*
     * Returns the bounds of an image without creating an Image instance.
     */
    static Point getImageSize(InputStream stream) {
        Point point = null;
        
        try {
            point = org.eclipse.swt.internal.qt.graphics.ImageLoader.getImageSize(stream);
        } catch (IOException e) {
            SWT.error(SWT.ERROR_IO);
        } catch (NullPointerException e) {
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        } catch (IllegalArgumentException e) {
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        }
        
        return point;
    }

    /**
     * Disposes of the operating system resources associated with the image.
     */
    public void dispose() {
        if(cgImage != null) {
            cgImage.dispose();
            cgImage = null;
        }
        // Cleanup the icon associated with this
        if(icon != SWT.NULL) {
            OS.QIcon_delete(icon);
            icon = SWT.NULL;
        }

        if (device == null) return;
        if (device.isDisposed()) return;
        if (Device.tracking) device.dispose_Object(this);
        device = null;
    }

    /**
     * Compares the argument to the receiver, and returns true
     * if they represent the <em>same</em> object using a class
     * specific comparison.
     *
     * @param object the object to compare with this object
     * @return <code>true</code> if the object is the same as
     * this object and <code>false</code> otherwise
     *
     * @see #hashCode
     */
    public boolean equals (Object object) {
        if (object == this) {
            return true;
        }
        if (!(object instanceof Image)) {
            return false;
        }
        Image image = (Image)object;
        return (
            (device == image.device) &&
            (hashCode() == image.hashCode()));
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
    public Rectangle getBounds() {
        checkValidity();
        return new Rectangle(
            0, 0, cgImage.getWidth(), cgImage.getHeight());
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
    public ImageData getImageData() {
        checkValidity();
        return cgImage.getImageData();
    }

    /**
     * Returns an integer hash code for the receiver. Any two
     * objects that return <code>true</code> when passed to
     * <code>equals</code> must return the same value for this
     * method.
     *
     * @return the receiver's hash
     *
     * @see #equals
     */
    public int hashCode () {
        if(cgImage != null) {
            return cgImage.getHandle();
        }
        return SWT.DEFAULT;
    }

    /*
     * Helper method for constructing an empty instance of this class with the
     * specified width and height.
     */
    void init() {
        if(cgImage != null && !cgImage.isDisposed()) {
            cgImage.dispose();
        }
    }

    /*
     * Helper method for constructing an empty instance of this class with the
     * specified image data.
     *
     * @param imageData The image data
     */
    void init(ImageData image) {
        if (image == null) {
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        }

        if (!(((image.depth == 1 || image.depth == 2 ||
                image.depth == 4 || image.depth == 8) && !image.palette.isDirect) ||
             ((image.depth == 8) || (image.depth == 16 ||
                     image.depth == 24 || image.depth == 32) && image.palette.isDirect))) {
            SWT.error (SWT.ERROR_UNSUPPORTED_DEPTH);
        }

        init();

        int width = image.width;
        int height = image.height;
        PaletteData palette = image.palette;
        if (!(((image.depth == 1 || image.depth == 2 || image.depth == 4 || image.depth == 8) && !palette.isDirect) ||
        ((image.depth == 8) || (image.depth == 16 || image.depth == 24 || image.depth == 32) && palette.isDirect)))
            SWT.error (SWT.ERROR_UNSUPPORTED_DEPTH);
        byte[] buffer = image.data;
        if (image.depth != 32 || image.bytesPerLine != width*4) {
            buffer = new byte[width * 4 * height];
            if (palette.isDirect) {
                ImageData.blit(ImageData.BLIT_SRC,
                    image.data, image.depth, image.bytesPerLine, image.getByteOrder(), 0, 0, width, height, palette.redMask, palette.greenMask, palette.blueMask,
                    ImageData.ALPHA_OPAQUE, null, 0, 0, 0,
                    buffer, 32, width*4, ImageData.MSB_FIRST, 0, 0, width, height, 0x00FF0000, 0x0000FF00, 0x000000FF,
                    false, false);
                palette = new PaletteData(0x00FF0000, 0x0000FF00, 0x000000FF);
            } else {
                RGB[] rgbs = palette.getRGBs();
                int length = rgbs.length;
                byte[] srcReds = new byte[length];
                byte[] srcGreens = new byte[length];
                byte[] srcBlues = new byte[length];
                for (int i = 0; i < rgbs.length; i++) {
                    RGB rgb = rgbs[i];
                    if (rgb == null) continue;
                    srcReds[i] = (byte)rgb.red;
                    srcGreens[i] = (byte)rgb.green;
                    srcBlues[i] = (byte)rgb.blue;
                    }
                ImageData.blit(ImageData.BLIT_SRC,
                    image.data, image.depth, image.bytesPerLine, image.getByteOrder(), 0, 0, width, height, srcReds, srcGreens, srcBlues,
                    ImageData.ALPHA_OPAQUE, null, 0, 0, 0,
                    buffer, 32, width*4, ImageData.MSB_FIRST, 0, 0, width, height, 0x00FF0000, 0x0000FF00, 0x000000FF,
                    false, false);
            }

            image = new ImageData(width, height, 32, palette, 4, buffer, image.maskPad, image.maskData,
                                  image.alphaData, image.alpha, image.transparentPixel, image.type,
                                  image.x, image.y, image.disposalMethod, image.delayTime);
        }

        this.type = SWT.BITMAP;
        cgImage =
            new org.eclipse.swt.internal.qt.graphics.Image(image);

        try {
            if (image.transparentPixel != -1) {
                RGB rgb = null;
                if (palette.isDirect) {
                    rgb = palette.getRGB(image.transparentPixel);
                } else {
                    if (image.transparentPixel < palette.colors.length) {
                        rgb = palette.getRGB(image.transparentPixel);
                    }
                }

                if (rgb != null) {
                    transparentPixel = rgb.red << 24 | rgb.green << 16 | rgb.blue << 8 | 0xFF;
                }
            }
        } catch(Error e) {
            cgImage.dispose();
            throw e;
        }
    }

    /*
     * Helper method for constructing an empty instance of this class with the
     * specified width and height.
     *
     * @param width the width of the new image
     * @param height the height of the new image
     *
     * @see #Image(Device device, int width, int height)
     * @see #Image(Device device, Rectangle bounds)
     */
    void init(int width, int height) {
        if (width <= 0 || height <= 0) {
            SWT.error (SWT.ERROR_INVALID_ARGUMENT);
        }
        init();
        this.type = SWT.BITMAP;
        cgImage =
                new org.eclipse.swt.internal.qt.graphics.Image(
                                                    width, height);
    }

    /*
     * Helper method for constructing an instance of this class with the
     * specified internal image.
     *
     * @param iinternalImage The internal image.
     */
    void init(org.eclipse.swt.internal.qt.graphics.Image cgImage) {
        if (cgImage == null) {
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        }
        if (cgImage.isDisposed()) {
            SWT.error (SWT.ERROR_UNSUPPORTED_DEPTH);
        }
        init();
        this.type = SWT.BITMAP;
        this.cgImage = cgImage;
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
     * @param data the platform specific GC data
     * @exception SWTException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if a GCData is null</li>
     * </ul>
     */
    public void internal_dispose_GC(GCData data) {
        if(data == null || data.internalGc == null) {
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        }
        data.internalGc.releaseTarget();
        data.internalGc.dispose();
        data.internalGc = null;
    }

    /*
     * Returns OS specific icon handle.
     */
    int getIconHandle() {
        if(isDisposed()) {
            SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        }
        if(icon == SWT.NULL)    {
            icon = OS.QIcon_new(cgImage.getNativePixmapHandle());
        }
        return icon;
    }

    /*
     * Returns OS specific image instance.
     */
    org.eclipse.swt.internal.qt.graphics.Image getImage() {
        if(isDisposed()) {
            SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        }
        return cgImage;
    }

    /*
     * Returns a handle of OS specific image instance.
     */
    int getImageHandle() {
        if(isDisposed()) {
            SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        }
        return cgImage.getHandle();
    }

    /*
     * Returns OS specific pixmap handle.
     */
    int getPixmapHandle() {
        if(isDisposed())
            SWT.error(SWT.ERROR_GRAPHIC_DISPOSED);
        return cgImage.getNativePixmapHandle();
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
     * @exception SWTException <ul>
     *    <li>ERROR_GRAPHIC_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_INVALID_IMAGE - if the image is not a bitmap or an icon</li>
     *    <li>ERROR_INVALID_ARGUMENT - if a GC is already binded</li>
     *    <li>ERROR_NULL_ARGUMENT - if a GCData is null</li>
     * </ul>
     */
    public int internal_new_GC(GCData data) {
        checkValidity();
        if(data == null) {
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        }
        int paintDevice = OS.QPixmap_swt_paintDevice(cgImage.getNativePixmapHandle());
        if(OS.QPaintDevice_paintingActive(paintDevice)) {
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        }

        int mask = SWT.LEFT_TO_RIGHT | SWT.RIGHT_TO_LEFT;
        if ((data.style & mask) == 0) {
            data.style |= SWT.LEFT_TO_RIGHT;
        } else {
            if ((data.style & SWT.RIGHT_TO_LEFT) != 0) {
                data.style |= SWT.MIRRORED;
            }
        }

        data.internalGc = new GraphicsContext();
        data.internalGc.bindTarget(getImage());
        data.device = device;
        data.drawable = paintDevice;
        data.background = device.getSystemColor(SWT.COLOR_WHITE);
        data.foreground = device.getSystemColor(SWT.COLOR_BLACK);
        data.font = Font.qt_new(device, OS.SwtFontCache_cache(OS.QApplication_swt_font_new()));
        return data.internalGc.getHandle();
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
    public boolean isDisposed() {
        if(this.cgImage != null) {
            return cgImage.isDisposed();
        }
        return true;
    }

    void setLoadSize(org.eclipse.swt.internal.qt.graphics.ImageLoader loader) {
        Point loadSize = Internal_PackageSupport.readAndClearImageLoadSize((Display)device);
        if(loadSize != null) {
        	loader.setLoadSize(loadSize.x, loadSize.y);
        }
    }

    /**
     * Returns a string containing a concise, human-readable
     * description of the receiver.
     *
     * @return a string representation of the receiver
     */
    public String toString () {
        if (isDisposed()) return "Image {*DISPOSED*}";
        return "Image {" + cgImage.getHandle() + "}";
    }

    /*
     * Call this only after dealing with all exceptions!
     */
    private void track() {
        if (Device.tracking) device.new_Object(this);
    }
}
