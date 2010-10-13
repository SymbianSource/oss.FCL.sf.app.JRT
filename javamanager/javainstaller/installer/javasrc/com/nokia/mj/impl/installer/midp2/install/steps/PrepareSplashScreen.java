/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.installer.midp2.install.steps;

import com.nokia.mj.impl.installer.exetable.ExeBall;
import com.nokia.mj.impl.installer.exetable.ExeStep;
import com.nokia.mj.impl.installer.integrityservice.IntegrityService;
import com.nokia.mj.impl.installer.utils.FileUtils;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.installer.utils.SysUtil;
import com.nokia.mj.impl.utils.Tokenizer;

import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.util.jar.JarFile;
import java.util.jar.JarEntry;

import org.eclipse.swt.graphics.ImageData;
import org.eclipse.swt.graphics.ImageLoader;
import org.eclipse.swt.graphics.Point;

/**
 * Installation step PrepareSplashScreen prepares splash
 * screen images from Nokia-MIDlet-Splash-Screen-Image attribute.
 */
public class PrepareSplashScreen extends ExeStep
{
    public void execute(ExeBall aBall)
    {
        InstallBall ball = (InstallBall)aBall;

        String userImageRoot = ball.iSuite.getRootDir() +
                               "startupimages" + FileUtils.pathSeparator();
        if (!deleteSplashScreenDir(ball, userImageRoot))
        {
            // Deleting splash screen directory failed,
            // do not process splash screen images.
            return;
        }

        String autoImageRoot = ball.iSuite.getRootDir() +
                               "autostartupimages" + FileUtils.pathSeparator();
        if (!deleteSplashScreenDir(ball, autoImageRoot))
        {
            // Deleting screenshots directory failed,
            // do not process splash screen images.
            return;
        }

        if (ball.iJarFilename == null)
        {
            // No jar file available, do not proceed further.
            return;
        }

        String attrName = "Nokia-MIDlet-Splash-Screen-Image";
        String attrValue = ball.getAttributeValue(attrName);
        if (attrValue != null && attrValue.length() > 0)
        {
            if (attrValue.equalsIgnoreCase("suppress"))
            {
                // Splash screen is suppressed, do not
                // process splash screen images.
                Log.log("Splash screen suppressed");
                // Create splash screen directory to indicate
                // that splash screen has been suppressed.
                createSplashScreenDir(ball, userImageRoot);
                return;
            }

            // Get the screen size for portrait and landscape.
            int screenWidth = SysUtil.getScreenWidth();
            int screenHeight = SysUtil.getScreenHeight();
            Point portraitScreenSize = new Point(screenWidth, screenHeight);
            Point landscapeScreenSize = new Point(screenHeight, screenWidth);
            if (screenWidth > screenHeight)
            {
                portraitScreenSize.x = screenHeight;
                portraitScreenSize.y = screenWidth;
                landscapeScreenSize.x = screenWidth;
                landscapeScreenSize.y = screenHeight;
            }
            Log.log("Portrait screen size: " + portraitScreenSize);
            Log.log("Landscape screen size: " + landscapeScreenSize);
            // Get the sizes of all images specified in the attribute value.
            ImageData[] currentImage = null;
            String[] tokens = Tokenizer.split(attrValue, ",");
            Point[] imageSizes = new Point[tokens.length];
            for (int i = 0; i < tokens.length; i++)
            {
                tokens[i] = tokens[i].trim();
                currentImage = loadImage(tokens[i], ball.iJarFilename);
                if (currentImage == null)
                {
                    // Image loading failed, proceed to the next image.
                    imageSizes[i] = new Point(0, 0);
                    continue;
                }
                imageSizes[i] = new Point(
                    currentImage[0].width, currentImage[0].height);
            }
            // Choose the images which best fill the portrait and
            // landscape screens.
            String portraitImageName = null;
            String landscapeImageName = null;
            int imageIndex = selectImage(portraitScreenSize, imageSizes);
            if (imageIndex != -1)
            {
                portraitImageName = tokens[imageIndex];
                Log.log("Selected portrait image " + imageIndex +
                        ": " + portraitImageName);
            }
            imageIndex = selectImage(landscapeScreenSize, imageSizes);
            if (imageIndex != -1)
            {
                landscapeImageName = tokens[imageIndex];
                Log.log("Selected landscape image " + imageIndex +
                        ": " + landscapeImageName);
            }

            // Save splash screen image to the disk.
            if (portraitImageName != null &&
                    landscapeImageName != null &&
                    !portraitImageName.equals(landscapeImageName))
            {
                // Save portrait and landscape splash screen images.
                createSplashScreenDir(ball, userImageRoot);
                saveImage(portraitImageName, ball.iJarFilename,
                          userImageRoot + "startupscreen_portrait",
                          ball.iIntegrityService);
                saveImage(landscapeImageName, ball.iJarFilename,
                          userImageRoot + "startupscreen_landscape",
                          ball.iIntegrityService);
            }
            else if (portraitImageName != null)
            {
                // Save one splash screen image.
                createSplashScreenDir(ball, userImageRoot);
                saveImage(portraitImageName, ball.iJarFilename,
                          userImageRoot + "startupscreen",
                          ball.iIntegrityService);
            }
            else if (landscapeImageName != null)
            {
                // Save one splash screen image.
                createSplashScreenDir(ball, userImageRoot);
                saveImage(landscapeImageName, ball.iJarFilename,
                          userImageRoot + "startupscreen",
                          ball.iIntegrityService);
            }
            else
            {
                Log.logWarning("No splash screen image selected.");
            }
        }
        else
        {
            createSplashScreenDir(ball, autoImageRoot);
        }
    }

