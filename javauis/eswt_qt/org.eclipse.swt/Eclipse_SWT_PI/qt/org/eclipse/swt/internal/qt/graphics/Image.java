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


import org.eclipse.swt.graphics.ImageData;

public final class Image {

    /**
     * Specifies no transformation.
     */
    public static final int TRANS_NONE = 100;
    /**
     * Specifies transformation for 90 degrees clockwise
     * along z-axis.
     */
    public static final int TRANS_ROT90 = 101;
    /**
     * Specifies transformation for 180 degrees clockwise
     * along z-axis.
     */
    public static final int TRANS_ROT180 = 102;
    /**
     * Specifies transformation for 270 degrees clockwise
     * along z-axis.
     */
    public static final int TRANS_ROT270 = 103;
    /**
     * Specifies transformation for image mirror along y-axis.
     */
    public static final int TRANS_MIRROR = 104;
    /**
     * Specifies transformation for first image mirror along y-axis
     * and then 90 degrees rotation clockwise along z-axis.
     */
    public static final int TRANS_MIRROR_ROT90 = 105;
    /**
     * Specifies transformation for first image mirror along y-axis
     * and then 180 degrees rotation clockwise along z-axis.
     */
    public static final int TRANS_MIRROR_ROT180 = 106;
    /**
     * Specifies transformation for first image mirror along y-axis
     * and then 270 degrees rotation clockwise along z-axis.
     */
    public static final int TRANS_MIRROR_ROT270 = 107;

    /**
     * Specifies transformation for mirror the image along x-axis.
     */
    public static final int TRANS_FLIP_HORIZONTAL = 108;

    /**
     * Specifies transformation for mirror the image along y-axis.
     */
    public static final int TRANS_FLIP_VERTICAL = 109;

    /**
     * Specifies default image format.
     */
    public static final int FORMAT_IMG_NONE = 0;

    /**
     * Specifies 32 bit per pixel image format.
     */
    public static final int FORMAT_IMG_RGB32 = 1;

    /**
     * Specifies 32 bit per pixel image format with alpha channel.
     */
    public static final int FORMAT_IMG_ARGB32 = 2;

    /**
     * Specifies 32 bit per pixel image format with alpha channel.
     */
    public static final int FORMAT_IMG_ARGB32PREMULTIPLIED = 3;

    /**
     * Specifies 16 bit per pixel image format.
     */
    public static final int FORMAT_IMG_RGB16 = 4;

    /**
     * Specifies 16 bit per pixel image format.
     */
    public static final int FORMAT_IMG_RGB555 = 5;

    /**
     * Specifies 16 bit per pixel image format.
     */
    public static final int FORMAT_IMG_RGB444 = 6;

    /**
     * Specifies 16 bit per pixel image format.
     */
    public static final int FORMAT_IMG_RGB4444PREMULTIPLIED = 7;

    /**
     * Specifies 1 bit per pixel image format.
     */
    public static final int FORMAT_IMG_MONO = 8;

    // Native image handle
    int handle;
    // Handle of native QPixmap wrapped by image
    int pixmapHandle;
    // Status of native peer
    private boolean disposed;
    // Image dimensions
    private int w;
    private int h;
    
    // Image pixel format
    private int pixelFormat = FORMAT_IMG_NONE;

    // flag to indicate if this image is currently
    // bound as rendering target by GraphicsContext
    private boolean isBound = false;
    // GraphicsContext bound to this image
    private GraphicsContext boundingGc = null;

    /**
     * Constructs new image with given native image handle.
     *
     * @param imgHandle The handle of valid native image
     * @throws OutOfMemoryError if imgHandlde is invalid
     */
    Image(int imgHandle) {
    	Utils.validateUiThread();
        // validate handle
        if(imgHandle == 0) {
            throw new OutOfMemoryError();
        }
        // store handle & get dimensions
        handle = imgHandle;
        updateSize();
    }

