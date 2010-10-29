/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Icon conversion utility implemented using eSWT.
*
*/


package com.nokia.mj.impl.installer.utils;

import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.util.jar.JarFile;
import java.util.jar.JarEntry;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.ImageData;
import org.eclipse.swt.graphics.ImageLoader;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.internal.extension.Toolkit;
import org.eclipse.swt.widgets.Display;

/**
 * Icon conversion utility implemented using eSWT.
 */
public class IconConverterEswt
{
    /** List of unsupported bitmap image format filename extensions. */
    private static String[] iUnsupportedBitmapFormats =
        new String[] { ".wbmp", ".wmf" };

    /**
     * Converts icon to platform specific format.
     *
     * @param aInputIconFilename file name for input icon file
     * @param aOutputIconFilename file name for output icon file
     * @param aJarFilename jar file name if aInputIconFilename specifies
     *    file inside jar file,
     *    or null if aInputIconFilename specifies file from disk
     * @param aIconSuffix the correct suffix of the icon is returned through
     *    this parameter, will contain '.png' or '.svg' when function returns
     * @return true if the conversion succeeds
     */
    public static boolean convertIcon(
        String aInputIconFilename, String aOutputIconFilename,
        String aJarFilename, StringBuffer aIconSuffix)
    {
        Log.log("IconConverterEswt.convertIcon: " + aInputIconFilename +
                " to " + aOutputIconFilename + " from " + aJarFilename);
        boolean result = false;
        try
        {
            if (isSvgIcon(aInputIconFilename, aJarFilename))
            {
                result = convertSvgIcon(
                    aInputIconFilename, aOutputIconFilename,
                    aJarFilename, aIconSuffix);
            }
            else
            {
                result = convertBitmapIcon(
                    aInputIconFilename, aOutputIconFilename,
                    aJarFilename, aIconSuffix);
            }
        }
        catch (Throwable t)
        {
            Log.logWarning("IconConverterEswt.convertIcon: Exception when " +
                           "converting " + aInputIconFilename + " to " +
                           aOutputIconFilename + " from " + aJarFilename, t);
        }
        return result;
    }

    /**
     * Converts bitmap icon to platform specific format.
     *
     * @param aInputIconFilename file name for input icon file
     * @param aOutputIconFilename file name for output icon file
     * @param aJarFilename jar file name if aInputIconFilename specifies
     *    file inside jar file,
     *    or null if aInputIconFilename specifies file from disk
     * @param aIconSuffix the correct suffix of the icon is returned through
     *    this parameter
     * @return true if the conversion succeeds
     */
    private static boolean convertBitmapIcon(
        String aInputIconFilename, String aOutputIconFilename,
        String aJarFilename, StringBuffer aIconSuffix)
    {
        if (!isSupportedBitmapIcon(aInputIconFilename, aJarFilename))
        {
            Log.logWarning("IconConverterEswt.convertBitmapIcon: unsupported " +
                           "image format " + aInputIconFilename + " from " +
                           aJarFilename + " not converted");
            return false;
        }

        final Display display = Toolkit.getInternalDisplay();
        if (display == null)
        {
            Log.logWarning("IconConverterEswt: Getting Display failed, " +
                           "icon conversion aborted for " + aInputIconFilename +
                           " from " + aJarFilename);
            return false;
        }

        final Point iconSize = new Point(88, 88);
        if (iconSize != null)
        {
            display.syncExec(new Runnable()
            {
                public void run()
                {
                    display.setData("org.eclipse.swt.internal.image.loadSize",
                                    iconSize);
                }
            });
        }

        Image image = null;
        if (aJarFilename == null)
        {
            // Load image from file.
            image = new Image(display, aInputIconFilename);
        }
        else
        {
            // Load image from jar file.
            JarFile jarFile = null;
            try
            {
                jarFile = new JarFile(aJarFilename);
                InputStream inputStream = jarFile.getInputStream(
                    new JarEntry(FileUtils.trimJarEntry(aInputIconFilename)));
                image = new Image(display, inputStream);
            }
            catch (IOException ioe)
            {
                Log.logWarning("IconConverterEswt: Getting icon " +
                               aInputIconFilename + " from " +
                               aJarFilename + " failed", ioe);
            }
            finally
            {
                if (jarFile != null)
                {
                    try
                    {
                        jarFile.close(); // Closes also InputStream.
                        jarFile = null;
                    }
                    catch (IOException ioe)
                    {
                        Log.logWarning("IconConverterEswt: Closing " +
                                       aJarFilename + " failed", ioe);
                    }
                }
            }
        }

        boolean result = false;
        if (image != null)
        {
            // Save image to file.
            OutputStream outputStream = null;
            try
            {
                outputStream = FileUtils.getOutputStream(aOutputIconFilename);
                ImageLoader imageLoader = new ImageLoader();
                imageLoader.data = new ImageData[] { image.getImageData() };
                imageLoader.save(outputStream, SWT.IMAGE_PNG);
                aIconSuffix.append(".png");
                result = true;
            }
            catch (IOException ioe)
            {
                Log.logWarning("IconConverterEswt: Saving icon " +
                               aOutputIconFilename + " failed", ioe);
            }
            finally
            {
                if (outputStream != null)
                {
                    try
                    {
                        outputStream.close();
                        outputStream = null;
                    }
                    catch (IOException ioe)
                    {
                        Log.logWarning("IconConverterEswt: Closing " +
                                       aOutputIconFilename + " failed", ioe);
                    }
                }
            }
        }
        return result;
    }

