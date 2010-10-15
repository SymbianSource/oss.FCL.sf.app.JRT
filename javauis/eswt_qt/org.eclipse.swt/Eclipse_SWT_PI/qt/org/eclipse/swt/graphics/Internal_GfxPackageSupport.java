/*******************************************************************************
 * Copyright (c) 2009, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 * 
 * Contributors:
 *     Nokia Corporation - initial implementation 
 *******************************************************************************/

package org.eclipse.swt.graphics;

import java.io.InputStream;

import org.eclipse.swt.internal.qt.graphics.WindowSurface;

/**
 * <p>
 * <b>IMPORTANT:</b> This class is <em>not</em> part of the SWT public API. It
 * must never be accessed from application code.
 * </p>
 */
public final class Internal_GfxPackageSupport {
/*
 * From the class Image
 */
public static int getNullIconHandle() {
    return Image.getNullIconHandle();
}

public static Image new_Image(Device device, 
        org.eclipse.swt.internal.qt.graphics.Image cgImage) {
    return Image.qt_new(device, cgImage);
}

public static int getIconHandle(Image i) {
    return i.getIconHandle();
}

public static org.eclipse.swt.internal.qt.graphics.Image getImage(Image i) {
    return i.getImage();
}

public static int getImageHandle(Image i) {
    return i.getImageHandle();
}

public static Image createImageWithoutSecurityCheck(Device device,
        String filename) {
    return Image.createImageWithoutSecurityCheck(device, filename);
}

public static Point getImageSize(Device device, String filename) {
    return Image.getImageSize(device, filename);
}

public static Point getImageSize(InputStream stream) {
    return Image.getImageSize(stream);
}

/*
 * From the class Device
 */
public static boolean internal(Device d) {
	return d.internal;
}

public static boolean initialized() {
	return Device.initialized;
}

public static Color newColor(Device device, int handle) {
    return Color.qt_new(device, handle);
}

public static Font newFont(Device device, int handle) {
    return Font.qt_new(device, handle);
}

/* 
 * From class GC
 */

public static Rectangle startExternalRendering(GC gc) {
	return gc.startExternalRendering();
}

public static void endExternalRendering(GC gc) {
	gc.endExternalRendering();
}

public static WindowSurface getWindowSurface(GC gc) {
	return gc.getWindowSurface();
}

}
