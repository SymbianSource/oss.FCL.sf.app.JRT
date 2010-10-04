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

import java.io.*;

import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Device;

/**
 * ImageLoader is class for image loading from various data formats.
 *
 *
 */

final public class ImageLoader {

    /**
     * Size of block, in bytes, used for transferring read data to native side in iterative way.
     * Value given for this field affects on the memory consumption peak when loading
     * data trough java to native buffer. If value is very small, i.e. 100 the transfer
     * takes place many times and may result in performance loss, however memory usage will
     * be steadier. With bigger value transfer is faster but, it creates steeper peak in
     * memory consumption.
     *
     * If value is negative or zero, then block size is set based on InputStream.available(), otherwise
     * given positive integer value is used as block size (e.g. 4096).
     * Negative values are not allowed.
     */
    private static final int BLOCKSIZE = 0;

    /**
     * Native imageloader handle
     */
    private int handle = 0;

    /**
     * Status of native peer
     */
    private boolean disposed;

    /**
     * Creates an instance of this class and initializes native
     * image loader.
     */
    public ImageLoader() {
    	Utils.validateUiThread();
        handle = OS.imageLoader_init();
        if (handle == 0) {
            throw new OutOfMemoryError();
        }
        disposed = false;
    }

    /**
     * Disposes native image, i.e. frees resources.
     */
    public void dispose() {
    	Utils.validateUiThread();
        if (disposed) {
            return;
        } else {
            OS.imageLoader_dispose(handle);
            handle = 0;
            disposed = true;
        }
    }

    /**
     * Gets the status of this image loader.
     *
     * @return True if this instance has been already disposed otherwise false
     */
    public boolean isDisposed() {
        return disposed;
    }

    /**
     * Loads image from path specified.
     * Throws an error of image loading fails or it is unsupported format.
     *
     * @param path The path for image to be loaded
     * @return Instance of loaded image
     * @throws NullPointerException if path is null
     * @throws OutOfMemoryError if memory allocation for new image fails
     * @throws IOException if image cannot be loaded
     * @throws IllegalStateException if image data is invalid
     * @throws IllegalArgumentException if image format is not supported
     */
    public Image loadImage(String path) throws IOException {
        checkState();

        if (path == null) {
            throw new NullPointerException("path is null");
        }

        InputStream is = getClass().getResourceAsStream(path);

        if (is == null) {
            throw new IOException("Cannot obtain inputstream for given path: " +path);
        }
        return loadImage(is);
    }

    /**
     * Loads image from given byte array.
     *
     * @param data The array containing image data
     * @param offset The offset from beginnig of data where image data starts
     * @param length The length of data beginning form offset
     * @return Instance of loaded image
     * @throws NullPointerException if data is null
     * @throws IllegalArgumentException if offset is less than 0
     * @throws IllegalArgumentException if length is equal or less than 0
     * @throws ArrayIndexOutOfBoundsException if offset and length define an invalid range
     * @throws IOException if image cannot be loaded or it is unsupported format
     * @throws OutOfMemoryError if native buffer allocation fails
     */
    public Image loadImage(byte[] data, int offset, int length) throws IOException {
        checkState();

        if (data == null) {
            throw new NullPointerException("data is null");
        }
        if (offset < 0) {
            throw new IllegalArgumentException("offset is negative");
        }
        if (length <= 0) {
            throw new IllegalArgumentException("length is negative or zero");
        }
        if ((offset + length) > data.length) {
            throw new ArrayIndexOutOfBoundsException("offset and lenght define invalid range");
        }

        // All checks ok, so upload data to loader, with one go
        OS.imageLoader_beginStream(handle, length + 1);
        OS.imageLoader_append(handle, length, offset, data);

        return new Image(OS.imageLoader_endStream(handle));
    }

	/**
	 * Loads image from file directly using the native APIs. Note that Java
	 * security checks are not done.
	 *
	 * @param filename The filename to pass to the native APIs.
	 * @return Image The loaded image.
	 * @throws IOException
	 */
    public Image nativelyLoadImageFromFileNoSecurity(String filename) throws IOException {
        checkState();
        if (filename == null) {
            throw new NullPointerException("data is null");
        }
        return new Image(OS.imageLoader_load(handle, filename));
    }

    /**
     * Contructs an instance of Image by reading image data from given InputStream.
     *
     * @param is The InputStream from where to read the image data from
     * @return Instance of loaded image
     * @throws NullPointerException if InputStream is is null
     * @throws IOException if image cannot be loaded
     * @throws OutOfMemoryError if allocation of native buffer of image creation fails
     * @throws IllegalStateException if image data is invalid
     * @throws IllegalArgumentException if image format is not supported
     */
    public Image loadImage(InputStream is) throws IOException {
        checkState();

        if (is == null) {
            throw new NullPointerException("InputStream is null");
        }

        int bytesRead = 0;
        int bytesAvailable = 0;
        byte[] data;

        // Determine transfer buffer size
        if (BLOCKSIZE <= 0) {
            bytesAvailable = is.available(); // may throw IOException

            if (bytesAvailable == 0) {
                throw new IllegalArgumentException("Empty file");
            }

            data = new byte[bytesAvailable];
        } else {
            data = new byte[BLOCKSIZE];
        }

        // Start streaming to native buffer, with initial buffer size
        OS.imageLoader_beginStream(handle, bytesAvailable);

        // Read input stream and pass blocks to native buffer
        try {
            while ((bytesRead = is.read(data, 0, data.length)) != -1) {
                OS.imageLoader_append(handle, bytesRead, 0, data);
            }
        } catch (IOException e) {
            // Throw exception forward, native loader automatically resets state
            // when problems occur so there's no need to do any extra native calls here
            throw e;
        }
        catch (IllegalStateException e) {
            throw e;
        }
        catch (IllegalArgumentException e) {
            throw e;
        }
        return new Image(OS.imageLoader_endStream(handle));
    }

    /**
     * Sets the size that the Image will be scaled to when loaded. Useful for
     * SVG images.
     * @param width The width to scale to
     * @param height The height to scale to
     */
    public void setLoadSize(int width, int height) {
    	OS.imageLoader_setLoadSize(handle, width, height);
    }

    /**
     * Returns the bounds of an Image without creating an actual Image instance.
     *
     * @param is The InputStream from where to read the image data from
     * @return Bounds of the image
     */
    public static Point getImageSize(InputStream is) throws IOException {

        if (is == null) {
            throw new NullPointerException("InputStream is null");
        }

        int bytesAvailable = is.available(); // may throw IOException

        if (bytesAvailable == 0) {
            throw new IllegalArgumentException("Empty file");
        }

        byte [] data = new byte[bytesAvailable];
        
        if (is.read(data, 0, data.length) != bytesAvailable) {
            throw new IOException("Could not load data from InputStream");
        }
        
        return OS.imageLoader_getImageSize(data);
    }

    /**
     * Private helper to check the state of the current instance.
     */
    private void checkState() {
    	Utils.validateUiThread();
        if (disposed) {
            throw new IllegalStateException("Image loader already disposed");
        }
    }
}
