// UITestUtils.java // // //

package com.nokia.mj.impl.uitestutils;

import java.lang.String;
import org.eclipse.swt.internal.qt.OS;

public class UITestUtils {

    private static final int RES_CHANGE_DEFAULT_DELAY = 5000;

    public UITestUtils() {
        // Always disable screensaver when running tests.
        S60Utils.S60DisableScreensaver(); 	
    }

    /** Init test and s. shot directory, make sure you use slashes to start and
    end it dir name */
    public void startTest(String screenShotDir) {
    }

    /** Ends the test, prints a summary, etc */
    public void endTest(boolean passed, String msg) {
    }

    /** Log a message */
    public void log(String msg) {
    }

    /** Logs one of the two messages and either PASSED or FAILED according to
    value of    passed parameter */
    public void log(boolean passed, String msgPassed, String msgFailed) {
    }

    /** Sends a key event to the midlet using the specified key and scan code */
    public void triggerKeyPressEvent(Key key) {
        int keycode = key.scanCode;
        int modifiers = key.modifiers;
        if(S60Utils.S60SendKeyToFocusWindow(keycode, modifiers, S60Utils.PRESS) != 0)
            throw new RuntimeException("Emulating key press failed");
        if(S60Utils.S60SendKeyToFocusWindow(keycode, modifiers, S60Utils.RELEASE) != 0)
            throw new RuntimeException("Emulating key release failed");
    }

    /** Sends a key down event to the application using the specified key */
    public void triggerKeyDownEvent(Key key) {
        int keycode = key.scanCode;
        int modifiers = key.modifiers;
        if(S60Utils.S60SendKeyToFocusWindow(keycode, modifiers, S60Utils.PRESS) != 0)
            throw new RuntimeException("Emulating key press failed");
    }

    /** Sends a key up event to the application using the specified key */
    public void triggerKeyUpEvent(Key key) {
        int keycode = key.scanCode;
        int modifiers = key.modifiers;
        if(S60Utils.S60SendKeyToFocusWindow(keycode, modifiers, S60Utils.RELEASE) != 0)
            throw new RuntimeException("Emulating key release failed");
    }


    /** Triggers key repeat events using the specified key and scan code.
        Calling this method will trigger the following events to Canvas/CustomItem:
            1. keyPressed event
            2. keyRepeated events, the number of events is indicated by count parameter
            3. keyReleased events

        NOTE 1: This method returns right after triggering the keyPressed event. There should be enough delay
        after calling this so that all repeat events can be handled before the test result is checked.
        The delay before triggering first repeat event is 0.6 seconds and between sequential repeat
        events 0.2 seconds.

        NOTE 2: For triggering media key repeat events triggerMediaKeyRepeatEvents()
        must be used.
    */
    public int triggerKeyRepeatEvents(Key key, int count) {
        return 0;
    }

    /** This method must be used when triggering repeat events for media keys.
        pressedTimeInMillis defines how long a key is kept pressed down.

        NOTE 1: This method returns right after triggering the keyDown event. There should be enough delay
         (> pressedTimeInMillis) after calling this so that all repeat events and key up can be handled before
         the test result is checked.
    */
    public int triggerMediaKeyRepeatEvents(Key key, int pressedTimeInMillis) {
        return 0;
    }

    /** Sends pointer down event to the midlet */
    public void triggerPointerDownEvent(int x, int y) {
        // Setting double click interval to 0 disables double clicks, making it possible
        // to click around in the test cases quickly without causing unwanted double click
        // events 
        OS.QApplication_setDoubleClickInterval(0); 	
        if(S60Utils.S60SendPointerEvent(x, y, S60Utils.BUTTON1, S60Utils.PRESS) != 0)
            throw new RuntimeException("Emulating pointer down failed");
    }

    /** Sends pointer up event to the midlet */
    public void triggerPointerUpEvent(int x, int y) {
        if(S60Utils.S60SendPointerEvent(x, y, S60Utils.BUTTON1, S60Utils.RELEASE) != 0)
            throw new RuntimeException("Emulating pointer up failed");
    }

    /** Sends pointer drag event to the midlet */
    public void triggerPointerDragEvent(int x, int y) {
        if(S60Utils.S60SendPointerMoveEvent(x, y, S60Utils.BUTTON1) != 0 ) {
            throw new RuntimeException("Emulating pointer move failed");
        }
    }

    /**
     * Sends a pointer event to open up a context menu. E.g. long-press, right-click
     * or such depending on the platform.
     */
    public void triggerContextPointerEvent(int x, int y) {
        if(S60Utils.S60SendPointerEvent(x, y, S60Utils.BUTTON3, S60Utils.PRESS) != 0)
            throw new RuntimeException("Emulating pointer down failed");
        if(S60Utils.S60SendPointerEvent(x, y, S60Utils.BUTTON3, S60Utils.RELEASE) != 0)
            throw new RuntimeException("Emulating pointer up failed");
    }

    /** Takes a screen shot and saves it into a .png file. Returns true if
    reference screen shot exists and is identical to the new screen shot*/
    public boolean getScreenShot(String screenShotName) {
        return true;
    }

    /** Takes a screen shot and saves it into a .png file. Returns true if
    reference screen shot exists and compared area is identical in the reference and new screen shot.
    areaToCompare and displayableType define the area that is compared. See also SpedeRunner.java.*/
    public boolean getScreenShot(String screenShotName, int areaToCompare, int displayableType) {
        return true;
    }

    /** Takes a screen shot and saves it into a .png file. Returns true if
    reference screen shot exists and compared area is identical in the reference and new screen shot.
    Four integer parameters define the rectangle area for screen shot comparison. */
    public boolean getScreenShot(String screenShotName, int topLeftX, int topLeftY,
                                                        int bottomRightX, int bottomRightY) {
        return true;
    }

    public void dispose() {
    }

    /** Enable or disable QWERTY input */
    public void changeQwertyInput(boolean aOn) {
    }

    public int switchResolution() {
        return switchResolution(1);
    }

    /** Change the screen resolution - works only on the emulator */
    public int switchResolution(int numTimes) {
        return 0;
    }

    /** Change the screen resolution - works only on the emulator */
    public int switchResolutionWithDelay(int delayMillis) {
        return 0;
    }

    /**
     * Get the top left and bottom right coordinates of main pane, form rect, etc.
     *
     * @param coordinates   The coordinates are returned in this array
     * @param type          Defines for which area the coordinates are fetched
     */
    public void getBoundsOf(int type, int[] coordinates) {
    }

    /**
     * Disable screen saver
     */
    public void disableScreenSaver() {
    }

    /**
     * Enable screen saver
     */
    public void enableScreenSaver() {
    }
}
