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
package com.nokia.openlcdui.mt.canvas;

import junit.framework.*;

import javax.microedition.lcdui.*;

import com.nokia.openlcdui.mt.SWTTestCase;

/**
 * CanvasTest.
 */
public class CanvasTest extends SWTTestCase {

    private TestCanvas testCanvas;

    final int[][] refKeyToGameTable = {
        {50, Canvas.UP},
        {-1, Canvas.UP},
        {56, Canvas.DOWN},
        {-2, Canvas.DOWN},
        {52, Canvas.LEFT},
        {-3, Canvas.LEFT},
        {54, Canvas.RIGHT},
        {-4, Canvas.RIGHT},
        {53, Canvas.FIRE},
        {-5, Canvas.FIRE},
        {-10, Canvas.FIRE},
        {55, Canvas.GAME_A},
        {57, Canvas.GAME_B},
        {42, Canvas.GAME_C},
        {35, Canvas.GAME_D},
    };

    final int[][] refGameToKeyTable = {
        {-1, Canvas.UP},
        {-2, Canvas.DOWN},
        {-3, Canvas.LEFT},
        {-4, Canvas.RIGHT},
        {-5, Canvas.FIRE},
        {55, Canvas.GAME_A},
        {57, Canvas.GAME_B},
        {42, Canvas.GAME_C},
        {35, Canvas.GAME_D},
    };

    /**
     * Constructor.
     */
    public CanvasTest() {
    }

    /**
     * Constructor.
     *
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public CanvasTest(String sTestName) {
        super(sTestName);
    }

    /* (non-Javadoc)
     * @see junit.framework.TestCase#setUp()
     */
    protected void setUp() throws Exception {
        super.setUp();
        System.out.println("CanvasTest setup enter");
        testCanvas = new TestCanvas();
        Display.getDisplay(getMIDlet()).setCurrent(testCanvas);
        System.out.println("CanvasTest setup exit");
    }

    /* (non-Javadoc)
     * @see junit.framework.TestCase#tearDown()
     */
    protected void tearDown() throws Exception {
        super.tearDown();
        System.out.println("CanvasTest teadDown");
        testCanvas = null;
    }

    /**
     * To create the test suite. You need to add a new aSuite.addTest entry for
     * any new test methods.
     *
     * @return new testsuite.
     */
    public static Test suite() {
		TestSuite suite = new TestSuite();

	    java.util.Vector methodNames;
	    java.util.Enumeration e;

	    // Add widget tests
	    methodNames = CanvasTest.methodNames();
	    e = methodNames.elements();
	    while (e.hasMoreElements()) {
	        suite.addTest(new CanvasTest((String)e.nextElement()));
	    }

		return suite;
	}

    public static java.util.Vector methodNames() {
        java.util.Vector methodNames = new java.util.Vector();
        methodNames.addElement("testCreateCanvas");
        methodNames.addElement("testIsDoubleBuffered");
        methodNames.addElement("testHasPointerEvents");
        methodNames.addElement("testHasPointerMotionEvents");
        methodNames.addElement("testHasRepeatEvents");
        methodNames.addElement("testSetFullScreenMode");
        methodNames.addElement("testGetGameAction");
        methodNames.addElement("testGetKeyCode");
        methodNames.addElement("testGetKeyName");
        methodNames.addElement("testServiceRepaints");
        methodNames.addElement("testGraphicsProperties");
        return methodNames;
    }
    
    protected void runTest() throws Throwable {
        if (getName().equals("testCreateCanvas")) testCreateCanvas();
        else if (getName().equals("testIsDoubleBuffered")) testIsDoubleBuffered();
        else if (getName().equals("testHasPointerEvents")) testHasPointerEvents();
        else if (getName().equals("testHasPointerMotionEvents")) testHasPointerMotionEvents();
        else if (getName().equals("testHasRepeatEvents")) testHasRepeatEvents();
        else if (getName().equals("testSetFullScreenMode")) testSetFullScreenMode();
        else if (getName().equals("testGetGameAction")) testGetGameAction();
        else if (getName().equals("testGetKeyCode")) testGetKeyCode();
        else if (getName().equals("testGetKeyName")) testGetKeyName();
        else if (getName().equals("testServiceRepaints")) testServiceRepaints();
        else if (getName().equals("testGraphicsProperties")) testGraphicsProperties();
        else super.runTest();
    }

    
    public void testCreateCanvas() {
        Canvas canv = new TestCanvas();
        canv.isDoubleBuffered();
    }

    public void testShowNotify() {
        fail("Canvas.showNotify is not supported due to not implemented ShellListener.shellActivated");
    }

    public void testHideNotify() {
        fail("Canvas.hideNotify is not supported due to not implemented ShellListener.shellDeactivated");
    }

