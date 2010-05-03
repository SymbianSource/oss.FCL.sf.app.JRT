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
package t_uirobot.customitem;

import j2meunit.framework.*;

import javax.microedition.lcdui.*;

import t_uirobot.ItemUITestBase;

import com.nokia.mj.impl.uitestutils.Key;

/**
 * TEST CASE SPECIFICATION <br>
 * <br>
 * Short description of the module test:
 * <li> UI-robot based test for CustomItem. <br>
 * <br>
 * Created: 2008-12-01
 */
public class CustomItemTest extends ItemUITestBase {

    private static String longString = "Lorem ipsum dolor sit amet, "
        + "consectetuer adipiscing elit. Cras turpis ligula, "
        + "condimentum nec, rhoncus quis, molestie in, arcu. "
        + "Curabitur id lacus. Quisque dictum nulla id odio. "
        + "Nullam nec urna. Cras ac lacus nec lacus iaculis aliquet."
        + " Integer ut eros. Proin laoreet justo et augue. "
        + "Praesent dui. Proin vel leo a eros auctor convallis. "
        + "Aenean urna nunc, sagittis vel, pellentesque a, "
        + "luctus a, metus. Phasellus posuere lacus nec augue.";

    /**
     * Constructor.
     */
    public CustomItemTest() {
    }

    /**
     * Constructor.
     *
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public CustomItemTest(String sTestName, TestMethod rTestMethod) {
        super(sTestName, rTestMethod);
    }

    /**
     * Creates the test suite. You need to add a new aSuite.addTest antry for
     * any new test methods, otherwise they won't be run.
     *
     * @return New testsuite.
     */
    public Test suite() {
        TestSuite aSuite = new TestSuite();

        aSuite.addTest(new CustomItemTest("testBasicMethodCalls",
                new TestMethod() {
                    public void run(TestCase tc) {
                        ((CustomItemTest) tc).testBasicMethodCalls();
                    }
                }));

        aSuite.addTest(new CustomItemTest("testInnerTraverse1",
                new TestMethod() {
                    public void run(TestCase tc) {
                        ((CustomItemTest) tc).testInnerTraversal(1);
                    }
                }));

        aSuite.addTest(new CustomItemTest("testInnerTraverse3",
                new TestMethod() {
                    public void run(TestCase tc) {
                        ((CustomItemTest) tc).testInnerTraversal(3);
                    }
                }));

        aSuite.addTest(new CustomItemTest("testInnerTraverse5",
                new TestMethod() {
                    public void run(TestCase tc) {
                        ((CustomItemTest) tc).testInnerTraversal(5);
                    }
                }));

        return aSuite;
    }

    /**
     * Tests that getMinimumSize returns non-zero values.
     */
    public void testBasicMethodCalls() {
        Form bg = new Form("background");
        setCurrent(bg);

        MyCustomItem ci1 = new MyCustomItem("CustomItem 1", 1);
        ci1.sizeChanged(1000, 75);
        ci1.clearState();
        MyCustomItem ci2 = new MyCustomItem("CustomItem 2", 1);
        ci2.sizeChanged(1000, 75);
        ci2.clearState();
        form.append(ci1);
        form.append(ci2);

        // Set form current
        setCurrent(form);

        assertEquals("1. Invalid CustomItem state",
                MyCustomItem.TEST_SIZECHANGE
                | MyCustomItem.TEST_SHOW
                | MyCustomItem.TEST_PAINT
                | MyCustomItem.TEST_TRAVERSEIN,
                ci1.getState());

        assertTrue("invalid width", ci1.width < 1000);

        ci1.clearState();

        // Simulate key presses
        key(Key.CBA1);
        key(Key.CBA2);
        assertEquals("2. Invalid CustomItem state",
                MyCustomItem.TEST_KEYPRESS
                | MyCustomItem.TEST_KEYRELEASE,
                ci1.getState());


        // Hide form
        setCurrent(bg);
        assertTrue("hideNotify() not called",
                ci1.isCalled(MyCustomItem.TEST_HIDE));

        // Set current item
        ci1.clearState();
        display.setCurrentItem(ci1);
        block(CHANGE_DISPLAYABLE_DELAY);
        assertTrue("showNotify() not called",
                ci1.isCalled(MyCustomItem.TEST_SHOW));

        // partial repaint
        ci1.clearState();
        ci1.doRepaint(10, 5, 20, 10);
        block(KEYEVENT_DELAY);
        assertTrue("paint() not called", ci1.isCalled(MyCustomItem.TEST_PAINT));
        assertTrue("wrong clipping area",
                ci1.clipX == 10 && ci1.clipY == 5 &&
                ci1.clipW == 20 && ci1.clipH == 10);
    }

