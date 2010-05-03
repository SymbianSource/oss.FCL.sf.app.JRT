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
package com.nokia.openlcdui.mt;

import java.util.Vector;

import junit.framework.TestCase;
//import junit.framework.Test;
//import junit.textui.TestRunner;

import javax.microedition.midlet.MIDlet;
import javax.microedition.midlet.MIDletStateChangeException;

import com.nokia.mj.impl.uitestutils.Key;
import com.nokia.mj.impl.uitestutils.UITestUtils;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;


public class SWTTestCase extends TestCase {

    public static final Point POINTER_LSK = new Point(10, 470);
    public static final Point POINTER_MSK = new Point(160, 470);
    public static final Point POINTER_RSK = new Point(310, 470);
    public static final Point POINTER_MENU_ITEM_0 = new Point(160, 415);
    public static final Point POINTER_MENU_ITEM_1 = new Point(160, 380);
    public static final Point POINTER_MENU_ITEM_2 = new Point(160, 345);
    public static final Point POINTER_MENU_ITEM_3 = new Point(160, 310);
    public static final Point POINTER_MENU_ITEM_4 = new Point(160, 275);
    public static final Point POINTER_MENU_ITEM_5 = new Point(160, 240);
    public static final Point POINTER_MESSAGE_BOX = new Point(160, 375);

    public class TestDisabledException extends RuntimeException {
    	private static final long serialVersionUID = 3005532374213580596L;
    	}
    
    static Vector disabledTests = new Vector();
    private UITestUtils uiTestUtils;
    
    private class TestMIDlet extends MIDlet {

		protected void destroyApp(boolean arg0) throws MIDletStateChangeException {
		}

		protected void pauseApp() {
		}

		protected void startApp() throws MIDletStateChangeException {
		}};
    
	private static TestMIDlet midlet;

	/**
     * Constructor.
     */
    public SWTTestCase() {
    	super();
    	uiTestUtils = new UITestUtils();
    	if (midlet == null) midlet = new TestMIDlet();
    }

    /**
     * Constructor.
     *
     * @param name
     * @param method
     */
    public SWTTestCase(String name) {
        super(name);
        uiTestUtils = new UITestUtils();
        if (midlet == null) midlet = new TestMIDlet();
    }

    /**
     * Returns a list of tests that have been executed but were explicitly disabled.
     * This cases show in the results as having passed but the test case was in
     * reality skipped.
     *
     * @return A vector of test case names of disabled tests
     */
    static public Vector getDisabledTests() {
        return disabledTests;
    }
    
    /**
     * Call this in the beginning of a test case to temporarily disable it. I.e.
     * test will be skipped and it will show as passed. There will be a warning
     * in the test results about having disabled some tests. The purpose of this is
     * to temporarily hide the known issues so that it's easier to spot the real
     * failures that are not being worked on yet.
     */
    protected void DISABLE_TEST() {
        throw new TestDisabledException();
    }
    
