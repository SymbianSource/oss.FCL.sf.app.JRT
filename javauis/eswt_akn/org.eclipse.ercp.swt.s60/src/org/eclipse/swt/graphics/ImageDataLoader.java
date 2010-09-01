/*******************************************************************************
 * Copyright (c) 2004, 2007 Nokia Corporation.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Yu You (Nokia Corp.)- initial API specification
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


package org.eclipse.swt.graphics;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import org.eclipse.swt.internal.FileCompatibility;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.internal.symbian.*;

class ImageDataLoader
{

    int logicalScreenWidth;
    int logicalScreenHeight;
    int backgroundPixel = -1;



    void save(OutputStream stream, int format, ImageData imageData)
    {

        int handle;
        handle = OS.ImageDataLoader_New();

        try
        {
            OS.ImageDataLoader_EncodeImageToStream(handle,imageData, format,stream);
        }
        catch (Error e)
        {
            OS.ImageDataLoader_Dispose(handle);
            throw e;
        }
        catch (RuntimeException r)
        {
            OS.ImageDataLoader_Dispose(handle);
            throw r;
        }


        OS.ImageDataLoader_Dispose(handle);
    }

    void save(String filename, int format, ImageData imageData)
    {

        int handle;
        handle = OS.ImageDataLoader_New();

        try
        {
            OS.ImageDataLoader_EncodeImage(handle,imageData, format,filename);
        }
        catch (Error e)
        {
            OS.ImageDataLoader_Dispose(handle);
            throw e;
        }
        catch (RuntimeException r)
        {
            OS.ImageDataLoader_Dispose(handle);
            throw r;
        }


        OS.ImageDataLoader_Dispose(handle);
    }



    public ImageData[] load(InputStream stream)
    {

        if (stream == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);

        byte receiveData[] = null;

        receiveData=Image.readImageStream(stream);

        // check the format
        int imageFormat = Image.imageFormat(receiveData);
        if (imageFormat == SWT.IMAGE_UNDEFINED) SWT.error(SWT.ERROR_UNSUPPORTED_FORMAT);
        if (imageFormat == SWT.IMAGE_GIF)
        {
            // The giff data must contains at least the GIF Signature (6 bytes) and the Screen Descriptor (8 bytes)
            if (receiveData.length < 14)
                SWT.error(SWT.ERROR_INVALID_IMAGE);
            backgroundPixel = getBackground(receiveData);
        }

        ImageData[] imageDataArray=null ;
        int handle = OS.ImageDataLoader_New();

        // decode image
        try
        {
            imageDataArray = OS.ImageDataLoader_DecodeWholeImageFromBuffer(handle, receiveData);
        }
        catch (Error e)
        {
            OS.ImageDataLoader_Dispose(handle);
            throw e;
        }
        catch (RuntimeException r)
        {
            OS.ImageDataLoader_Dispose(handle);
            SWT.error(SWT.ERROR_INVALID_IMAGE);
        }

        SetFields(handle, imageFormat, imageDataArray);

        OS.ImageDataLoader_Dispose(handle);

        if (imageDataArray == null) SWT.error(SWT.ERROR_INVALID_IMAGE);
        if (imageDataArray.length == 0) SWT.error(SWT.ERROR_INVALID_IMAGE);

        return imageDataArray;
    }

    public ImageData[] load(String filename)
    {

        if (filename == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        int imageFormat = Image.imageFormat(filename);
        if (imageFormat == SWT.IMAGE_UNDEFINED) SWT.error(SWT.ERROR_UNSUPPORTED_FORMAT);
        if (imageFormat == SWT.IMAGE_GIF) backgroundPixel = getBackground(filename);

        ImageData[] imageDataArray=null;
        int handle;
        handle = OS.ImageDataLoader_New();

        try
        {
            imageDataArray = OS.ImageDataLoader_DecodeImage(handle,filename);
        }
        catch (Error e)
        {
            OS.ImageDataLoader_Dispose(handle);
            throw e;
        }
        catch (RuntimeException r)
        {
            OS.ImageDataLoader_Dispose(handle);
            throw r;
        }

        SetFields(handle, imageFormat, imageDataArray);

        OS.ImageDataLoader_Dispose(handle);

        if (imageDataArray == null) SWT.error(SWT.ERROR_INVALID_IMAGE);
        if (imageDataArray.length == 0) SWT.error(SWT.ERROR_INVALID_IMAGE);

        return imageDataArray;
    }

    private void SetFields(int handle, int imageFormat, ImageData[] images)
    {
        if (images != null)
        {
            // logicalScreenValues
            logicalScreenHeight = OS.ImageDataLoader_GetLogicalScreenHeight(handle);
            logicalScreenWidth = OS.ImageDataLoader_GetLogicalScreenWidth(handle);

            // image type
            for (int count = 0; count<images.length; ++count)
            {
                images[count].type = imageFormat;
            }
        }
    }

    private int getBackground(String fileName)
    {
        InputStream stream = null;
        byte[] buffer = new byte[12];
        try
        {
            stream = FileCompatibility.getInputStreamFromFile(fileName);
            stream.read(buffer, 0, buffer.length);
            stream.close();
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
            catch (Throwable t)
            {
                // do nothing
            }
        }
        return getBackground(buffer);
    }
    private int getBackground(byte[] buffer)
    {
        if ((buffer[10] & 0x80) != 0) return buffer[11]&0xff;
        return -1;
    }
}









