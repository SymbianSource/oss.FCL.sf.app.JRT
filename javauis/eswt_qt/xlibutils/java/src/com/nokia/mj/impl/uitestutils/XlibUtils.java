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

import org.eclipse.swt.internal.Library;

final class XlibUtils {

	static {
		Library.loadLibrary("xlibevents");
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
	static final native int XSendKeyToFocusWindow(int keycode, int modifiers, int pressOrRelease);
	
	/**
	 * Returns the keycode of the given symbol (keysym) in the currently active
	 * keyboard layout. Keysym constants are defined in XlibKeys. 
	 * @param keysym
	 * @return The keycode or zero in case of an error
	 * @see XlibKeys
	 */
	static final native int XKeysymToKeycode(int keysym);

    /**
     * Sends a pointer press or release event. 
     * @param x X-coordinate to send, relative to X root window (i.e. screen). 
     * @param y Y-coordinate to send, relative to X root window (i.e. screen). 
     * @param modifiers The modifiers to send. 
     * @param button The button that's pressed or released. 
     * @param pressOrRelease Either PRESS or RELEASE must be passed here.  
     * @return 0 if successful, otherwise a non-zero error code
     */
    static final native int XSendPointerEvent(int x, int y, int button, int pressOrRelease);

    /**
     * Sends a pointer move event. 
     * @param x X-coordinate to send, relative to X root window (i.e. screen). 
     * @param y Y-coordinate to send, relative to X root window (i.e. screen).
     * @param button The button that's held pressed during the move. 
     * @return 0 if successful, otherwise a non-zero error code
     */
    static final native int XSendPointerMoveEvent(int x, int y, int button);
}