    /**
     * Block execution for the given time without blocking the UI Thread. This
     * method should be used instead of the Thread.sleep, which will block the
     * SWT UI Thread.
     *
     * @param miliseconds
     */
    protected void block(int milliseconds) {
        try {
            Thread.sleep(milliseconds);
        }
        catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    /**
     * Convenience method for dispatching all events for the current UI thread.
     */
    protected void flush() {
        Display display = Display.getCurrent();
        if (display != null) {
            while (display.readAndDispatch()) {
            }
        }
    }

    protected void tearDown() throws Exception {
        super.tearDown();
        flush();

        // Make sure all control's are disposed so that they are not "leaked"
        // to the next test case.
        /*Display display = Display.getCurrent();
        if (display != null) {
            Shell[] shells = display.getShells();
            for (int i = 0; i < shells.length; ++i) {
                if (shells[i] != null && !shells[i].isDisposed()) {
                    shells[i].dispose();
                }
            }
        }*/

        System.gc();
    }

    /**
     * System out print.
     */
    public void log(String aLogString) {
        System.out.println(aLogString);
    }

    /**
     * MIDlet instance.
     */
    public MIDlet getMIDlet() {
    	return midlet;
    }

    /**
     * Spede instance.
     */
    /*public SpedeRunner spede() {
        return (SpedeRunner) TestRunner.getInstance();
    }*/

    /**
     * Emulate key event.
     */
    public void key(int aKeyCode) {
        //spede().triggerKeyPressEvent(new Key(aKeyCode));
        block(500);
    }

    /**
     * Emulate key event with delay after.
     */
    public void key(int aKeyCode, int aTimeout) {
        //spede().triggerKeyPressEvent(new Key(aKeyCode));
        if (aTimeout > 0) {
            block(aTimeout);
        }
    }

    /**
     * Takes a reference screenshot and writes results to a file.
     */
    public boolean takeScreenshot(String screenShotName) {
    	//return spede().takeScreenshot(screenShotName);
    	return false;
    }

    /**
     * Add comment to test results log.
     */
    public void print(String s) {
        uiTestUtils.log(s);
    }

    /**
     * Add exception to test results log.
     */
    public void print(Throwable t) {
    	uiTestUtils.log(t.toString());
    }

    /**
     * Take a screenshot. This method should preferably be used by all the test
     * cases to take screenshots so that waiting for the screen to paint can be
     * handled efficiently.
     */
    public boolean screen(String title) {
        return screen(title, new Rectangle(0, 0, 0, 0));
    }

    /**
     * Take a screenshot. This method should preferably be used by all the test
     * cases to take screenshots so that waiting for the screen to paint can be
     * handled efficiently. It compares given area of screenshot to
     * corresponding area in reference image. If given rectangle area is empty,
     * the whole screen is compared.
     */
    public boolean screen(String title, Rectangle compareArea) {
        final int DEFAULT_MAXWAITTIME = 1000;
        return screen(title, DEFAULT_MAXWAITTIME, compareArea);
    }

    /**
     * Take a screenshot with specified wait time. If the screen takes longer
     * time to stabilise, for example in a complex layout test case, then this
     * method should be used and a longer wait time given.
     */
    public boolean screen(String title, int maxWaitTime) {
        return screen(title, maxWaitTime, new Rectangle(0, 0, 0, 0));
    }

    /**
     * Take a screenshot with specified wait time. If the screen takes longer
     * time to stabilise, for example in a complex layout test case, then this
     * method should be used and a longer wait time given. It compares given
     * area of screenshot to corresponding area in reference image. If given
     * rectangle area is empty, the whole screen is compared.
     */
    public boolean screen(String title, int maxWaitTime, Rectangle compareArea) {
        final int WAIT_INITIAL = 100;
        final int WAIT_RETRY = 100;

        block(WAIT_INITIAL);
        int waitSum = WAIT_INITIAL;

        boolean screenMatchesReferenceImage = false;
        boolean finalAttempt = false;
        while (waitSum < maxWaitTime && !screenMatchesReferenceImage) {
            if (!(waitSum + WAIT_RETRY < maxWaitTime))
                finalAttempt = true;
            screenMatchesReferenceImage = doTakeScreenShot(title, finalAttempt,
                    compareArea);

            if (!screenMatchesReferenceImage) {
                block(WAIT_RETRY);
                waitSum += WAIT_RETRY;
            }
        }

        return screenMatchesReferenceImage;
    }

    /**
     * This is private because it must not be used by the test cases directly.
     * Paint events must be dispatched before taking the screenshot
     * readAndDispatch or the screen won't be up to date.
     *
     * @param finalAttempt If true then results will be written even on failure.
     *            If false then results will be written only after successful
     *            comparison to the reference image.
     */
    protected boolean doTakeScreenShot(String title, boolean finalAttempt) {
        return doTakeScreenShot(title, finalAttempt, new Rectangle(0, 0, 0, 0));
    }

    /**
     * This is private because it must not be used by the test cases directly.
     * Paint events must be dispatched before taking the screenshot
     * readAndDispatch or the screen won't be up to date. It compares given area
     * of screenshot to corresponding area in reference image. If given
     * rectangle area is empty, the whole screen is compared.
     *
     * @param finalAttempt If true then results will be written even on failure.
     *            If false then results will be written only after successful
     *            comparison to the reference image.
     */
    protected boolean doTakeScreenShot(String title, boolean finalAttempt,
            Rectangle compareArea) {
        String actualTitle = getName();
        if (title != "") {
            actualTitle += "_" + title;
        }
        //return spede().takeScreenshot(actualTitle, finalAttempt, compareArea);
        return false;
    }

    /**
     * Take screenshot.
     */
    public boolean screen() {
        return screen("");
    }

    /**
     * Take screenshot.
     */
    public boolean screen(int maxWaitTime) {
        return screen("", maxWaitTime);
    }

    /**
     * Take screenshot.
     */
    public boolean screen(Rectangle compareArea) {
        return screen("", compareArea);
    }

    /**
     * Take screenshot.
     */
    public void screenAssert() {
        boolean res = true;
        res = screen();
        assertTrue("Screenshot mismatch", res);
    }

    /**
     * Take screenshot.
     */
    public void screenAssert(String title) {
        boolean res = true;
        res = screen(title);
        assertTrue("Screenshot " + title + " mismatch", res);
    }


    /**
     * Emulate pointer event.
     */
    public void pointerTap(int x, int y) {
        uiTestUtils.triggerPointerDownEvent(x, y);
        uiTestUtils.triggerPointerUpEvent(x, y);
    }

    /**
     * Emulate pointer event.
     */
    public void pointerLongTap(int x, int y) {
        //spede().longTap(x, y);
        //block(500);
    }

    /**
     * Emulate pointer event.
     */
    public void pointerDown(int x, int y) {
        uiTestUtils.triggerPointerDownEvent(x, y);
    }

    /**
     * Emulate pointer event.
     */
    public void pointerUp(int x, int y) {
        uiTestUtils.triggerPointerUpEvent(x, y);
    }

    /**
     * Emulate pointer event.
     */
    public void pointerDrag(int x1, int y1, int x2, int y2) {
        //spede().pointerDrag(x1, y1, x2, y2);
        //block(500);
        //uiTestUtils.triggerPointerDragEvent(arg0, arg1)
    }

    /**
     * Emulate pointer event.
     */
    public void pointerTap(Point p) {
        pointerTap(p.x, p.y);
    }

    /**
     * Emulate pointer event.
     */
    public void pointerLongTap(Point p) {
        pointerLongTap(p.x, p.y);
    }

    /**
     * Emulate pointer event.
     */
    public void pointerDown(Point p) {
        pointerDown(p.x, p.y);
    }

    /**
     * Emulate pointer event.
     */
    public void pointerUp(Point p) {
        pointerUp(p.x, p.y);
    }

    /**
     * Emulate pointer event.
     */
    public void pointerDrag(Point p1, Point p2) {
        pointerDrag(p1.x, p1.y, p2.x, p2.y);
    }

    /**
     * Control tapping target.
     */
    public Point pointerTarget(Control aCtrl, int aType) {
        assertTrue(aCtrl.getSize().x > 0 && aCtrl.getSize().y > 0);
        Point res = new Point(0, 0);
        Rectangle rect = aCtrl.getBounds();
        switch (aType) {
            case SWT.CENTER:
                res = new Point(rect.x + rect.width / 2, rect.y + rect.height
                        / 2);
                break;
            case SWT.TOP:
                res = new Point(rect.x + rect.width / 2, rect.y);
                break;
            case SWT.BOTTOM:
                res = new Point(rect.x + rect.width / 2, rect.y + rect.height);
                break;
            case SWT.LEFT:
                res = new Point(rect.x, rect.y + rect.height / 2);
                break;
            case SWT.RIGHT:
                res = new Point(rect.x + rect.width, rect.y + rect.height / 2);
                break;
            case SWT.TOP | SWT.LEFT:
                res = new Point(rect.x, rect.y);
                break;
            case SWT.TOP | SWT.RIGHT:
                res = new Point(rect.x + rect.width, rect.y);
                break;
            case SWT.BOTTOM | SWT.LEFT:
                res = new Point(rect.x, rect.y + rect.height);
                break;
            case SWT.BOTTOM | SWT.RIGHT:
                res = new Point(rect.x + rect.width, rect.y + rect.height);
                break;
            default:
                assertTrue("No such pointer target type", false);
                break;
        }
        if (aCtrl.getParent() != null) {
            res = aCtrl.getParent().toDisplay(res);
        }
        return res;
    }

    /**
     * Control tapping target.
     */
    public Point pointerTarget(Control aCtrl) {
        return pointerTarget(aCtrl, SWT.CENTER);
    }

    public boolean isTouchEnabled() {
        //return spede().isTouchEnabled();
    	return true;
    }

    public void runBare() throws Throwable {
        long startMillis = 0;
        try {
            startMillis = System.currentTimeMillis();
            setUp();
            runTest();
        } catch (TestDisabledException e) {
        	if(disabledTests == null) throw new Exception("Test " + getName() + " is disabled");
            disabledTests.addElement(getName() + " (" + getClass().getName() + ")");
        } finally {
            tearDown();
            if (startMillis > 0) {
				long totalTime = System.currentTimeMillis() - startMillis;
				print("Total test case execution time: "
						+ (Float.valueOf(Long.toString(totalTime)).floatValue() / 1000)
						+ " s");
			}
            // Requests disposing all resources
            javax.microedition.lcdui.Display.getDisplay(getMIDlet()).setCurrent(null);
        }
    }
}
