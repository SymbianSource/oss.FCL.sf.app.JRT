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
package com.nokia.mj.impl.uitestutils;


final class S60Utils {

	static {
		// Should use vmport, this is CDC/J2SE API
		//System.loadLibrary("s60uitestutils");
		org.eclipse.swt.internal.Library.loadLibrary("s60uitestutils");
	}

    /**
     * Constant for key press event. 
     */
    static final int PRESS = 0;
    
    /**
     * Constant for key release event. 
     */
    static final int RELEASE = 1;
    
    /**
     * Button constants. 
     */
    static final int BUTTON1 = 1;
    static final int BUTTON2 = 2;
    static final int BUTTON3 = 3;


	/**
	 * Sends a key event to the window which has the keyboard focus. Get the keycode of 
	 * the desired symbol using XKeysymToKeycode and pass that here. Or if the desired 
	 * keycode is known it can be passed directly. 
	 * @param keycode The keycode of the key to send. 
	 * @param modifiers The modifiers to send. 
     * @param pressOrRelease Either PRESS or RELEASE must be passed here.  
	 * @return 0 if successful, otherwise a non-zero error code
     * @see XlibKeys
	 */
    static final native int S60SendKeyToFocusWindow(int keycode, int modifiers, int pressOrRelease);
	
	/**
	 * Returns the keycode of the given symbol (keysym) in the currently active
	 * keyboard layout. Keysym constants are defined in XlibKeys. 
	 * @param keysym
	 * @return The keycode or zero in case of an error
	 * @see XlibKeys
	 */
	static final native int S60KeysymToKeycode(int keysym);

    /**
     * Sends a pointer press or release event. 
     * @param x X-coordinate to send, relative to X root window (i.e. screen). 
     * @param y Y-coordinate to send, relative to X root window (i.e. screen). 
     * @param modifiers The modifiers to send. 
     * @param button The button that's pressed or released. 
     * @param pressOrRelease Either PRESS or RELEASE must be passed here.  
     * @return 0 if successful, otherwise a non-zero error code
     */
    static final native int S60SendPointerEvent(int x, int y, int button, int pressOrRelease);

    /**
     * Sends a pointer move event. 
     * @param x X-coordinate to send, relative to X root window (i.e. screen). 
     * @param y Y-coordinate to send, relative to X root window (i.e. screen).
     * @param button The button that's held pressed during the move. 
     * @return 0 if successful, otherwise a non-zero error code
     */
    static final native int S60SendPointerMoveEvent(int x, int y, int button);

    /**
     * Disables the screensaver. 
     */
    static final native void S60DisableScreensaver();

    /**
     * Sets the doubleclick interval. Set to 0 to disable doubleclicks. 
     * @param interval New doubleclick interval
     */
    static final native void S60SetDoubleclickInterval(int interval);

	/*static final native int _create(int toolkitHandle, UITestUtils testUtils); 
	static final native void _dispose(int toolkitHandle, UITestUtils testUtils, int testUtilsHandle);

	static final native void _startTest(int toolkitHandle, int handle, String screenShotDir); 	
	static final native void _endTest(int toolkitHandle, int handle, boolean passed, String msg);

	static final native void _log(int toolkitHandle, int handle, String msg); 	
	static final native void _logResult(int toolkitHandle, int handle, boolean passed, String msgPassed, String msgFailed);

	static final native void _triggerKeyPressEvent(int toolkitHandle, int handle, int keyCode, int scanCode); 	
	static final native int _triggerKeyRepeatEvents(int toolkitHandle, int handle, int keyCode, int scanCode, int count); 	
	static final native int _triggerMediaKeyRepeatEvents(int toolkitHandle, int handle, int keyCode, int scanCode, int pressedTimeInMillis); 	
	
	static final native boolean _getScreenShot(int toolkitHandle, int handle, String screenShotName);
	static final native boolean _getScreenShot(int toolkitHandle, int handle, String screenShotName, 
	                                      int areaToCompare, int displayableType);
	static final native boolean _getScreenShot(int toolkitHandle, int handle, String screenShotName, 
	                                      int topLeftX, int topLeftY, int bottomRightX, int bottomRightY);
	
	static final native void _triggerPointerDownEvent(int toolkitHandle, int handle, int x, int y); 	
	static final native void _triggerPointerUpEvent(int toolkitHandle, int handle, int x, int y); 	
	static final native void _triggerPointerDragEvent(int toolkitHandle, int handle, int x, int y); 	

	static final native void _changeQwertyInput(int toolkitHandle, int handle, boolean aOn); 	
	static final native int _switchResolution(int toolkitHandle, int handle, int numTimes, int delayMillis); 

    static final native void _getBoundsOf(int toolkitHandle, int handle, int type, int[] coordinates);	
    
    static final native void _allowScreenSaver(int toolkitHandle, int handle, int allow);*/

}
