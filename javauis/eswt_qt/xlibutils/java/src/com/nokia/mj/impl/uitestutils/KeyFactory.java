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

public class KeyFactory
{

    public static Key RIGHT_ARROW = new Key(Key.RightArrow);
    public static Key LEFT_ARROW = new Key(Key.LeftArrow);
    public static Key UP_ARROW = new Key(Key.UpArrow);
    public static Key DOWN_ARROW = new Key(Key.DownArrow);
    public static Key LEFTUP_ARROW = new Key(Key.HomeKey);
    public static Key RIGHTUP_ARROW = new Key(Key.PageUp);
    public static Key RIGHTDOWN_ARROW = new Key(Key.PageDown);
    public static Key LEFTDOWN_ARROW = new Key(Key.EndKey);
    
    public static Key SELECT_KEY = new Key(Key.Select);
    public static Key SK1 = new Key(Key.Undefined);
    public static Key SK2 = new Key(Key.Undefined);

    public static Key KEYPAD_0 = new Key('0');
    public static Key KEYPAD_1 = new Key('1');
    public static Key KEYPAD_2 = new Key('2');
    public static Key KEYPAD_3 = new Key('3');
    public static Key KEYPAD_4 = new Key('4');
    public static Key KEYPAD_5 = new Key('5');
    public static Key KEYPAD_6 = new Key('6');
    public static Key KEYPAD_7 = new Key('7');
    public static Key KEYPAD_8 = new Key('8');
    public static Key KEYPAD_9 = new Key('9');

    /*public static Key F1 = new Key(Key.EKeyF1);
    public static Key F2 = new Key(Key.EKeyF2);
    public static Key F3 = new Key(Key.EKeyF3);
    public static Key F4 = new Key(Key.EKeyF4);
    public static Key F5 = new Key(Key.EKeyF5);
    public static Key F6 = new Key(Key.EKeyF6);
    public static Key F7 = new Key(Key.EKeyF7);
    public static Key F8 = new Key(Key.EKeyF8);
    public static Key F9 = new Key(Key.EKeyF9);
    public static Key F10 = new Key(Key.EKeyF10);
    public static Key F11 = new Key(Key.EKeyF11);
    public static Key F12 = new Key(Key.EKeyF12);
    public static Key F13 = new Key(Key.EKeyF13);
    public static Key F14 = new Key(Key.EKeyF14);
    public static Key F15 = new Key(Key.EKeyF15);*/

    public static Key BACKSPACE = new Key(Key.Backspace);
    public static Key ENTER = new Key(Key.ReturnKey);
    public static Key ESCAPE = new Key(Key.Escape);
    public static Key TAB = new Key('\t');
    public static Key CTRL_RIGHT = new Key(Key.ControlRight);
    public static Key CTRL_LEFT = new Key(Key.ControlLeft);
    public static Key SHIFT_RIGHT = new Key(Key.ShiftRight);
    public static Key SHIFT_LEFT = new Key(Key.ShiftLeft);
    public static Key SPACE = new Key(' ');
    public static Key DEL = new Key(Key.DeleteKey);

    public static Key STAR = new Key(Key.Multiply);
    public static Key DIVIDE = new Key(Key.Divide);
    public static Key ADD = new Key(Key.Add);
    public static Key SUBTRACT = new Key(Key.Subtract);
    public static Key HASH = new Key('#');
    public static Key CLEAR = new Key(Key.Backspace);
    public static Key EDIT = new Key(Key.Backspace);
    public static Key SEND = new Key(Key.ReturnKey);
    public static Key END = new Key(Key.Undefined);
    public static Key APPLICATIONS = new Key(Key.Undefined);
    //public static Key VOICE = new Key(Key.EStdKeyDevice6);
    public static Key SOFTKEY_1 = new Key(Key.Undefined);
    public static Key SOFTKEY_2 = new Key(Key.Undefined);

    public static Key key(int keyCode)
    {
        return new Key(keyCode);
    }
}