    void testInnerTraversal(int numInnerTraversals) {
        Form fg = new Form("InnerTraverse " + numInnerTraversals);
        MyCustomItem ci1 = new MyCustomItem("CustomItem 1", numInnerTraversals);
        ci1.sizeChanged(1000, 75);
        ci1.clearState();
        MyCustomItem ci2 = new MyCustomItem("CustomItem 2", numInnerTraversals);
        ci2.sizeChanged(1000, 75);
        ci2.clearState();
        fg.append(ci1);
        fg.append(ci2);

        // Set form current
        setCurrent(fg);

        // Traversing to CI 2
        ci1.clearState();
        ci2.clearState();
        for (int i = 0; i < numInnerTraversals; i++) {
            assertTrue("Illegal traversing to CI 2",
                    !ci1.isCalled(MyCustomItem.TEST_TRAVERSEOUT)
                    && !ci2.isCalled(MyCustomItem.TEST_TRAVERSEIN));
            key(Key.DownArrow);
        }
        assertTrue("Should traverse to CI 2",
                ci1.isCalled(MyCustomItem.TEST_TRAVERSEOUT)
                && ci2.isCalled(MyCustomItem.TEST_TRAVERSEIN));

        // Traversing back to CI 1
        ci1.clearState();
        ci2.clearState();
        for (int i = 0; i < numInnerTraversals - 1; i++) {
            assertTrue("1. Illegal traversing to CI 1",
                    !ci1.isCalled(MyCustomItem.TEST_TRAVERSEIN)
                    && !ci2.isCalled(MyCustomItem.TEST_TRAVERSEOUT));
            key(Key.DownArrow);
        }

        for (int i = 0; i < numInnerTraversals; i++) {
            assertTrue("2. Illegal traversing to CI 1",
                    !ci1.isCalled(MyCustomItem.TEST_TRAVERSEIN)
                    && !ci2.isCalled(MyCustomItem.TEST_TRAVERSEOUT));
            key(Key.UpArrow);
        }

        assertTrue("Should traverse to CI 1",
                ci1.isCalled(MyCustomItem.TEST_TRAVERSEIN)
                && ci2.isCalled(MyCustomItem.TEST_TRAVERSEOUT));
    }

    class MyCustomItem extends CustomItem {

        static final int TEST_SHOW = 1;
        static final int TEST_HIDE = 2;
        static final int TEST_SIZECHANGE = 4;
        static final int TEST_PAINT = 8;
        static final int TEST_TRAVERSEIN = 16;
        static final int TEST_TRAVERSEOUT = 32;
        static final int TEST_KEYPRESS = 64;
        static final int TEST_KEYRELEASE = 128;

        private int numFocus;
        private int focus;
        private int state;

        int clipX, clipY, clipW, clipH;
        int width, height;

        protected MyCustomItem(String label, int numInnerFocus) {
            super(label);
            numFocus = Math.max(1, numInnerFocus);
            focus = -1;
        }

        void clearState() {
            state = 0;
        }

        int getState() {
            return state;
        }

        boolean isCalled(int mask) {
            return (state & mask) != 0;
        }

        private void setState(int newState) {
            state |= newState;
        }

        void doRepaint(int x, int y, int w, int h) {
            repaint(x, y, w, h);
        }

        protected void keyPressed(int keyCode) {
            setState(TEST_KEYPRESS);
        }

        protected void keyReleased(int keyCode) {
            setState(TEST_KEYRELEASE);
        }

        protected void showNotify() {
            setState(TEST_SHOW);
        }

        protected void hideNotify() {
            setState(TEST_HIDE);
        }

        protected void sizeChanged(int width, int height) {
            this.width = width;
            this.height = height;
            setState(TEST_SIZECHANGE);
        }

        protected boolean traverse(int dir, int viewportWidth,
                int viewportHeight, int[] visRect_inout) {
            boolean ret = false;

            setState(TEST_TRAVERSEIN);
            if (numFocus == 1) {
                focus = 0;
            }
            else if (numFocus > 1) {
                int div = numFocus + 1;
                focus++;
                if (dir == Canvas.UP || dir == Canvas.LEFT) {
                    focus = (focus - 1 + div) % div;
                }
                else if (dir == Canvas.DOWN || dir == Canvas.RIGHT) {
                    focus = (focus + 1 + div) % div;
                }
                else {
                    focus = 1;
                }
                focus--;

                if (focus > -1) {
                    ret = true;
                }
            }

            return ret;
        }

        protected void traverseOut() {
            setState(TEST_TRAVERSEOUT);
            focus = -1;
            repaint();
        }

        protected void paint(Graphics g, int w, int h) {
            setState(TEST_PAINT);

            clipX = g.getClipX();
            clipY = g.getClipY();
            clipW = g.getClipWidth();
            clipH = g.getClipHeight();

            g.setColor(0xffffff);
            g.fillRect(0, 0, w, h);

            if (focus > -1) {
                g.setColor(0x000000);
                g.fillRect(0, focus * (h / numFocus), w, h / numFocus);
            }
        }

        protected int getMinContentHeight() {
            return height;
        }

        protected int getMinContentWidth() {
            return width;
        }

        protected int getPrefContentHeight(int h) {
            return height;
        }

        protected int getPrefContentWidth(int w) {
            return width;
        }

    }

}