    /**
     * Constructs new image filled with default fill color with specified width and height.
     *
     * @param width The width of new image
     * @param height The height of new image
     * @throws IllegalArgumetException if width or height is equal or less than zero
     * @throws OutOfMemoryError if memory allocation for new image fails
     */
    public Image(int width, int height) {
        Utils.validateUiThread();
        // check width and height
        if (width <= 0 || height <= 0) {
            throw new IllegalArgumentException("width or height is equal or less than 0");
        }

        // Construct image in native side and store the handle
        handle = OS.image_create(width, height, Config.IMAGE_DEFAULT_FILL_COLOR);     // may throw outOfMemoryError
        // set dimensions
        updateSize();
    }

    /**
     * Constructs new image with specified width, height and fill color.
     * The RGB values passed in is interpreted with the least significant eight bits giving the blue
     * component, the next eight more significant bits giving the green component, and the next eight
     * bits giving the red component. The high order byte of this value, i.e. alpha, is ignored.
     *
     * @param width The width of new image
     * @param height The height of new image
     * @param fillColor The initial fill color for the image in format #00RRGGBB.
     * @throws IllegalArgumetException if width or height is equal or less than zero
     * @throws OutOfMemoryError if memory allocation for new image fails
     */
    public Image(int width, int height, int fillColor) {
        Utils.validateUiThread();
        // check width and height
        if (width <= 0 || height <= 0) {
            throw new IllegalArgumentException("width or height is equal or less than 0");
        }

        // Construct image in native side and store the handle
        handle = OS.image_create(width, height, fillColor);     // may throw outOfMemoryError
        // set dimensions
        updateSize();
    }

    /**
     * Constructs a new instance of this class based on given image.
     *
     * @param sourceImage The image used as source
     * @throws NullPointerException if sourceImage is null
     * @throws OutOfMemoryError if memory allocation for new image fails
     */
    public Image(Image sourceImage) {
        this(sourceImage, 0, 0, 0, 0);
    }

    /**
     * Constructs a new instance of this class based on given image.
     * If the given copy region is empty, the whole image is copied.
     *
     * @param sourceImage The image used as source
     * @param x The top-left x coordinate of the copy region.
     * @param y The top-left y coordinate of the copy region.
     * @param width The width of the copy region.
     * @param height The height of the copy region.
     * @throws NullPointerException if sourceImage is null
     */
    public Image(Image sourceImage, int x, int y, int width, int height) {
        // validate sourceImage
        if (sourceImage == null) {
            throw new NullPointerException("img is null");
        }
        // Construct image in native side and store the handle
        handle = OS.image_create(sourceImage.handle, x, y, width, height); // may throw outOfMemoryError
        // set dimensions
        updateSize();
    }

    /**
     * Creates a new image from given ImageData
     * @param imageData
     * @throws NullPointerException if imageData is null
     */
    public Image(ImageData imageData) {
        if (imageData == null) {
            throw new NullPointerException("imageData is null");
        }
        handle = OS.image_create(imageData);
        // set dimensions
        updateSize();
    }

    /**
     * Constructs a new instance of this class based on the given rgb data.
     *
     * @param argbData a RGB data array where one pixel is specified as 0xAARRGGBB.
     * @param width The width of the image
     * @param height The height of the image
     * @param hasAlpha If true the rgb data has also an alpha channel,
     * otherwise all pixels are fully opaque, e.g. 0xFFRRGGBB.
     * @throws NullPointerException if the <code>argbData</code> is null
     *
     */
    public Image(int[] argbData, int width, int height, boolean hasAlpha) {
        // input validation
        if(argbData == null) {
            throw new NullPointerException("argbData is null");
        }
        // Construct an ge in native side and store the handle
        handle = OS.image_create(argbData, width, height, hasAlpha);
        // set dimensions
        updateSize();
    }

    /**
     * Disposes the instance if this image and frees memory.
     * After disposing image the only valid metdod is isDisposed(), other
     * methods will throw illegalStateException.
     *
     * If this instance is being disposed before gc has released it, the
     * releaseTarget() of binding gc is called automatically.
     */
    public void dispose() {
    	Utils.validateUiThread();
        if (handle != 0 || !disposed) {
            // If this instance is being disposed while
            // gc is still bound, release binding before disposing.
            if(isBound) {
                isBound = false;
                if(boundingGc != null) {
                    boundingGc.releaseTarget();
                }
                boundingGc = null;
            }

            // Delete window surface from cache in case
            // it is created for this instance
            SurfaceCache.getInstance().deleteSurface(this);

            // dispose native peer
            OS.image_dispose(handle);
            handle = 0;
            pixmapHandle = 0;
            disposed = true;
        }
    }

