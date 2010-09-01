/*
* Copyright (c) 1999 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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


package javax.microedition.lcdui;

import com.nokia.mj.impl.rt.legacy.NativeError;

//
// Provides information about device capabilities
//
final class Device
{
    Toolkit iToolkit;

    static final int CAPS_POINTER_EVENTS = 1;
    static final int CAPS_POINTER_MOTION_EVENTS = 2;
    static final int CAPS_KEY_REPEAT_EVENTS = 4;
    static final int CAPS_COLOR = 8;
    static final int CAPS_DOUBLE_BUFFERED = 16;

    static final int ATTRIB_CAPABILITIES = 0;
    static final int ATTRIB_NUM_COLORS   = 1;
    static final int ATTRIB_NUM_ALPHAS   = 2;
    static final int ATTRIB_SCREEN_WIDTH = 3;
    static final int ATTRIB_SCREEN_HEIGHT = 4;
    static final int ATTRIB_COUNT = 5;

    int iScreenWidth;     // full screen width
    int iScreenHeight;    // full screen height
    final int iNumColors;
    final int iNumAlphas;
    final boolean iHasPointerEvents;
    final boolean iHasPointerMotionEvents;
    final boolean iHasRepeatEvents;
    final boolean iIsDoubleBuffered;
    final boolean iIsColor;

    Device(Toolkit aToolkit)
    {
        iToolkit = aToolkit;

        final int[] attribs = new int[ATTRIB_COUNT];

        NativeError.check(_getAttribs(aToolkit.getHandle(), attribs));

        final int caps = attribs[ATTRIB_CAPABILITIES];
        iHasPointerEvents       = (caps & CAPS_POINTER_EVENTS) == CAPS_POINTER_EVENTS;
        iHasPointerMotionEvents = (caps & CAPS_POINTER_MOTION_EVENTS) == CAPS_POINTER_MOTION_EVENTS;
        iHasRepeatEvents        = (caps & CAPS_KEY_REPEAT_EVENTS) == CAPS_KEY_REPEAT_EVENTS;
        iIsColor = (caps & CAPS_COLOR) == CAPS_COLOR;
        iIsDoubleBuffered = (caps & CAPS_DOUBLE_BUFFERED) == CAPS_DOUBLE_BUFFERED;

        iNumColors = attribs[ATTRIB_NUM_COLORS];
        iNumAlphas = attribs[ATTRIB_NUM_ALPHAS];

        iScreenWidth  = attribs[ATTRIB_SCREEN_WIDTH];
        iScreenHeight = attribs[ATTRIB_SCREEN_HEIGHT];
    }

    final boolean hasPointerEvents()
    {
        return iHasPointerEvents;
    }

    final boolean hasPointerMotionEvents()
    {
        return iHasPointerMotionEvents;
    }

    final boolean hasRepeatEvents()
    {
        return iHasRepeatEvents;
    }

    final boolean isDoubleBuffered()
    {
        return iIsDoubleBuffered;
    }

    // Display requirements:

    final boolean isColor()
    {
        return iIsColor;
    }

    final int numColors()
    {
        return iNumColors;
    }

    final int numAlphas()
    {
        return iNumAlphas;
    }

    final int getScreenWidth()
    {
        return iScreenWidth;
    }

    final int getScreenHeight()
    {
        return iScreenHeight;
    }

    public final void getDeviceSizeWithoutKeypad()
    {
        final int[] size = new int[2];
        // This method is called during construction of Canvas,
        // then we need check variables to null.
        if (iToolkit != null)
        {
            Display display = iToolkit.getDisplay();
            if (display != null)
            {
                Displayable displayable = display.getCurrent();
                if (displayable != null)
                {
                    _getCanvasSizeWithoutKeypad(iToolkit.getHandle(),
                                                displayable.getContainerHandle(),
                                                size);
                    iScreenWidth  = size[0];
                    iScreenHeight = size[1];
                }
            }
        }
    }

    private native int _getAttribs(int aToolkit, int[] aAttribs);

    private native void _getCanvasSizeWithoutKeypad(int aToolkit, int aDisplayable, int[] aSize);
}