    public void testIsDoubleBuffered() {
        assertTrue("Canvas.isDoubleBuffered returned false", testCanvas.isDoubleBuffered());
    }

    public void testHasPointerEvents() {
        assertTrue("Canvas.hasPointerEvents returned false", testCanvas.hasPointerEvents());
    }

    public void testHasPointerMotionEvents() {
        assertTrue("Canvas.hasPointerMotionEvents returned false", testCanvas.hasPointerMotionEvents());
    }

    public void testHasRepeatEvents() {
        assertTrue("Canvas.hasRepeatEvents returned false", testCanvas.hasRepeatEvents());
    }

    public void testSetFullScreenMode() {
        testCanvas.setFullScreenMode(true);
        testCanvas.setFullScreenMode(false);
    }

    public void testGetGameAction() {
        for (int i = 0; i < refGameToKeyTable.length; i++) {
            int refKeyCode = refGameToKeyTable[i][0];
            int refGameAction = refGameToKeyTable[i][1];

            int testGameAction = testCanvas.getGameAction(refKeyCode);
            assertTrue("Game action for keycode " + refKeyCode + " invalid. Returned: "
                    + testGameAction + ", expected: " + refGameAction,
                    refGameAction == testGameAction);
        }

        // now test for invalid input
        try {
			testCanvas.getGameAction(0);
			fail("IllegalArgumentException not throw for invalid input");
		}
        catch (IllegalArgumentException e) {
			// Ok
		}
    }


    public void testGetKeyCode() {
        for (int i = 0; i < refGameToKeyTable.length; i++) {
            int refKeyCode = refGameToKeyTable[i][0];
            int refGameAction = refGameToKeyTable[i][1];

            int testKeyCode = testCanvas.getKeyCode(refGameAction);
            assertTrue("Key code for game action " + refGameAction + " invalid. Returned: "
                    + testKeyCode + ", expected: " + refKeyCode,
                    refKeyCode == testKeyCode);
        }

        // now test for invalid input

        try {
            testCanvas.getKeyCode(-111);
            fail("IllegalArgumentException not throw for invalid input");
		}
        catch (IllegalArgumentException e) {
			// Ok
		}
    }

    public void testGetKeyName() {
        for (int i = 0; i < refKeyToGameTable.length; i++) {
            int refKeyCode = refKeyToGameTable[i][0];
            String testKeyName = testCanvas.getKeyName(refKeyCode);
            assertTrue("Canvas.getKeyName() returned null", testKeyName != null);
        }
    }

    public void testServiceRepaints() {
        testCanvas.repaint();
        testCanvas.serviceRepaints();
    }

    public void testGraphicsProperties() {
        testCanvas.reset();
        testCanvas.repaint();
        testCanvas.serviceRepaints();
        assertTrue(testCanvas.failMsg, testCanvas.passed);
    }

    class TestCanvas extends Canvas {

        public boolean passed;
        public String failMsg;

        public TestCanvas() {
            reset();
        };

        public void reset() {
            passed = true;
            failMsg = "";
        }

        protected void paint(Graphics g) {
            if (g.getClipWidth() <= 0) {
                failMsg = "Graphics.getClipWidth() returns <= 0 in Canvas.paint()";
                passed = false;
                return;
            }

            if (g.getClipHeight() <= 0) {
                failMsg = "Graphics.getClipHeight() returns <= 0 in Canvas.paint()";
                passed = false;
                return;
            }

            if (g.getColor() != 0x0000000) {
                failMsg = "Graphics.getColor() returns not black in Canvas.paint()";
                passed = false;
                return;
            }

            Font graphicsFont = g.getFont();
            Font defaultFont = Font.getDefaultFont();

            if (graphicsFont.getFace() != defaultFont.getFace()
                    || graphicsFont.getStyle() != defaultFont.getStyle()
                    || graphicsFont.getSize() != defaultFont.getSize()) {
                failMsg = "Graphics.getFont() returns not default font in Canvas.paint()";
                passed = false;
                return;
            }

            if (g.getStrokeStyle() != Graphics.SOLID) {
                failMsg = "Graphics.getStrokeStyle() returns not SOLID in Canvas.paint()";
                passed = false;
                return;
            }

            if (g.getTranslateX() != 0
                    || g.getTranslateY() != 0) {
                failMsg = "Graphics.getTranslateX() and getTranslateY() "
                        + "return not (0,0) in Canvas.paint()";
                passed = false;
                return;
            }

            if (!isShown()) {
                failMsg = "Canvas.isShown returns false in Canvas.paint()";
                passed = false;
                return;
            }

            g.setColor(255, 255, 0);
            g.setFont(Font.getFont(Font.FACE_MONOSPACE, Font.STYLE_ITALIC,
                    Font.SIZE_LARGE));
            g.setStrokeStyle(Graphics.DOTTED);
            g.translate(20, 30);
        }
    }

}
