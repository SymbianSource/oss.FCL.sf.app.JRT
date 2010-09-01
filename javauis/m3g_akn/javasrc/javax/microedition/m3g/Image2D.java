/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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


package javax.microedition.m3g;

import com.nokia.mj.impl.rt.legacy.ToolkitInvoker;
import javax.microedition.lcdui.Image;

public class Image2D extends Object3D
{
    //------------------------------------------------------------------
    // Static data
    //------------------------------------------------------------------

    public static final int ALPHA               = 96;
    public static final int LUMINANCE           = 97;
    public static final int LUMINANCE_ALPHA     = 98;
    public static final int RGB                 = 99;
    public static final int RGBA                = 100;

    //------------------------------------------------------------------
    // Constructor(s)
    //------------------------------------------------------------------

    public Image2D(int format, Object image)
    {
        // If image is instance of lcdui.Image then checkAndCreate
        // builds the image and returns the handle to native image,
        // otherwise throws exception Done this way because class of
        // image cannot be checked befor calling super()
        super(Image2D.checkAndCreate(format, image));
    }

    public Image2D(int format, int width, int height, byte[] image)
    {
        super(createHandle(format, width, height, image));
    }

    public Image2D(int format,
                   int width, int height,
                   byte[] image,
                   byte[] palette)
    {
        super(createHandle(format, width, height, image, palette));
    }

    public Image2D(int format, int width, int height)
    {
        super(createHandle(format, width, height));
    }

    Image2D(int handle)
    {
        super(handle);
    }

    //------------------------------------------------------------------
    // Public methods
    //------------------------------------------------------------------

    public void set(int x, int y, int width, int height, byte[] image)
    {
        if (image == null)
        {
            throw new NullPointerException();
        }
        _set(handle, x, y, width, height, image);
    }

    public boolean isMutable()
    {
        return _isMutable(handle);
    }

    public int getFormat()
    {
        return _getFormat(handle);
    }

    public int getWidth()
    {
        return _getWidth(handle);
    }

    public int getHeight()
    {
        return _getHeight(handle);
    }

    //------------------------------------------------------------------
    // Private methods
    //------------------------------------------------------------------

    private static int checkAndCreate(int format, Object image)
    {
        if (image == null)
        {
            throw new NullPointerException();
        }
        if (!(image instanceof Image))
        {
            throw new IllegalArgumentException();
        }
        Platform.heuristicGC();
        ToolkitInvoker invoker = ToolkitInvoker.getToolkitInvoker();
        Platform.sync((Image) image);
        int handle = 0;
        try
        {
            handle = _ctorImage(Interface.bindEventSource(), Interface.getHandle(), format, invoker.imageGetHandle(image));
        }
        finally
        {
            Interface.releaseEventSource();
        }
        return handle;
    }

    private static int createHandle(int format, int width, int height, byte[] image)
    {
        Platform.heuristicGC();
        return _ctorSizePixels(Interface.getHandle(),
                               format,
                               width, height,
                               image);
    }

    private static int createHandle(int format,
                                    int width, int height,
                                    byte[] image,
                                    byte[] palette)
    {
        Platform.heuristicGC();
        return _ctorSizePixelsPalette(Interface.getHandle(),
                                      format,
                                      width, height,
                                      image, palette);
    }

    private static int createHandle(int format, int width, int height)
    {
        Platform.heuristicGC();
        return _ctorSize(Interface.getHandle(), format, width, height);
    }

    // Native methods
    private native static int _ctorImage(int eventSourceHandle,
                                         int hInterface,
                                         int format,
                                         int imageHandle);
    private native static int _ctorSizePixels(int hInterface,
            int format,
            int width, int height,
            byte[] image);
    private native static int _ctorSizePixelsPalette(int hInterface,
            int format,
            int width, int height,
            byte[] image,
            byte[] palette);
    private native static int _ctorSize(int hInterface,
                                        int format,
                                        int width, int height);

    private native static void _set(int handle, int x, int y, int width,
                                    int height, byte[] image);
    private native static boolean _isMutable(int handle);
    private native static int _getFormat(int handle);
    private native static int _getWidth(int handle);
    private native static int _getHeight(int handle);
}
