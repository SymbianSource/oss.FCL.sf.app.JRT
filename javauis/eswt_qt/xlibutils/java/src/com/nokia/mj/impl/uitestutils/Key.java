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

/**
 * This class is used when sending emulated key events from automated test
 * cases. hides the platform keycode specifities from the test cases. There
 * might be an implementation of this class for different platforms enabling
 * running of the same key input test cases on each platform. This
 * implementation is for Xlib.
 */
public class Key {
    
    //
    // LOGICAL KEY CODES 
    //
    
    // Non unicode MIDP key codes, these are the correct values from MIDP spec. 
    public static final  int UpArrow                     = -1; 
    public static final  int DownArrow                   = -2;
    public static final  int LeftArrow                   = -3;
    public static final  int RightArrow                  = -4;
    public static final  int Select                      = -5; // Middle softkey
    public static final  int CBA1                        = -6; // Left softkey
    public static final  int CBA2                        = -7; // Right softkey
    public static final  int Clear                       = -8;    
    public static final  int Send                        = -10; 
    public static final  int End                         = -11;
    public static final  int Applications                = -12;
    public static final  int Voice                       = -13;
    public static final  int ClockwiseRotate             = -16;
    public static final  int CounterClockwiseRotate      = -17;
    public static final  int ClockwiseFastRotate         = -18;
    public static final  int CounterClockwiseFastRotate  = -19;
    public static final  int PlayPause                   = -20;
    public static final  int PreviousRew                 = -21;
    public static final  int NextFF                      = -22;
    public static final  int Stop                        = -23;
    public static final  int Edit                        = -50;
    public static final  int AAction                     = -51;
    public static final  int BAction                     = -52;
        
    // Non unicode non MIDP key codes. These values are just made up and are not from any spec. 
    public static final  int  Power                      = -100;
    public static final  int  Backspace                  = -101;
    public static final  int  GripOpen                   = -102;
    public static final  int  GripClose                  = -103;
    public static final  int  Camera                     = -104;
    public static final  int  VolumeUp                   = -105;
    public static final  int  VolumeDown                 = -106;
    public static final  int  PageUp                     = -107;
    public static final  int  PageDown                   = -108;
    public static final  int  HomeKey                    = -109;
    public static final  int  EndKey                     = -110;
    // Activate key represents a key that will cause a "default selection" of an item. On Linux
    // this is Enter, but on S60 it may be the same key as Select.
    public static final  int  Activate                   = -111;
    public static final  int  InsertKey                  = -112;
    public static final  int  ReturnKey                  = -113;
    public static final  int  DeleteKey                  = -114;
    public static final  int  Escape                     = -115;
    public static final  int  ControlRight               = -116;
    public static final  int  ControlLeft                = -117;
    public static final  int  ShiftRight                 = -118;
    public static final  int  ShiftLeft                  = -119;
    public static final  int  Multiply                   = -120;
    public static final  int  Divide                     = -121;
    public static final  int  Add                        = -122;
    public static final  int  Subtract                   = -123;

    public static final  int  Undefined                  = -121;
    