    /**
     * Gets the state of the image, is it disposed or not.
     *
     * @return true if this instanace has been disposed, otherwise false
     */
    public boolean isDisposed() {
        return disposed;
    }

    /**
     * Gets the format of the image.
     *
     * @return image format
     */
    public int getFormat() {
    	checkState();
    	if (pixelFormat == FORMAT_IMG_NONE) {
    		pixelFormat = OS.image_getFormat(handle);
    	}
        return pixelFormat;
    }
    /**
     * Gets the height of the image in pixels.
     *
     * @return The height of this image
     * @throws IllegalStateException if the image has already been disposed
     */
    public int getHeight() {
        checkState();
        return h;
    }

    /**
     * Gets the width of the image in pixels.
     *
     * @return The width of this image
     * @throws IllegalStateException if the image has already been disposed
     */
    public int getWidth() {
        checkState();
        return w;
    }

    /**
     * Copies image data to given integer array from rectangle specified
     * by x, y, width and height.
     *
     * @param argbData The integer array where image data is copied to
     * @param offset The index into the array where first pixel is stored
     * @param scanlength The relative offset in the array between pixles in consecutive rows of the region
     * @param x The x-coordinate of upper left corner of the region
     * @param y The y-coordinate of upper left corner of the region
     * @param width The width of the region
     * @param height The height of the region
     *
     * @throws ArrayIndexOutOfBoundsException if the operation tries to access an element in argbData outside is range
     * @throws IllegalArgumentException if the absolute value of scanlength is less than width
     * @throws IllegalArgumentException if area to be retrieved exceed the bounds of the image
     * @throws NullPointerException if argbData is null
     * @throws OutOfMemoryError if memory allocation failure occurs during operation
     * @throws IllegalStateException if the image has already been disposed
     */
    public void getRGB(int[] argbData, int offset, int scanlength,
                       int x, int y, int width, int height) {
        checkState();

        // input validation
        if(argbData == null) {
            throw new NullPointerException("argbData is null");
        }
        if(((x+width) > getWidth()) || ((y+height) > getHeight()) || (x < 0) || (y < 0)) {
                throw new IllegalArgumentException("Area specified is out of image bounds");
        }
        if(scanlength < 0) {
            // End of first row
            int a = width - 1;
            int b = 0;
            int index = offset + (a - x) + (b -y) * scanlength;
            if(index > argbData.length - 1) {
                throw new ArrayIndexOutOfBoundsException("Data does not fit in given array");
            }
            // Beginning of last row
            a = 0;
            b = height - 1;
            index = offset + (a - x) + (b -y) * scanlength;
            if(index < 0) {
                throw new ArrayIndexOutOfBoundsException("Data does not fit in given array");
            }
        } else {
            if((argbData.length < (offset + (scanlength * height))) || offset < 0) {
                throw new ArrayIndexOutOfBoundsException("Data does not fit in given array");
            }
        }
        if((scanlength > 0 ? scanlength : -scanlength) < width) {
            throw new IllegalArgumentException("scanlength is less than width");
        }


        OS.image_getRGB(handle, argbData, offset, scanlength, x, y, width, height);
    }