    public void cancel(ExeBall aBall)
    {
        // nop
    }

    /**
     * Deletes splash screen image directory if it exists.
     *
     * @return true if directory does not exist after this method returns,
     * false if deleting existing directory fails
     */
    private static boolean deleteSplashScreenDir(
        InstallBall aBall, String aImageRoot)
    {
        boolean result = true;
        if (aBall.iOldSuite != null)
        {
            // Note that PrepareSplashScreen step is executed after
            // CopyAppFiles step, which means that old image root has
            // already been moved under new application suite root.
            if (FileUtils.exists(aImageRoot))
            {
                // In case of update, remove old splash screen images.
                result = aBall.iIntegrityService.delete(aImageRoot);
                if (!result)
                {
                    Log.logWarning("Removing " + aImageRoot +
                                   " failed, aborting splash screen" +
                                   " image preparation.");
                    return result;
                }
            }
        }
        return result;
    }

    /**
     * Creates splash screen image directory.
     *
     * @return true if directory exists after this method returns,
     * false if creating directory fails
     */
    private static boolean createSplashScreenDir(
        InstallBall aBall, String aImageRoot)
    {
        boolean result = true;
        if (!FileUtils.exists(aImageRoot))
        {
            result = aBall.iIntegrityService.createDir(aImageRoot);
            if (!result)
            {
                Log.logWarning("Creating " + aImageRoot +
                               " failed, aborting splash screen" +
                               " image preparation.");
                return result;
            }
        }
        return result;
    }

    /**
     * Loads image from specified resource from given jar file.
     *
     * @param aResource resource file name
     * @param aJar jar file name
     * @return loaded image, or null if image loading fails
     */
    private static ImageData[] loadImage(String aResource, String aJar)
    {
        ImageData[] result = null;
        JarFile jarFile = null;
        InputStream is = null;
        try
        {
            // Open jar file and input stream.
            jarFile = new JarFile(aJar);
            is = jarFile.getInputStream
                 (new JarEntry(FileUtils.trimJarEntry(aResource)));
            if (is != null)
            {
                result = (new ImageLoader()).load(is);
                Log.log("Loaded image " + aResource + " from " + aJar);
            }
            else
            {
                Log.logWarning("Image " + aResource + " not found from " + aJar);
            }
        }
        catch (Throwable t)
        {
            Log.logWarning("Loading image " + aResource + " from " +
                           aJar + " failed", t);
        }
        finally
        {
            // Close streams and jar file.
            if (is != null)
            {
                try
                {
                    is.close();
                    is = null;
                }
                catch (IOException ioe)
                {
                    Log.logWarning("Closing InputStream failed", ioe);
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
                    Log.logWarning("Closing " + aJar + " failed", ioe);
                }
            }
        }
        return result;
    }

