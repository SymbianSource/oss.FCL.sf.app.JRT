/*******************************************************************************
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 * 
 * Contributors:
 *     Nokia Corporation - initial implementation 
 *******************************************************************************/

package org.eclipse.swt.internal.extension;

import org.eclipse.swt.graphics.Device;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Internal_GfxPackageSupport;
import java.io.InputStream;

public final class ImageUtil{
    
    public static Image createImageWithoutSecurityCheck(Device device, String filename) {
        return Internal_GfxPackageSupport.createImageWithoutSecurityCheck(device, filename);
    }
    
    /**
     * @deprecated Use createImageFromTheme(Device device, String name) instead.
     * @param device
     * @param id
     * @return
     */
    public static Image createImageFromTheme(Device device, int id) {
        return null;
    }
    
    /**
     * Creates an eSWT Image from a theme icon.
     * Example:
     *      Image image = ImageUtil.createImageFromTheme(display, "qtg_fr_rocker_normal_c");
     * @param device
     * @param name
     * @return
     */
    public static Image createImageFromTheme(Device device, String name) {
        Image res = null;
        int iconHandle = org.eclipse.swt.internal.qt.OS.HbIcon_new(name);
        org.eclipse.swt.internal.qt.graphics.Image cgImage = 
            org.eclipse.swt.internal.qt.graphics.Image.createImageFromPixmap(
                    org.eclipse.swt.internal.qt.OS.HbIcon_pixmap(iconHandle));
        res = Internal_GfxPackageSupport.new_Image(device, cgImage);
        org.eclipse.swt.internal.qt.OS.HbIcon_delete(iconHandle);
        return res;
    }
    
    /**
     * Returns the bounds of an Image without creating an actual Image instance.
     * Useful if there's a need to check the bounds of large collection of images
     * to choose one that gets initialized.
     *
     * For bitmap files the function returns the size of the bitmap. For SVG
     * the default size of the image is returned.
     *
     * Note that contents of the file are loaded to memory even when using
     * this function. If you are going to load the image anyway, using this 
     * function is not efficient.
     *
     * @param device The device to use when loading
     * @param filename Filename from where to read the image data from. File is loaded from the classpath, not from the file system. 
     * @return Size of the image
     * @throws IOException if image cannot be loaded
     */
    public static Point getImageSize(Device device, String filename) {
        return Internal_GfxPackageSupport.getImageSize(device, filename);
    }
    
    /**
     * Returns the bounds of an Image without creating an actual Image instance.
     * Useful if there's a need to check the bounds of large collection of images
     * to choose one that gets initialized.
     *
     * For bitmap files the function returns the size of the bitmap. For SVG
     * the default size of the image is returned.
     *
     * Note that contents of the file are loaded to memory even when using
     * this function. If you are going to load the image anyway, using this 
     * function is not efficient.
     *
     * @param is The InputStream from where to read the image data from
     * @return Size of the image
     * @throws IOException if image cannot be loaded
     */
    public static Point getImageSize(InputStream stream) {
        return Internal_GfxPackageSupport.getImageSize(stream);
    }
}