    /**TODO: change comments
     * Copies image data to given integer array from rectangle specified
     * by x, y, width and height.
     *
     * @param argbData The byte array where image data is copied to
     * @param transparencyMask the byte array where mask values are copied to
     * @param offset The index into the array where first pixel is stored
     * @param scanlength The relative offset in the array between pixles in consecutive rows of the region
     * @param x The x-coordinate of upper left corner of the region
     * @param y The y-coordinate of upper left corner of the region
     * @param width The width of the region
     * @param height The height of the region
     * @param format The format in which pixels are stored into argbData
     * @throws ArrayIndexOutOfBoundsException if the operation tries to access an element in argbData outside is range
     * @throws IllegalArgumentException if the absolute value of scanlength is less than width
     * @throws IllegalArgumentException if area to be retrieved exceed the bounds of the image
     * @throws NullPointerException if argbData is null
     * @throws OutOfMemoryError if memory allocation failure occurs during operation
     * @throws IllegalStateException if the image has already been disposed
     */
    public void getRGB(byte[] argbData, byte[] transparencyMask, int offset, int scanlength,
                       int x, int y, int width, int height, int format) {
        checkState();

        // input validation
        if(argbData == null) {
            throw new NullPointerException("argbData is null");
        }
        if(transparencyMask == null) {
            throw new IllegalArgumentException("transparencyMask is null");
        }
        if(((x+width) > getWidth()) || ((y+height) > getHeight()) || (x < 0) || (y < 0)) {
                throw new IllegalArgumentException("Area specified is out of image bounds");
        }
        if(scanlength < 0) {
            // End of first row
            int a = width - 1;
            int b = 0;
            int index = offset + (a - x) + (b -y) * scanlength;
            if(index > argbData.length - 1) {
                throw new ArrayIndexOutOfBoundsException("Data does not fit in given array");
            }
            if(index > transparencyMask.length - 1) {
                throw new ArrayIndexOutOfBoundsException("Mask does not fit in given array");
            }
            // Beginning of last row
            a = 0;
            b = height - 1;
            index = offset + (a - x) + (b -y) * scanlength;
            if(index < 0) {
                throw new ArrayIndexOutOfBoundsException("Data does not fit in given array");
            }
        } else {
            if((argbData.length < (offset + (scanlength * height))) || offset < 0) {
                throw new ArrayIndexOutOfBoundsException("Data does not fit in given array");
            }
            if((transparencyMask.length < (offset + (scanlength * height))) || offset < 0) {
                throw new ArrayIndexOutOfBoundsException("Mask does not fit in given array");
            }
        }
        if((scanlength > 0 ? scanlength : -scanlength) < width) {
            throw new IllegalArgumentException("scanlength is less than width");
        }


        OS.image_getRGB(handle, argbData, transparencyMask,offset, scanlength, x, y, width, height, format);
    }


    /**
     * Copies image data to given integer array from rectangle specified
     * by x, y, width and height.
     *
     * @param argbData The short array where image data is copied to
     * @param offset The index into the array where first pixel is stored
     * @param scanlength The relative offset in the array between pixles in consecutive rows of the region
     * @param x The x-coordinate of upper left corner of the region
     * @param y The y-coordinate of upper left corner of the region
     * @param width The width of the region
     * @param height The height of the region
     * @param format Image format in which pixels are store into argbData //TODO:add explanation when format are addewd to image.
     *
     * @throws ArrayIndexOutOfBoundsException if the operation tries to access an element in argbData outside is range
     * @throws IllegalArgumentException if the absolute value of scanlength is less than width
     * @throws IllegalArgumentException if area to be retrieved exceed the bounds of the image
     * @throws NullPointerException if argbData is null
     * @throws OutOfMemoryError if memory allocation failure occurs during operation
     * @throws IllegalStateException if the image has already been disposed
     */
    public void getRGB(short[] argbData, int offset, int scanlength,
                       int x, int y, int width, int height, int format) {
        checkState();

        // input validation
        if(argbData == null) {
            throw new NullPointerException("argbData is null");
        }
        if(((x+width) > getWidth()) || ((y+height) > getHeight()) || (x < 0) || (y < 0)) {
                throw new IllegalArgumentException("Area specified is out of image bounds");
        }
        if(scanlength < 0) {
            // End of first row
            int a = width - 1;
            int b = 0;
            int index = offset + (a - x) + (b -y) * scanlength;
            if(index > argbData.length - 1) {
                throw new ArrayIndexOutOfBoundsException("Data does not fit in given array");
            }
            // Beginning of last row
            a = 0;
            b = height - 1;
            index = offset + (a - x) + (b -y) * scanlength;
            if(index < 0) {
                throw new ArrayIndexOutOfBoundsException("Data does not fit in given array");
            }
        } else {
            if((argbData.length < (offset + (scanlength * height))) || offset < 0) {
                throw new ArrayIndexOutOfBoundsException("Data does not fit in given array");
            }
        }
        if((scanlength > 0 ? scanlength : -scanlength) < width) {
            throw new IllegalArgumentException("scanlength is less than width");
        }


        OS.image_getRGB(handle, argbData, offset, scanlength, x, y, width, height, format);
    }

