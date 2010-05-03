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
import org.eclipse.swt.graphics.ImageDataLoader;
import org.eclipse.swt.internal.FileCompatibility;


/**
 * Instances of this class are used to load images from,
 * and save images to, a file or stream.
 * <p>
 * Currently supported image formats are:
 * </p><ul>
 * <li>JPEG</li>
 * <li>GIF</li>
 * <li>PNG</li>
 * </ul>
 * <code>ImageLoaders</code> can be used to:
 * <ul>
 * <li>load single images in all formats</li>
 * <li>load multiple images (GIF)</li>
 * <li>load animated GIF images</li>
 * <li>load interlaced GIF/PNG images</li>
 * <li>load progressive JPEG images</li>
 * </ul>
 */

public class ImageLoader
{

    /**
     * the array of ImageData objects in this ImageLoader.
     * This array is read in when the load method is called,
     * and it is written out when the save method is called
     */
    public ImageData[] data;

    /**
     * the width of the logical screen on which the images
     * reside, in pixels (this corresponds to the GIF89a
     * Logical Screen Width value)
     */
    public int logicalScreenWidth;

    /**
     * the height of the logical screen on which the images
     * reside, in pixels (this corresponds to the GIF89a
     * Logical Screen Height value)
     */
    public int logicalScreenHeight;

    /**
     * the background pixel for the logical screen (this
     * corresponds to the GIF89a Background Color Index value).
     * The default is -1 which means 'unspecified background'
     *
     */
    public int backgroundPixel;

    /**
     * the number of times to repeat the display of a sequence
     * of animated images (this corresponds to the commonly-used
     * GIF application extension for "NETSCAPE 2.0 01").
     * The default is 1. A value of 0 means 'display repeatedly'
     */
    public int repeatCount;



    /*
     * The Decoder we use
     */
    private ImageDataLoader imageDataLoader;
    /**
     * Construct a new empty ImageLoader.
     */
    public ImageLoader()
    {
        imageDataLoader = new ImageDataLoader();
        reset();
    }

    /**
     * Resets the fields of the ImageLoader, except for the
     * <code>imageLoaderListeners</code> field.
     */
    void reset()
    {
        data = null;
        logicalScreenWidth = 0;
        logicalScreenHeight = 0;
        backgroundPixel = -1;
        repeatCount = 1;
    }
    /**
     * Loads an array of <code>ImageData</code> objects from the
     * specified input stream. Throws an error if either an error
     * occurs while loading the images, or if the images are not
     * of a supported type. Returns the loaded image data array.
     *
     * @param stream the input stream to load the images from
     * @return an array of <code>ImageData</code> objects loaded from the specified input stream
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the stream is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_INVALID_IMAGE - if the image file contains invalid data</li>
     *    <li>ERROR_IO - if an input/output error occurs while reading data</li>
     *    <li>ERROR_UNSUPPORTED_FORMAT - if the image file contains an unrecognized format</li>
     * </ul>
     */
    public ImageData[] load(InputStream stream)
    {
        reset();
        data = imageDataLoader.load(stream);
        SetFields();
        return data;
    }

    /**
     * Loads an array of <code>ImageData</code> objects from the
     * file with the specified name. Throws an error if either
     * an error occurs while loading the images, or if the images are
     * not of a supported type. Returns the loaded image data array.
     *
     * @param filename the name of the file to load the images from
     * @return an array of <code>ImageData</code> objects loaded from the specified file
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the file name is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_INVALID_IMAGE - if the image file contains invalid data</li>
     *    <li>ERROR_IO - if an IO error occurs while reading data</li>
     *    <li>ERROR_UNSUPPORTED_FORMAT - if the image file contains an unrecognized format</li>
     * </ul>
     */
    public ImageData[] load(String filename)
    {

        if (filename == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        reset();
        InputStream stream = null;
        try
        {
            stream = FileCompatibility.getInputStreamFromFile(filename);
            if (stream != null)
            {
                data = imageDataLoader.load(stream);
                SetFields();
                stream.close();
            }
        }
        catch (IOException e)
        {
            SWT.error(SWT.ERROR_IO, e);
        }
        if (stream == null)
        {
            SWT.error(SWT.ERROR_IO);
        }
        return data;
    }

    /**
     * Saves the image data in this ImageLoader to the specified stream.
     * The format parameter can have one of the following values:
     * <dl>
     * <dt><code>IMAGE_GIF</code></dt>
     * <dd>GIF file format</dd>
     * <dt><code>IMAGE_JPEG</code></dt>
     * <dd>JPEG file format</dd>
     * <dt><code>IMAGE_PNG</code></dt>
     * <dd>PNG file format</dd>
     * </dl>
     *
     * @param stream the output stream to write the images to
     * @param format the format to write the images in
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the stream is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_INVALID_IMAGE if the image data contains invalid data</li>
     *    <li>ERROR_IO if an IO error occurs while writing to the file</li>
     *    <li>ERROR_UNSUPPORTED_FORMAT - if the image file contains an unrecognized format</li>
     * </ul>
     */
    public void save(OutputStream stream, int format)
    {
        if (stream == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        if (format != SWT.IMAGE_GIF && format != SWT.IMAGE_PNG && format != SWT.IMAGE_JPEG)
            SWT.error(SWT.ERROR_UNSUPPORTED_FORMAT);
        if (data == null) SWT.error(SWT.ERROR_INVALID_IMAGE);
        imageDataLoader.save(stream, format,data[0]);
    }

    /**
     * Saves the image data in this ImageLoader to a file with the specified name.
     * The format parameter can have one of the following values:
     * <dl>
     * <dt><code>IMAGE_GIF</code></dt>
     * <dd>GIF file format</dd>
     * <dt><code>IMAGE_JPEG</code></dt>
     * <dd>JPEG file format</dd>
     * <dt><code>IMAGE_PNG</code></dt>
     * <dd>PNG file format</dd>
     * </dl>
     *
     * @param filename the name of the file to write the images to
     * @param format the format to write the images in
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the file name is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_INVALID_IMAGE if the image data contains invalid data</li>
     *    <li>ERROR_IO if an IO error occurs while writing to the file</li>
     * </ul>
     */
    public void save(String filename, int format)
    {
        if (filename == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        if (format != SWT.IMAGE_GIF && format != SWT.IMAGE_PNG && format != SWT.IMAGE_JPEG)
            SWT.error(SWT.ERROR_UNSUPPORTED_FORMAT);

        try
        {
            FileCompatibility.getOutStreamFromFile(filename);
        }
        catch (IOException e)
        {
            SWT.error(SWT.ERROR_IO, e);
        }

        imageDataLoader.save(FileCompatibility.convertPathFromSpecific(filename), format, data[0]);
    }


    private void SetFields()
    {
        logicalScreenHeight = imageDataLoader.logicalScreenHeight;
        logicalScreenWidth = imageDataLoader.logicalScreenWidth;
        backgroundPixel = imageDataLoader.backgroundPixel;
    }


}
