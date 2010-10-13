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

package org.eclipse.swt.internal.extension;

import org.eclipse.swt.graphics.Device;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.internal.symbian.OS;

public final class ImageUtil {
    
    public static final int THEME_IMAGE_SECURITY_TRUSTED = 0;
    public static final int THEME_IMAGE_SECURITY_UNTRUSTED = 1;

    public static Image createImageWithoutSecurityCheck(Device device,
            String filename) {
        return new Image(device, filename);
    }

    public static Image createImageFromTheme(Device device, int id) {
        return Image.internal_new(device, OS.Image_NewFromTheme(device.handle, id));
    }
    
    public static Image scaleImage(Device device, Image srcImage,
            Point destSize, boolean keepAspectRatio) {
        return Image.internal_new(device, OS.Image_Scale(device.handle,
                srcImage.handle, destSize.x, destSize.y, keepAspectRatio));
    }
}