    /**
     * Converts SVG icon to platform specific format.
     *
     * @param aInputIconFilename file name for input icon file
     * @param aOutputIconFilename file name for output icon file
     * @param aJarFilename jar file name if aInputIconFilename specifies
     *    file inside jar file,
     *    or null if aInputIconFilename specifies file from disk
     * @param aIconSuffix the correct suffix of the icon is returned through
     *    this parameter
     * @return true if the conversion succeeds
     */
    private static boolean convertSvgIcon(
        String aInputIconFilename, String aOutputIconFilename,
        String aJarFilename, StringBuffer aIconSuffix)
    {
        boolean result = false;
        JarFile jarFile = null;
        InputStream inputStream = null;
        OutputStream outputStream = null;
        try
        {
            if (aJarFilename == null)
            {
                inputStream = FileUtils.getInputStream(aInputIconFilename);
            }
            else
            {
                jarFile = new JarFile(aJarFilename);
                inputStream = jarFile.getInputStream(
                    new JarEntry(FileUtils.trimJarEntry(aInputIconFilename)));
            }
            outputStream = FileUtils.getOutputStream(aOutputIconFilename);
            // Copy the image data from InputStream to OutputStream.
            byte[] buf = new byte[16384];
            int i = 0;
            while ((i = inputStream.read(buf)) != -1)
            {
                outputStream.write(buf, 0, i);
            }
            aIconSuffix.append(".svg");
            result = true;
        }
        catch (IOException ioe)
        {
            Log.logWarning("IconConverterEswt: Saving SVG icon " +
                           aInputIconFilename + " from " +
                           aJarFilename + " to " + aOutputIconFilename +
                           " failed", ioe);
        }
        finally
        {
            if (outputStream != null)
            {
                try
                {
                    outputStream.close();
                    outputStream = null;
                }
                catch (IOException ioe)
                {
                    Log.logWarning("IconConverterEswt: Closing " +
                                   aOutputIconFilename + " failed", ioe);
                }
            }
            if (inputStream != null)
            {
                try
                {
                    inputStream.close();
                    inputStream = null;
                }
                catch (IOException ioe)
                {
                    Log.logWarning("IconConverterEswt: Closing " +
                                   aInputIconFilename + " failed", ioe);
                }
            }
            if (jarFile != null)
            {
                try
                {
                    jarFile.close();
                    jarFile = null;
                }
                catch (IOException ioe)
                {
                    Log.logWarning("IconConverterEswt: Closing " +
                                   aJarFilename + " failed", ioe);
                }
            }
        }
        return result;
    }

    /**
     * Returns true if given icon file is in SVG format.
     */
    private static boolean isSvgIcon(String aIconFilename, String aJarFilename)
    {
        boolean result = false;
        // Simple file type detection from filename extension.
        //if (aIconFilename.toLowerCase().endsWith(".svg"))
        //{
        //    result = true;
        //}
        //return result;

        // Detect SVG files by checking if file begins with "<?xml".
        JarFile jarFile = null;
        InputStream inputStream = null;
        try
        {
            if (aJarFilename == null)
            {
                inputStream = FileUtils.getInputStream(aIconFilename);
            }
            else
            {
                jarFile = new JarFile(aJarFilename);
                inputStream = jarFile.getInputStream(
                    new JarEntry(FileUtils.trimJarEntry(aIconFilename)));
            }
            byte[] bytes = new byte[5];
            int readCount = inputStream.read(bytes);
            if (readCount == bytes.length)
            {
                result = true;
                byte[] xmlDecl = { '<', '?', 'x', 'm', 'l' };
                for (int i = 0; i < readCount; i++)
                {
                    if (bytes[i] != xmlDecl[i])
                    {
                        result = false;
                        break;
                    }
                }
            }
        }
        catch (IOException ioe)
        {
            Log.logWarning("IconConverterEswt.isSvgIcon: Checking file type of " +
                           aIconFilename + " from " + aJarFilename +
                           " failed", ioe);
        }
        finally
        {
            if (inputStream != null)
            {
                try
                {
                    inputStream.close();
                    inputStream = null;
                }
                catch (IOException ioe)
                {
                    Log.logWarning("IconConverterEswt.isSvgIcon: Closing " +
                                   aIconFilename + " failed", ioe);
                }
            }
            if (jarFile != null)
            {
                try
                {
                    jarFile.close();
                    jarFile = null;
                }
                catch (IOException ioe)
                {
                    Log.logWarning("IconConverterEswt.isSvgIcon: Closing " +
                                   aJarFilename + " failed", ioe);
                }
            }
        }
        return result;
    }

    /**
     * Returns true if given icon file is in supported bitmap format.
     */
    private static boolean isSupportedBitmapIcon(
        String aIconFilename, String aJarFilename)
    {
        boolean result = true;
        for (int i = 0; i < iUnsupportedBitmapFormats.length; i++)
        {
            if (aIconFilename.toLowerCase().endsWith(
                    iUnsupportedBitmapFormats[i]))
            {
                result = false;
                break;
            }
        }
        return result;
    }
}
