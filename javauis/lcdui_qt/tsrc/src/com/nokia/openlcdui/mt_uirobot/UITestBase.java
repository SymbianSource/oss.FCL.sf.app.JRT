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
package com.nokia.openlcdui.mt_uirobot;

import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Displayable;

import org.eclipse.swt.graphics.Point;

import com.nokia.mj.impl.uitestutils.Key;
import com.nokia.mj.impl.uitestutils.UITestUtils;
import com.nokia.openlcdui.mt.SWTTestCase;

public class UITestBase extends SWTTestCase implements CommandListener {

    public final static int KEYEVENT_DELAY = 100;
    public final static int CHANGE_DISPLAYABLE_DELAY = 500;

    private Command lastDispCommand;
    private Displayable lastDisplayable;

    protected UITestUtils uiTestUtils;
    protected Display display;

    public UITestBase() {
    	this("");
    }
    
    public UITestBase(String name) {
        super(name);
        uiTestUtils = new UITestUtils();
    }

    protected void setUp() throws Exception {
        super.setUp();
        display = Display.getDisplay(getMIDlet());
        lastDisplayable = null;
        lastDispCommand = null;
    }

    protected void setCurrent(Displayable disp) {
        display.setCurrent(disp);
        block(CHANGE_DISPLAYABLE_DELAY);
    }

    public void runTest() throws Throwable {
    	super.runTest();
    }
    
    /**
     * Emulates key event by sending key pressed and key released events. Events
     * are also dispatched if there is a Display available.
     *
     * @param aKeyCode Keycode as defined in com.nokia.mj.impl.uitestutils.Key
     */
    public void key(int keycode) {
        key(keycode, KEYEVENT_DELAY);
    }

    /**
     * Emulates key event by sending key pressed and key released events. Events
     * are also dispatched if there is a Display available.
     *
     * @param keyCode Keycode as defined in com.nokia.mj.impl.uitestutils.Key
     * @param delay custom delay
     */
    public void key(int keycode, int delay) {
        Key key = new Key(keycode);
        uiTestUtils.triggerKeyPressEvent(key);
        flush();
        block(delay);
    }

    /**
     * Emulates key event by sending key pressed and key released events. Events
     * are also dispatched if there is a Display available.
     *
     * @param keyCode Keycode as defined in com.nokia.mj.impl.uitestutils.Key
     * @param modifiers Modifier as defined in
     *            com.nokia.mj.impl.uitestutils.Modifier
     */
    public void keyMod(int keycode, int modifiers) {
        Key key = new Key(keycode);
        key.modifiers = modifiers;
        uiTestUtils.triggerKeyPressEvent(key);
        flush();
        block(KEYEVENT_DELAY);
    }

    /**
     * Emulates pointer event by sending pressed and released events. Events are
     * also dispatched if there is a Display available.
     *
     * @param x X-coordinate relative to the screen.
     * @param y Y-coordinate relative to the screen.
     */
    public void pointerTap(Point p) {
        uiTestUtils.triggerPointerDownEvent(p.x, p.y);
        uiTestUtils.triggerPointerUpEvent(p.x, p.y);
        flush();
    }

    /**
     * Emulates pointer down event. Events are also dispatched if there is a
     * Display available.
     *
     * @param x X-coordinate relative to the screen.
     * @param y Y-coordinate relative to the screen.
     */
    public void pointerDown(Point p) {
        uiTestUtils.triggerPointerDownEvent(p.x, p.y);
        flush();
    }

    /**
     * Emulates pointer up event. Events are also dispatched if there is a
     * Display available.
     *
     * @param x X-coordinate relative to the screen.
     * @param y Y-coordinate relative to the screen.
     */
    public void pointerUp(Point p) {
        uiTestUtils.triggerPointerUpEvent(p.x, p.y);
        flush();
    }

    /**
     * Emulates pointer move event. Events are also dispatched if there is a
     * Display available.
     *
     * @param x X-coordinate relative to the screen.
     * @param y Y-coordinate relative to the screen.
     */
    public void pointerMove(Point p) {
        uiTestUtils.triggerPointerDragEvent(p.x, p.y);
        flush();
    }

    public void switchResolution() {
    	fail("switchResolution not implemented");
    }
    
    public int getNumberOfResolutions() {
    	fail("getNumberOfResolutions not implemented");
    	return 0;
    }
    
    /**
     * Assert the command listener values.
     *
     * @param expCmd expected Command
     * @param expDisp expected Displayable
     */
    protected void assertCmdListener(String msg, Command expCmd, Displayable disp) {
        block(100);
        boolean expNull = (expCmd == null && disp == null);
        boolean lastNull = (lastDispCommand == null && lastDisplayable == null);
        if (lastNull && !expNull) {
            fail(msg + " - Listener should have been called with "
                    + expCmd + " on " + disp);
        }
        else if (!lastNull && expNull) {
            fail(msg + " - Listener should not be called with "
                    + lastDispCommand + " on " + lastDisplayable);
        }
        else {
            assertEquals(msg + " - Listener displayable mismatch ", disp, lastDisplayable);
            assertEquals(msg + " - Listener command mismatch ", expCmd, lastDispCommand);
        }
        lastDisplayable = null;
        lastDispCommand = null;
    }

    public void commandAction(Command cmd, Displayable disp) {
        lastDisplayable = disp;
        lastDispCommand = cmd;
    }

}
