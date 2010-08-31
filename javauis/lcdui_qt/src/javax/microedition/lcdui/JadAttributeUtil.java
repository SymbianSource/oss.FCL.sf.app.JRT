/*
* Copyright (c) 2009,2010 Nokia Corporation and/or its subsidiary(-ies).
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

import com.nokia.mj.impl.rt.support.ApplicationInfo;

/**
 * Utilities for reading and checking the UI related JAD attributes.
 *
 */
final class JadAttributeUtil
{

    // ***** ATTRIBUTE CONSTANTS ******
    /**
     * jad attribute Nokia-UI-Enhancement
     */
    final static String ATTRIB_NOKIA_UI_ENHANCEMENT = "Nokia-UI-Enhancement";

    /**
     * jad attribute Nokia-MIDlet-S60-Selection-Key-Compatibility
     */
    final static String ATTRIB_NOKIA_MIDLET_S60_SELECTION_KEY_COMPATIBILITY = "Nokia-MIDlet-S60-Selection-Key-Compatibility";
    /**
     * jad attribute Nokia-MIDlet-No-Exit
     */
    final static String ATTRIB_NOKIA_MIDLET_NO_EXIT =  "Nokia-MIDlet-No-Exit";

    /**
     *  jad attribute Nokia-MIDlet-Background-Event
     */
    final static String ATTRIB_NOKIA_MIDLET_BACKGROUND_EVENT = "Nokia-MIDlet-Background-Event";

    /**
     * jad attribute Nokia-MIDlet-Tap-Detection-Options
     */
    final static String ATTRIB_NOKIA_MIDLET_TAP_DETECTION_OPTIONS = "Nokia-MIDlet-Tap-Detection-Options";

    /**
     * jad attribute Nokia-MIDlet-On-Screen-Keypad
     */
    final static String ATTRIB_NOKIA_MIDLET_ON_SCREEN_KEYPAD= "Nokia-MIDlet-On-Screen-Keypad";

    /**
     * jad attribute Nokia-MIDlet-App-Orientation
     */
    final static String ATTRIB_NOKIA_MIDLET_APP_ORIENTATION = "Nokia-MIDlet-App-Orientation";

    /**
     * jad attribute Nokia-MIDlet-Flip-Close
     */
    final static String ATTRIB_NOKIA_MIDLET_FLIP_CLOSE ="Nokia-MIDlet-Flip-Close";

    // ***** VALUE CONSTANTS *****

    /**
     * value CanvasHasBackground used with Nokia-UI-Enhancement
     * @see JadAttributeUtil#ATTRIB_NOKIA_UI_ENHANCEMENT
     */
    final static String VALUE_CANVAS_HAS_BACKGROUND= "CanvasHasBackground";

    /**
     * value MusicKeysSupported
     * @see JadAttributeUtil#ATTRIB_NOKIA_UI_ENHANCEMENT
     */
    final static String VALUE_MUSIC_KEYS_SUPPORTED = "MusicKeysSupported";
    /**
     * value true
     * @see JadAttributeUtil#ATTRIB_NOKIA_MIDLET_S60_SELECTION_KEY_COMPATIBILITY
     * @see JadAttributeUtil#ATTRIB_NOKIA_MIDLET_NO_EXIT
     */
    final static String VALUE_TRUE = "true";
    /**
     * value pause
     * @see JadAttributeUtil#ATTRIB_NOKIA_MIDLET_BACKGROUND_EVENT
     * @see JadAttributeUtil#ATTRIB_NOKIA_MIDLET_FLIP_CLOSE
     */
    final static String VALUE_PAUSE = "pause";

    /**
     * value no
     * @see JadAttributeUtil#ATTRIB_NOKIA_MIDLET_ON_SCREEN_KEYPAD
     */
    final static String VALUE_NO = "no";

    /**
     * value gameactions
     * @see JadAttributeUtil#ATTRIB_NOKIA_MIDLET_ON_SCREEN_KEYPAD
     */
    final static String VALUE_GAMEACTIONS ="gameactions";

    /**
     * value navigationkeys
     * @see JadAttributeUtil#ATTRIB_NOKIA_MIDLET_ON_SCREEN_KEYPAD
     */
    final static String VALUE_NAVIGATIONKEYS = "navigationkeys";

    /**
     * value portrait
     * @see JadAttributeUtil#ATTRIB_NOKIA_MIDLET_APP_ORIENTATION
     */
    final static String VALUE_PORTRAIT = "portrait";

    /**
     * value landscape
     * @see JadAttributeUtil#ATTRIB_NOKIA_MIDLET_APP_ORIENTATION
     */
    final static String VALUE_LANDSCAPE = "landscape";

    /**
     * value PopUpTextBox
     * @see JadAttributeUtil#ATTRIB_NOKIA_UI_ENHANCEMENT
     */
    final static String VALUE_POPUP_TEXTBOX = "PopUpTextBox";

    /**
     * value FullScreenTextBox
     * @see JadAttributeUtil#ATTRIB_NOKIA_UI_ENHANCEMENT
     */
    final static String VALUE_FULLSCREEN_TEXTBOX = "FullScreenTextBox";


    /**
     * Returns if the given value is set for the given midlet.
     * Use one of  the ATTRIB_ constants defined on {@link JadAttributeUtil} as
     * the attribute.
     *
     * @param attribute
     * @param value
     * @return
     */
    final static boolean isValue(final String attribute, final String value)
    {
        if(attribute == null || attribute.length() < 1) return false;
        if(value == null) return false;
        return value.equalsIgnoreCase(getValue(attribute));
    }

    /**
     * Reads the value of the given attribute
     * Use one of  the ATTRIB_ constants defined on {@link JadAttributeUtil} as
     * the attribute.
     * @param attribute
     * @return
     */
    final static String getValue(final String attribute)
    {
        return ApplicationInfo.getInstance().getAttribute(attribute);
    }


}