     // Map from logical key codes to Xlib key syms 
    static int keycodes[] = {
        UpArrow,
        DownArrow,
        LeftArrow,
        RightArrow,
        Select,
        CBA1,
        CBA2,
        Clear,
        Send,
        End,
        Applications,
        Voice,
        ClockwiseRotate,
        CounterClockwiseRotate,
        ClockwiseFastRotate,
        CounterClockwiseFastRotate,
        PlayPause,
        PreviousRew,
        NextFF,
        Stop,
        Edit,
        AAction,
        BAction,
        Power,
        Backspace,
        GripOpen,
        GripClose,
        Camera,
        VolumeUp,
        VolumeDown,
        PageUp,
        PageDown,
        HomeKey,
        EndKey,
        Activate,
        InsertKey,
        ReturnKey,
        DeleteKey,
        Escape,
        ControlRight,
        ControlLeft,
        ShiftRight,
        ShiftLeft,
        Multiply,
        Divide,
        Add,
        Subtract,
        Undefined
    };
    static int keysyms[] = {
        XlibKeys.XK_Up,         // UpArrow 
        XlibKeys.XK_Down,       // DownArrow
        XlibKeys.XK_Left,       // LeftArrow
        XlibKeys.XK_Right,      // RightArrow
        XlibKeys.XK_space,      // Select
        0,                      // CBA1
        0,                      // CBA2
        XlibKeys.XK_Clear,      // Clear    
        0,                      // Send 
        0,                      // End
        0,                      // Applications
        0,                      // Voice
        0,                      // ClockwiseRotate
        0,                      // CounterClockwiseRotate
        0,                      // ClockwiseFastRotate
        0,                      // CounterClockwiseFastRotate
        0,                      // PlayPause
        0,                      // PreviousRew
        0,                      // NextFF
        0,                      // Stop
        0,                      // Edit
        0,                      // AAction
        0,                      // BAction
        0,                      // Power
        XlibKeys.XK_BackSpace,  // Backspace
        0,                      // GripOpen
        0,                      // GripClose
        0,                      // Camera
        0,                      // VolumeUp
        0,                      // VolumeDown
        XlibKeys.XK_Page_Up,    // PageUp
        XlibKeys.XK_Page_Down,  // PageDown
        XlibKeys.XK_Home,       // HomeKey
        XlibKeys.XK_End,        // EndKey
        XlibKeys.XK_Return,     // Activate
        XlibKeys.XK_Insert,     // InsertKey
        XlibKeys.XK_Return,     // ReturnKey
        XlibKeys.XK_Delete,     // DeleteKey
        XlibKeys.XK_Escape,     // Esc
        XlibKeys.XK_Control_R,  // ControlRight
        XlibKeys.XK_Control_L,  // ControlLeft
        XlibKeys.XK_Shift_R,    // ControlRight
        XlibKeys.XK_Shift_L,    // ControlLeft
        XlibKeys.XK_KP_Multiply, // Multiply
        XlibKeys.XK_KP_Divide,  // Divide
        XlibKeys.XK_KP_Add,     // Add
        XlibKeys.XK_KP_Subtract, // Subtract
        XlibKeys.XK_Return                       // Undefined
    };
    
    protected int keysym(int code) {
        for(int i = 0; i < keycodes.length; ++i) {
            if(keycodes[i] == code) return keysyms[i];
        }
        return 0;
    }
    
    /** Default constructor. Only here for compatibility with old test cases. */
    public Key() {
        this.keyCode = 0;
    }
    
    /**
     * Same as Key(int), only here for compatibility with old test cases.
     * Scancode not used.
     */
    public Key(int keyCode, int scanCode) {
        this(keyCode);
    }
        
    /**
     * Constructor accepting a character code or a logical key code defined
     * above. E.g. to send a tab key pass '\t' here. To send an up arrow pass
     * UpArrow.
     * 
     * @param key
     *            A character code or a logical key code.
     */
    public Key(int key) {
            
        // Convert MIDP keyCodes to Xlib keycodes
        if (key < 0) {
            int keysym = keysym(key);
            if(keysym != 0) {
                keyCode = XlibUtils.XKeysymToKeycode(keysym);
            }
        } 
        // Try using character code as the keysym (except the null character)
        else {
            // Try if it maps to latin1
            keyCode = XlibUtils.XKeysymToKeycode(key);
            
            // If not latin1, try tty function keys
            if((keyCode == 0) && ((0xff00 & keyCode) == 0)) {
                keyCode = XlibUtils.XKeysymToKeycode(0xff00|key);
            }
            
            // More attempts
        } 
        
        if(keyCode == 0 && modifiers == 0) {
            throw new IllegalArgumentException("Failed to map to any keycode: code=" + key + " char =" + (char)key);
        }
     }    
    
    /**
     * Use constructor rather than setting directly, only public for
     * compatibility with old test cases.
     */
    public int keyCode; 
    
    /**
     * Not used for anything, only here for compatibility with old test cases.
     */
    public int scanCode; 
    
    /**
     * Inclusive OR of modifiers. Will be sent with the key event.
     * 
     * @see com.nokia.mj.impl.uitestutils.Modifier
     */
    public int modifiers;
}