    /**
     * Save image from specified resource from given jar file to disk.
     *
     * @param aResource resource file name
     * @param aJar jar file name
     * @param aFilename filename for the image (without extension)
     * @param aIs IntegrityService instance to be used in file creation
     */
    private static void saveImage(
        String aResource, String aJar, String aFilename, IntegrityService aIs)
    {
        JarFile jarFile = null;
        InputStream is = null;
        OutputStream os = null;
        String imageFilename = aFilename + ".img";
        try
        {
            // Open jar file and input and output streams.
            jarFile = new JarFile(aJar);
            is = jarFile.getInputStream
                 (new JarEntry(FileUtils.trimJarEntry(aResource)));
            os = FileUtils.getOutputStream(imageFilename);
            // Copy the image data from InputStream to OutputStream.
            byte[] buf = new byte[16384];
            int i = 0;
            while ((i = is.read(buf)) != -1)
            {
                os.write(buf, 0, i);
            }
            // Record file creation to integrityService.
            aIs.create(imageFilename);
            Log.log("Saved image to " + imageFilename);
        }
        catch (Throwable t)
        {
            Log.logWarning("Saving image " + aResource + " from " +
                           aJar + " to " + imageFilename + " failed", t);
        }
        finally
        {
            // Close streams and jar file.
            if (os != null)
            {
                try
                {
                    os.close();
                    os = null;
                }
                catch (IOException ioe)
                {
                    Log.logWarning("Closing OutputStream failed", ioe);
                }
            }
            if (is != null)
            {
                try
                {
                    is.close();
                    is = null;
                }
                catch (IOException ioe)
                {
                    Log.logWarning("Closing InputStream failed", ioe);
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
                    Log.logWarning("Closing " + aJar + " failed", ioe);
                }
            }
        }
    }

    /**
     * Selects image which best fills the given screen when image
     * is scaled down so that it fits to the screen.
     *
     * @param aScreenSize screen size
     * @param aImageSizes array of image sizes
     * @return index to aImageSizes array for the selected image, or -1
     * if selection could not be made
     */
    private static int selectImage(Point aScreenSize, Point[] aImageSizes)
    {
        // Amount of fill for the image which best fills
        // the screen when scaled down to fit the screen.
        int biggestFill = 0;
        // Index of the image which best fills the screen.
        int biggestFillIndex = -1;
        for (int i = 0; i < aImageSizes.length; i++)
        {
            Point imageSize = aImageSizes[i];
            if (imageSize.x > aScreenSize.x ||
                    imageSize.y > aScreenSize.y)
            {
                imageSize = downscaleImageSize(aScreenSize, imageSize);
            }
            int imageFill = imageSize.x * imageSize.y;
            if (imageFill > biggestFill)
            {
                biggestFill = imageFill;
                biggestFillIndex = i;
            }
            Log.log("Image " + i + " fill amount " + imageFill);
        }
        return biggestFillIndex;
    }

    /**
     * Downscales the given aImageSize so that it fits to aScreenSize.
     */
    private static Point downscaleImageSize(Point aScreenSize, Point aImageSize)
    {
        Point result = new Point(aImageSize.x, aImageSize.y);
        if (result.x > aScreenSize.x)
        {
            // Downscale the image width.
            result.y = result.y * aScreenSize.x / result.x;
            result.x = aScreenSize.x;
        }
        if (result.y > aScreenSize.y)
        {
            // Downscale the image height.
            result.x = result.x * aScreenSize.y / result.y;
            result.y = aScreenSize.y;
        }
        Log.log("Image size " + aImageSize + " scaled down to " +
                result + " for screen size " + aScreenSize);
        return result;
    }
}