    /**
     * Pixel level collision detection.
     */
    public static boolean detectCollision(Image image1, int transform1, int p1x, int p1y, int r1x1, int r1y1, int r1x2, int r1y2,
                                          Image image2, int transform2, int p2x, int p2y, int r2x1, int r2y1, int r2x2, int r2y2) {

        return OS.image_detectCollision(
                image1.getNativePixmapHandle(), transform1, p1x, p1y, r1x1, r1y1, r1x2, r1y2,
                image2.getNativePixmapHandle(), transform2, p2x, p2y, r2x1, r2y1, r2x2, r2y2);
    }

    /**
     * Transforms image with given transform.
     *
     * @throws IllegalArgumentException if transform is not valid
     * @throws IllegalStateException if the image has already been disposed
     */
    public void transform(int transform) {
        checkState();

        // validate transform
        if (transform != TRANS_NONE &&
            transform != TRANS_ROT90 &&
            transform != TRANS_ROT180 &&
            transform != TRANS_ROT270 &&
            transform != TRANS_MIRROR &&
            transform != TRANS_MIRROR_ROT90 &&
            transform != TRANS_MIRROR_ROT180 &&
            transform != TRANS_MIRROR_ROT270 ) {
            throw new IllegalArgumentException("Invalid transform");
        }

        // if no transformation is requested, return
        if(transform == TRANS_NONE) {
            return;
        }

        // perform transform
        OS.image_transform(handle, transform);

        // update width and height
        updateSize();
    }

    /**
     * Package private method to infrom this image that
     * it has been bound as rendering target by GraphicsContext.
     *
     * @param gc The GraphicsContext that binds this image as target
     */
    void notifyBind(GraphicsContext gc) {
        boundingGc = gc;
        isBound = true;
    }

    /**
     * Package private method to infrom this image that
     * it is no longer as a rendering target of gc.
     */
    void notifyRelease() {
        boundingGc = null;
        isBound = false;
    }

    /**
     * Returns native side handle.
     * @return the native handle.
     */
    public int getHandle()
    {
    	checkState();
        return handle;
    }

    /**
     * Returns native side QPixmap handle.
     * @return the native QPixmap handle.
     */
    synchronized public int getNativePixmapHandle()
    {
    	checkState();
        if(pixmapHandle == 0)
        {
            // In the current implementation this will return
            // pointer to the already existing QPixmap (which
            // will be destroyed with the object), so the
            // handle does not need to be released manually.
            pixmapHandle = OS.image_getPixmapHandle(handle);
        }
        return pixmapHandle;
    }

    /**
    /**
     * Creates <code>ImageData</code> objects
     * @return New object
     */
    public ImageData getImageData() {
    	checkState();
        return OS.image_getImageData(handle);
    }

    /**
     * Private helper to check the state of the current instance.
     */
    private void checkState() {
    	Utils.validateUiThread();
        if(disposed) {
            throw new IllegalStateException("Image already disposed");
        }
    }

    /**
     * private helper for updating iWidth and iHeight from native image.
     */
    private void updateSize() {
        w = OS.image_getWidth(handle);
        h = OS.image_getHeight(handle);
    }

    /**
     * Contructs an instance of Image based on the given <code>imageData</code>.
     *
     * @param imageData Image data @see org.eclipse.swt.graphics.ImageData
     * @return Instance of loaded image
     * @throws IllegalArgumentException if <code>imageData</code> is null.
     */
    public static Image createImage(ImageData imageData) {
        if (imageData == null) {
            throw new IllegalArgumentException("imageData is null");
        }
        int imageHandle = OS.image_create(imageData);
        return new Image(imageHandle);
    }
}
