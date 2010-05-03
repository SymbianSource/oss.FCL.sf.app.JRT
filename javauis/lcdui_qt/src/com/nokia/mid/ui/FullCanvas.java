/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
package com.nokia.mid.ui;

import javax.microedition.lcdui.Canvas;

/**
 * FullCanvas provides totally full screen painting area.
 * <p>
 * Commands can't be added to FullCanvas. Pressing the keys normally tied to
 * Commands (e.g. softbuttons or CBA keys) will result direct keystroke events
 * (see documentation on key events on javax.microedition.Canvas). FullCanvas
 * defines new constants for these key codes.
 * <p>
 * Callback methods of FullCanvas have same semantics as in standard Canvas.
 * However, the DirectGraphics will be passed to the paint method of
 * com.nokia.mid.ui.FullCanvas.
 * <p>
 * Generally in FullCanvas all key presses are reported as normal key press
 * events through keyPressed(), keyRepeated() and keyReleased() methods. There
 * are some additional constant key code definitions in FullCanvas. All the
 * normal key codes reported in Canvas are also reported. The keys tied with
 * Command area do not report any key events if the are is visible.
 */
public abstract class FullCanvas extends Canvas {

    private static final String FULLCANVAS_COMMAND_EXCEPTION =
        "Commands are not possible in FullCanvas.";

    /**
     * Constant for <code>Left Soft Key</code> or
     * <code>First command button</code> key.
     */
    public static final int KEY_SOFTKEY1 = -6;

    /**
     * Constant for <code>Right Soft Key</code> or
     * <code>Last command button</code> key.
     */
    public static final int KEY_SOFTKEY2 = -7;

    /**
     * Constant for <code>Send</code> key.
     */
    public static final int KEY_SEND = -10;

    /**
     * Constant for <code>End</code> key.
     */
    public static final int KEY_END = -11;

    /**
     * Constant for <code>Middle Soft Key</code> or
     * <code>Second command button</code> key.
     */
    public static final int KEY_SOFTKEY3 = -5;

    /**
     * Constant for <code>Up</code>" arrow key.
     */
    public static final int KEY_UP_ARROW = -1;

    /**
     * Constant for <code>Down</code> arrow key.
     */
    public static final int KEY_DOWN_ARROW = -2;

    /**
     * Constant for <code>Left</code> arrow key.
     */
    public static final int KEY_LEFT_ARROW = -3;

    /**
     * Constant for <code>Right</code> arrow key.
     */
    public static final int KEY_RIGHT_ARROW = -4;

    /**
     * Constructor.
     *
     * @deprecated Deprecated. As of Nokia UI API 1.1. in devices with MIDP 2.0
     *             or higher, replaced by
     *             javax.microedition.lcdui.Canvas.setFullScreenMode(boolean).
     */
    public FullCanvas() {
        super();
        super.setFullScreenMode(true);
    }

    /* (non-Javadoc)
     * @see javax.microedition.lcdui.Canvas#setFullScreenMode(boolean)
     */
    public void setFullScreenMode(boolean fullScreen) {
        // ignore
    }

    /**
     * Commands are not possible in FullCanvas.
     *
     * @deprecated Deprecated. As of Nokia UI API 1.1. in devices with MIDP 2.0
     *             or higher, replaced by
     *             javax.microedition.lcdui.Canvas.addCommand(Command).
     * @throws IllegalStateException
     */
    public void addCommand(javax.microedition.lcdui.Command cmd) {
        throw new IllegalStateException(FULLCANVAS_COMMAND_EXCEPTION);
    }

    /**
     * Commands are not possible in FullCanvas.
     *
     * @deprecated Deprecated. As of Nokia UI API 1.1. in devices with MIDP 2.0
     *             or higher, replaced by
     *             javax.microedition.lcdui.Canvas.addCommand(Command).
     * @throws IllegalStateException
     */
    public void setCommandListener(
            javax.microedition.lcdui.CommandListener listener) {
        throw new IllegalStateException(FULLCANVAS_COMMAND_EXCEPTION);
    }

}
