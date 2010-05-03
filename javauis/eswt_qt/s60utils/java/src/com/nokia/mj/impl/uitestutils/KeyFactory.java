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
    public static Key LEFTUP_ARROW = new Key(Key.EStdKeyDevice10);
    public static Key RIGHTUP_ARROW = new Key(Key.EStdKeyDevice11);
    public static Key RIGHTDOWN_ARROW = new Key(Key.EStdKeyDevice12);
    public static Key LEFTDOWN_ARROW = new Key(Key.EStdKeyDevice13);
    
    public static Key SELECT_KEY = new Key(Key.Select);
    public static Key SK1 = new Key(Key.CBA1);
    public static Key SK2 = new Key(Key.CBA2);

    public static Key KEYPAD_0 = new Key(Key.EStdKeyNkp0);
    public static Key KEYPAD_1 = new Key(Key.EStdKeyNkp1);
    public static Key KEYPAD_2 = new Key(Key.EStdKeyNkp2);
    public static Key KEYPAD_3 = new Key(Key.EStdKeyNkp3);
    public static Key KEYPAD_4 = new Key(Key.EStdKeyNkp4);
    public static Key KEYPAD_5 = new Key(Key.EStdKeyNkp5);
    public static Key KEYPAD_6 = new Key(Key.EStdKeyNkp6);
    public static Key KEYPAD_7 = new Key(Key.EStdKeyNkp7);
    public static Key KEYPAD_8 = new Key(Key.EStdKeyNkp8);
    public static Key KEYPAD_9 = new Key(Key.EStdKeyNkp9);

    public static Key F1 = new Key(Key.EKeyF1);
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
    public static Key F15 = new Key(Key.EKeyF15);

    public static Key BACKSPACE = new Key(Key.Backspace);
    public static Key ENTER = new Key(Key.EKeyEnter);
    public static Key ESCAPE = new Key(Key.EKeyEscape);
    public static Key TAB = new Key(Key.EKeyTab);
    public static Key CTRL_RIGHT = new Key(Key.EStdKeyRightCtrl);
    public static Key CTRL_LEFT = new Key(Key.EStdKeyLeftCtrl);
    public static Key SHIFT_RIGHT = new Key(Key.EStdKeyRightShift);
    public static Key SHIFT_LEFT = new Key(Key.EStdKeyLeftShift);
    public static Key SPACE = new Key(Key.EStdKeySpace);
    public static Key DEL = new Key(Key.EKeyDelete);

    public static Key STAR = new Key('*');
    public static Key DIVIDE = new Key(Key.EStdKeyNkpForwardSlash);
    public static Key ADD = new Key(Key.EStdKeyNkpPlus);
    public static Key SUBTRACT = new Key(Key.EStdKeyNkpMinus);
    public static Key HASH = new Key('#');
    public static Key CLEAR = new Key(Key.EStdKeyBackspace);
    public static Key EDIT = new Key(Key.EStdKeyRightShift);
    public static Key SEND = new Key(Key.EStdKeyYes);
    public static Key END = new Key(Key.EStdKeyNo);
    public static Key APPLICATIONS = new Key(Key.EStdKeyApplication0);
    public static Key VOICE = new Key(Key.EStdKeyDevice6);
    public static Key SOFTKEY_1 = new Key(Key.EStdKeyDevice0);
    public static Key SOFTKEY_2 = new Key(Key.EStdKeyDevice1);

    public static Key key(int keyCode)
    {
        return new Key(keyCode);
    }
}
