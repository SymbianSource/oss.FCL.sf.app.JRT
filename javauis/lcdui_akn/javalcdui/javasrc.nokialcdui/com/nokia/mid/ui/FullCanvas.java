/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  FullCanvas provides totally full screen painting area.
*
*/


package com.nokia.mid.ui;

import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;

/**
 * FullCanvas provides totally full screen painting area.
 * <p>
 * Implementation need to draw any status indicators overlaid on the FullCanvas.
 * Implementations should draw these indicators on top left corner, but there
 * may be reasons to deviate from this rule. Applications must take care that
 * the display become cluttered if overlaid indicators appear.
 * <p>
 * Commands can't be added to FullCanvas. Pressing the keys normally tied to
 * Commands (e.g. softbuttons or CBA keys) will result direct keystroke events
 * (see documentation on key events on javax.microedition.Canvas). FullCanvas
 * defines new constants for these key codes.
 * <p>
 * Callback methods of FullCanvas have same semantics as in standard Canvas.
 * However, the FullGraphics will be passed to the paint method of
 * com.nokia.mid.ui.FullCanvas.
 * <p>
 * Generally in FullCanvas all key presses are reported as normal key press
 * events through keyPressed(), keyRepeated() and keyReleased() methods. There are
 * some additional constant key code definitions in FullCanvas. All the normal
 * key codes reported in Canvas are also reported. The keys tied with Command
 * area do not report any key events if the are is visible.
 *
 * @version 1.1
 * @since 1.0
 */
public abstract class FullCanvas extends Canvas
{

    /**
     * Key code constant for left softkey or first CBA key.
     * @since 1.0
     */
    public static final int KEY_SOFTKEY1 = -6;

    /**
     * Key code constant for right softkey or last CBA key.
     * @since 1.0
     */
    public static final int KEY_SOFTKEY2 = -7;

    /**
     * Key code constant for Send key.
     * @since 1.0
     */
    public static final int KEY_SEND = -10;

    /**
     * Key code constant for End key. Note: Pressing end may have some other
     * effect depending on the system, for example, device UI
     * may return to idle and may close down the application. The key event
     * nevertheless is sent to the application before this happens.
     * @since 1.0
     */
    public static final int KEY_END = -11;

    /**
     * Key code constant for select key, middle softkey or second
     * CBA key. It is not possible
     * to set a label for this key in all implementations.
     * This key is not in all devices.
     * @since 1.0
     */
    public static final int KEY_SOFTKEY3 = -5;

    /**
     * Key code constant for Up arrow key.
     * @since 1.0
     */
    public static final int KEY_UP_ARROW = -1;

    /**
     * Key code constant for Down arrow key.
     * @since 1.0
     */
    public static final int KEY_DOWN_ARROW = -2;

    /**
     * Key code constant for Left arrow key.
     * This key is not in all devices.
     * @since 1.0
     */
    public static final int KEY_LEFT_ARROW = -3;

    /**
     * Key code constant for Right arrow key.
     * This key is not in all devices.
     * @since 1.0
     */
    public static final int KEY_RIGHT_ARROW = -4;

    private static int sFullWidth = 0;
    private static int sFullHeight = 0;

    /**
     * Constructs a new FullCanvas object. Applications can't call this
     * constructor directly, they need to subclass FullCanvas.
     */
    protected FullCanvas()
    {
        super();
        super.setFullScreenMode(true);
    }

    /**
     *  Throws IllegalStateException. Commands are not possible in FullCanvas.
     *  @throws java.lang.IllegalStateException thrown always
     */
    public void addCommand(Command aCmd) throws IllegalStateException
    {
        throw(new IllegalStateException("This method is not supported in com.nokia.mid.ui.FullCanvas"));
    }

    /**
     *  Throws IllegalStateException. Commands are not possible in FullCanvas.
     *  @throws java.lang.IllegalStateException thrown always
     */
    public void setCommandListener(CommandListener aL) throws IllegalStateException
    {
        throw(new IllegalStateException("This method is not supported in com.nokia.mid.ui.FullCanvas"));
    }

    /**
     * Throws IllegalArgumentException. Midlet cannot resize the nokia FullCanvas.
     *  @throws java.lang.IllegalArgumentException :  Midlet cannot resize the nokia FullCanvas.
     *
     */
    public void setFullScreenMode(boolean aMode)
    {
        if (!aMode)
        { // cannot resize the Nokia Full Canvas
            throw(new IllegalArgumentException("Not supported in com.nokia.mid.ui.FullCanvas"));
        }
    }
    /*
        public int getWidth()
            {
            if( sFullWidth == 0 )
                {
                sFullWidth = _getFullWidth( );
                }
            return sFullWidth;
            }


        public int getHeight()
            {
            if( sFullHeight == 0 )
                {
                sFullHeight = _getFullHeight( );
                }
            return sFullHeight;
            }
    */

}
