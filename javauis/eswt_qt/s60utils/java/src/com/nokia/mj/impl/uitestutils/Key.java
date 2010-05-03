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
//Key.java
//
//
//

package com.nokia.mj.impl.uitestutils;

public class Key {

    //
    // NON UNICODE LOGICAL KEY CODES
    // pass them to class c.tor to generate symbian symbian scan and key codes
    // (for unicode keys use unicode char)
    //

    //non unicode MIDP key codes
    public static final  int UpArrow                     = -1;
    public static final  int DownArrow                   = -2;
    public static final  int LeftArrow                   = -3;
    public static final  int RightArrow                  = -4;
    public static final  int Clear                       = -5;  // Clear key
    public static final  int Select                      = -6;  // Selection key
    public static final  int Edit                        = -7; // Edit Key
    public static final  int Send                        = -8; // Send Key
    public static final  int CBA1                        = -9;  // Left CBA
    public static final  int CBA2                        = -10;  // Right CBA
    public static final  int End                         = -11; // End key
    public static final  int Applications                = -12; // Applications key
    public static final  int Voice                       = -13; // Voice key

    //non unicode non MIDP key codes
    public static final  int  Power      = -14;
    public static final  int  Backspace  = -15;
    public static final  int  GripOpen   = -16;
    public static final  int  GripClose  = -17;
    public static final  int  Camera     = -18;
    public static final  int  VolumeUp   = -19;
    public static final  int  VolumeDown = -20;
    public static final  int  PlayPause = -21; //Media key
    public static final  int  Stop = -22; //Media Key
    public static final  int  NextFF = -23; //Media key
    public static final  int  PreviousRew = -24; //Media key

    public static final  int  PageUp                     = -25;
    public static final  int  PageDown                   = -26;
    public static final  int  HomeKey                    = -27;
    public static final  int  EndKey                     = -28;
    // Activate key represents a key that will cause a "default selection" of an item. On Linux
    // this is Enter, but on S60 it may be the same key as Select.
    public static final  int  Activate                   = -29;
    public static final  int  InsertKey                  = -30;
    public static final  int  ReturnKey                  = -31;
    public static final  int  DeleteKey                  = -32;
    public static final  int  Escape                     = -33;



    //
    // SYMBIAN SCAN CODES - copied from e32keys.h
    //

    public static final int EStdKeyNull=0x00;                     /**< No key present. */
    public static final int EStdKeyBackspace=0x01;                /**< Scan code for Backspace key.*/
    public static final int EStdKeyTab=0x02;                      /**< Scan code for Tab key. */
    public static final int EStdKeyEnter=0x03;                    /**< Scan code for Enter key.*/
    public static final int EStdKeyEscape=0x04;                   /**< Scan code for Escape (Esc) key.*/
    public static final int EStdKeySpace=0x05;                    /**< Scan code for Space key.*/
    public static final int EStdKeyPrintScreen=0x06;              /**< Scan code for Print Screen key.*/
    public static final int EStdKeyPause=0x07;                    /**< Scan code for Pause key.*/
    public static final int EStdKeyHome=0x08;                     /**< Scan code for Home key.*/
    public static final int EStdKeyEnd=0x09;                      /**< Scan code for End key.*/
    public static final int EStdKeyPageUp=0x0a;                   /**< Scan code for Page Up key.*/
    public static final int EStdKeyPageDown=0x0b;                 /**< Scan code for Page Down key.*/
    public static final int EStdKeyInsert=0x0c;                   /**< Scan code for Insert key.*/
    public static final int EStdKeyDelete=0x0d;                   /**< Scan code for Delete (Del) key.*/
    public static final int EStdKeyLeftArrow=0x0e;                /**< Scan code for Left arrow key.*/
    public static final int EStdKeyRightArrow=0x0f;               /**< Scan code for Right arrow key.*/
    public static final int EStdKeyUpArrow=0x10;                  /**< Scan code for Up arrow key.*/
    public static final int EStdKeyDownArrow=0x11;                /**< Scan code for Down arrow key.*/
    public static final int EStdKeyLeftShift=0x12;                /**< Scan code for left Shift key.*/
    public static final int EStdKeyRightShift=0x13;               /**< Scan code for right Shift key.*/
    public static final int EStdKeyLeftAlt=0x14;                  /**< Scan code for left Alt key.*/
    public static final int EStdKeyRightAlt=0x15;                 /**< Scan code for right Alt key.*/
    public static final int EStdKeyLeftCtrl=0x16;                 /**< Scan code for left Control (Ctrl) key.*/
    public static final int EStdKeyRightCtrl=0x17;                /**< Scan code for right Control (Ctrl) key.*/
    public static final int EStdKeyLeftFunc=0x18;                 /**< Scan code for left Fn key.*/
    public static final int EStdKeyRightFunc=0x19;                /**< Scan code for right Fn key.*/
    public static final int EStdKeyCapsLock=0x1a;                 /**< Scan code for Caps lock key.*/
    public static final int EStdKeyNumLock=0x1b;                  /**< Scan code for Num lock key.*/
    public static final int EStdKeyScrollLock=0x1c;               /**< Scan code for Scroll lock key.*/
    public static final int EStdKeyF1=0x60;                       /**< Scan code for function key F1.*/
    public static final int EStdKeyF2=0x61;                       /**< Scan code for function key F2.*/
    public static final int EStdKeyF3=0x62;                       /**< Scan code for function key F3.*/
    public static final int EStdKeyF4=0x63;                       /**< Scan code for function key F4.*/
    public static final int EStdKeyF5=0x64;                       /**< Scan code for function key F5*/
    public static final int EStdKeyF6=0x65;                       /**< Scan code for function key F6*/
    public static final int EStdKeyF7=0x66;                       /**< Scan code for function key F7*/
    public static final int EStdKeyF8=0x67;                       /**< Scan code for function key F8*/
    public static final int EStdKeyF9=0x68;                       /**< Scan code for function key F9*/
    public static final int EStdKeyF10=0x69;                      /**< Scan code for function key F10*/
    public static final int EStdKeyF11=0x6a;                      /**< Scan code for function key F11*/
    public static final int EStdKeyF12=0x6b;                      /**< Scan code for function key F12*/
    public static final int EStdKeyF13=0x6c;                      /**< Scan code for function key F13*/
    public static final int EStdKeyF14=0x6d;                      /**< Scan code for function key F14*/
    public static final int EStdKeyF15=0x6e;                      /**< Scan code for function key F15*/
    public static final int EStdKeyF16=0x6f;                      /**< Scan code for function key F16*/
    public static final int EStdKeyF17=0x70;                      /**< Scan code for function key F17*/
    public static final int EStdKeyF18=0x71;                      /**< Scan code for function key F18*/
    public static final int EStdKeyF19=0x72;                      /**< Scan code for function key F19*/
    public static final int EStdKeyF20=0x73;                      /**< Scan code for function key F20*/
    public static final int EStdKeyF21=0x74;                      /**< Scan code for function key F21*/
    public static final int EStdKeyF22=0x75;                      /**< Scan code for function key F22.*/
    public static final int EStdKeyF23=0x76;                      /**< Scan code for function key F23.*/
    public static final int EStdKeyF24=0x77;                      /**< Scan code for function key F24.*/
    public static final int EStdKeyXXX=0x78;                      /**< Scan code for the key to the left of the 1 key on a standard keyboard.*/
    public static final int EStdKeyComma=0x79;                    /**< Scan code for Comma (,) key.*/
    public static final int EStdKeyFullStop=0x7a;                 /**< Scan code for Full stop (.) key.*/
    public static final int EStdKeyForwardSlash=0x7b;             /**< Scan code for Forward slash (/) key.*/
    public static final int EStdKeyBackSlash=0x7c;                /**< Scan code for Back slash (\) key.*/
    public static final int EStdKeySemiColon=0x7d;                /**< Scan code for Semi colon (;) key.*/
    public static final int EStdKeySingleQuote=0x7e;              /**< Scan code for Single quote (') key.*/
    public static final int EStdKeyHash=0x7f;                     /**< Scan code for Hash key (#) key.*/
    public static final int EStdKeySquareBracketLeft=0x80;        /**< Scan code for left Square bracket ([) key.*/
    public static final int EStdKeySquareBracketRight=0x81;       /**< Scan code for right Square bracket (]) key.*/
    public static final int EStdKeyMinus=0x82;                    /**< Scan code for Minus key (-) key.*/
    public static final int EStdKeyEquals=0x83;                   /**< Scan code for Equals key (=) key.*/
    public static final int EStdKeyNkpForwardSlash=0x84;          /**< Scan code for forward slash (/) key on the Numeric keypad.*/
    public static final int EStdKeyNkpAsterisk=0x85;              /**< Scan code for Asterisk (*) key on the Numeric keypad.*/
    public static final int EStdKeyNkpMinus=0x86;                 /**< Scan code for Minus (-) key on the Numeric keypad.*/
    public static final int EStdKeyNkpPlus=0x87;                  /**< Scan code for Plus (+) key on the Numeric keypad.*/
    public static final int EStdKeyNkpEnter=0x88;                 /**< Scan code for Enter key on the Numeric keypad.*/
    public static final int EStdKeyNkp1=0x89;                     /**< Scan code for the 1 key on the Numeric keypad.*/
    public static final int EStdKeyNkp2=0x8a;                     /**< Scan code for the 2 key on the Numeric keypad.*/
    public static final int EStdKeyNkp3=0x8b;                     /**< Scan code for the 3 key on the Numeric keypad.*/
    public static final int EStdKeyNkp4=0x8c;                     /**< Scan code for the 4 key on the Numeric keypad.*/
    public static final int EStdKeyNkp5=0x8d;                     /**< Scan code for the 5 key on the Numeric keypad.*/
    public static final int EStdKeyNkp6=0x8e;                     /**< Scan code for the 6 key on the Numeric keypad.*/
    public static final int EStdKeyNkp7=0x8f;                     /**< Scan code for the 7 key on the Numeric keypad.*/
    public static final int EStdKeyNkp8=0x90;                     /**< Scan code for the 8 key on the Numeric keypad.*/
    public static final int EStdKeyNkp9=0x91;                     /**< Scan code for the 9 key on the Numeric keypad.*/
    public static final int EStdKeyNkp0=0x92;                     /**< Scan code for the 0 key on the Numeric keypad.*/
    public static final int EStdKeyNkpFullStop=0x93;              /**< Scan code for Full stop (.) key on the Numeric keypad.*/
    public static final int EStdKeyMenu=0x94;                     /**< Scan code for Menu key.*/
    public static final int EStdKeyBacklightOn=0x95;              /**< Scan code for Backlight on key.*/
    public static final int EStdKeyBacklightOff=0x96;             /**< Scan code for Backlight off key.*/
    public static final int EStdKeyBacklightToggle=0x97;          /**< Scan code for Backlight toggle key.*/
    public static final int EStdKeyIncContrast=0x98;              /**< Scan code for Increase contrast key.*/
    public static final int EStdKeyDecContrast=0x99;              /**< Scan code for Decrease contrast key.*/
    public static final int EStdKeySliderDown=0x9a;               /**< Scan code for Slider down key.*/
    public static final int EStdKeySliderUp=0x9b;                 /**< Scan code for Slider up key.*/
    public static final int EStdKeyDictaphonePlay=0x9c;           /**< Scan code for Dictaphone play key.*/
    public static final int EStdKeyDictaphoneStop=0x9d;           /**< Scan code for Dictaphone stop key.*/
    public static final int EStdKeyDictaphoneRecord=0x9e;         /**< Scan code for Dictaphone record key.*/
    public static final int EStdKeyHelp=0x9f;                     /**< Scan code for Help key */
    public static final int EStdKeyOff=0xa0;                      /**< Scan code for Off key.*/
    public static final int EStdKeyDial=0xa1;                     /**< Scan code for Dial key.*/
    public static final int EStdKeyIncVolume=0xa2;                /**< Scan code for Increase volume key.*/
    public static final int EStdKeyDecVolume=0xa3;                /**< Scan code for Decrease volume key.*/
    public static final int EStdKeyDevice0=0xa4;                  /**< Device-specific scan code - usually represents an extra hardware key on the phone.*/
    public static final int EStdKeyDevice1=0xa5;                  /**< Device-specific scan code - usually represents an extra hardware key on the phone.*/
    public static final int EStdKeyDevice2=0xa6;                  /**< Device-specific scan code - usually represents an extra hardware key on the phone.*/
    public static final int EStdKeyDevice3=0xa7;                  /**< Device-specific scan code - usually represents an extra hardware key on the phone.*/
    public static final int EStdKeyDevice4=0xa8;                  /**< Device-specific scan code - usually represents an extra hardware key on the phone.*/
    public static final int EStdKeyDevice5=0xa9;                  /**< Device-specific scan code - usually represents an extra hardware key on the phone.*/
    public static final int EStdKeyDevice6=0xaa;                  /**< Device-specific scan code - usually represents an extra hardware key on the phone.*/
    public static final int EStdKeyDevice7=0xab;                  /**< Device-specific scan code - usually represents an extra hardware key on the phone.*/
    public static final int EStdKeyDevice8=0xac;                  /**< Device-specific scan code - usually represents an extra hardware key on the phone.*/
    public static final int EStdKeyDevice9=0xad;                  /**< Device-specific scan code - usually represents an extra hardware key on the phone.*/
    public static final int EStdKeyDeviceA=0xae;                  /**< Device-specific scan code - usually represents an extra hardware key on the phone.*/
    public static final int EStdKeyDeviceB=0xaf;                  /**< Device-specific scan code - usually represents an extra hardware key on the phone.*/
    public static final int EStdKeyDeviceC=0xb0;                  /**< Device-specific scan code - usually represents an extra hardware key on the phone.*/
    public static final int EStdKeyDeviceD=0xb1;                  /**< Device-specific scan code - usually represents an extra hardware key on the phone.*/
    public static final int EStdKeyDeviceE=0xb2;                  /**< Device-specific scan code - usually represents an extra hardware key on the phone.*/
    public static final int EStdKeyDeviceF=0xb3;                  /**< Device-specific scan code - usually represents an extra hardware key on the phone.*/
    public static final int EStdKeyApplication0=0xb4;             /**< Scan code for device-specific application launcher key.*/
    public static final int EStdKeyApplication1=0xb5;             /**< Scan code for device-specific application launcher key.*/
    public static final int EStdKeyApplication2=0xb6;             /**< Scan code for device-specific application launcher key.*/
    public static final int EStdKeyApplication3=0xb7;             /**< Scan code for device-specific application launcher key.*/
    public static final int EStdKeyApplication4=0xb8;             /**< Scan code for device-specific application launcher key.*/
    public static final int EStdKeyApplication5=0xb9;             /**< Scan code for device-specific application launcher key.*/
    public static final int EStdKeyApplication6=0xba;             /**< Scan code for device-specific application launcher key.*/
    public static final int EStdKeyApplication7=0xbb;             /**< Scan code for device-specific application launcher key.*/
    public static final int EStdKeyApplication8=0xbc;             /**< Scan code for device-specific application launcher key.*/
    public static final int EStdKeyApplication9=0xbd;             /**< Scan code for device-specific application launcher key.*/
    public static final int EStdKeyApplicationA=0xbe;             /**< Scan code for device-specific application launcher key.*/
    public static final int EStdKeyApplicationB=0xbf;             /**< Scan code for device-specific application launcher key.*/
    public static final int EStdKeyApplicationC=0xc0;             /**< Scan code for device-specific application launcher key.*/
    public static final int EStdKeyApplicationD=0xc1;             /**< Scan code for device-specific application launcher key.*/
    public static final int EStdKeyApplicationE=0xc2;             /**< Scan code for device-specific application launcher key.*/
    public static final int EStdKeyApplicationF=0xc3;             /**< Scan code for device-specific application launcher key.*/
    public static final int EStdKeyYes=0xc4;                      /**< Scan code for Yes key.*/
    public static final int EStdKeyNo=0xc5;                       /**< Scan code for No key.*/
    public static final int EStdKeyIncBrightness=0xc6;            /**< Scan code for Increase brightness key.*/
    public static final int EStdKeyDecBrightness=0xc7;            /**< Scan code for Decrease brightness key.*/
    public static final int EStdKeyKeyboardExtend=0xc8;           /**< Scan code for flip actuated when keypad extends to full keyboard.*/
	public static final int EStdKeyDevice10 = 0xc9;               /**< Diagonal LeftUpArrow.*/
	public static final int EStdKeyDevice11 = 0xca;               /**< Diagonal RightUpArrow.*/
	public static final int EStdKeyDevice12 = 0xcb;               /**< Diagonal RightDownArrow.*/
	public static final int EStdKeyDevice13 = 0xcc;               /**< Diagonal LeftDownArrow.*/

    //
    // SYMBIAN KEY CODES
    //

    public static final int ENonCharacterKeyBase = 0xf800;

    public static final int EKeyNull=0x0000;                     /**< Code for the Null key. */
    public static final int EKeyBell=0x0007;                     /**< Code for the Bell key*/
    public static final int EKeyBackspace=0x0008;                /**< Code for the Backspace key */
    public static final int EKeyTab=0x0009;                      /**< Code for the Tab key*/
    public static final int EKeyLineFeed=0x000a;                 /**< Code for the Linefeed key*/
    public static final int EKeyVerticalTab=0x000b;              /**< Code for the Vertical tab key*/
    public static final int EKeyFormFeed=0x000c;                 /**< Code for the Form feed key*/
    public static final int EKeyEnter=0x000d;                    /**< Code for the Enter key */
    public static final int EKeyEscape=0x001b;                   /**< Code for the Escape key */
    public static final int EKeySpace=0x0020;                    /**< Code for the Space key*/
    public static final int EKeyDelete=0x007f;                   /**< Code for the Delete (Del) key*/
    public static final int EKeyPrintScreen = ENonCharacterKeyBase;/**< Code for the Print screen key */
    public static final int EKeyPause = ENonCharacterKeyBase + 1;     /**< Code for the Pause key*/
    public static final int EKeyHome = ENonCharacterKeyBase + 2;                            /**< Code for the Home key*/
    public static final int EKeyEnd = ENonCharacterKeyBase + 3;                             /**< Code for the End key*/
    public static final int EKeyPageUp = ENonCharacterKeyBase + 4;                          /**< Code for the Page up key*/
    public static final int EKeyPageDown = ENonCharacterKeyBase + 5;                        /**< Code for the Page down key*/
    public static final int EKeyInsert = ENonCharacterKeyBase + 6;                          /**< Code for the Insert key*/
    public static final int EKeyLeftArrow = ENonCharacterKeyBase + 7;                       /**< Code for the Left arrow key*/
    public static final int EKeyRightArrow = ENonCharacterKeyBase + 8;                      /**< Code for the Right arrow key*/
    public static final int EKeyUpArrow = ENonCharacterKeyBase + 9;                         /**< Code for the Up arrow key*/
    public static final int EKeyDownArrow = ENonCharacterKeyBase + 10;                       /**< Code for the Down arrow key*/
    public static final int EKeyLeftShift = ENonCharacterKeyBase + 11;                       /**< Code for the left Shift key*/
    public static final int EKeyRightShift = ENonCharacterKeyBase + 12;                      /**< Code for the right Shift key*/
    public static final int EKeyLeftAlt = ENonCharacterKeyBase + 13;                         /**< Code for the left Alt key*/
    public static final int EKeyRightAlt = ENonCharacterKeyBase + 14;                        /**< Code for the right Alt key*/
    public static final int EKeyLeftCtrl = ENonCharacterKeyBase + 15;                        /**< Code for the left Control (Ctrl) key*/
    public static final int EKeyRightCtrl = ENonCharacterKeyBase + 16;                       /**< Code for the right Control (Ctrl) key.*/
    public static final int EKeyLeftFunc = ENonCharacterKeyBase + 17;                        /**< Code for the left Fn key.*/
    public static final int EKeyRightFunc = ENonCharacterKeyBase + 18;                       /**< Code for the right Fn key.*/
    public static final int EKeyCapsLock = ENonCharacterKeyBase + 19;                        /**< Code for the Caps lock key.*/
    public static final int EKeyNumLock = ENonCharacterKeyBase + 20;                         /**< Code for the Num lock key.*/
    public static final int EKeyScrollLock = ENonCharacterKeyBase + 21;                      /**< Code for the Scroll lock key.*/
    public static final int EKeyF1 = ENonCharacterKeyBase + 22;                              /**< Code for the F1 function key.*/
    public static final int EKeyF2 = ENonCharacterKeyBase + 23;                              /**< Code for the F2 function key.*/
    public static final int EKeyF3 = ENonCharacterKeyBase + 24;                              /**< Code for the F3 function key.*/
    public static final int EKeyF4 = ENonCharacterKeyBase + 25;                              /**< Code for the F4 function key.*/
    public static final int EKeyF5 = ENonCharacterKeyBase + 26;                              /**< Code for the F5 function key.*/
    public static final int EKeyF6 = ENonCharacterKeyBase + 27;                              /**< Code for the F6 function key.*/
    public static final int EKeyF7 = ENonCharacterKeyBase + 28;                              /**< Code for the F7 function key.*/
    public static final int EKeyF8 = ENonCharacterKeyBase + 29;                              /**< Code for the F8 function key.*/
    public static final int EKeyF9 = ENonCharacterKeyBase + 30;                              /**< Code for the F9 function key.*/
    public static final int EKeyF10 = ENonCharacterKeyBase + 31;                             /**< Code for the F10 function key.*/
    public static final int EKeyF11 = ENonCharacterKeyBase + 21;                             /**< Code for the F11 function key.*/
    public static final int EKeyF12 = ENonCharacterKeyBase + 33;                             /**< Code for the F12 function key.*/
    public static final int EKeyF13 = ENonCharacterKeyBase + 34;                             /**< Code for the F13 function key.*/
    public static final int EKeyF14 = ENonCharacterKeyBase + 35;                             /**< Code for the F14 function key.*/
    public static final int EKeyF15 = ENonCharacterKeyBase + 36;                             /**< Code for the F15 function key.*/
    public static final int EKeyF16 = ENonCharacterKeyBase + 37;                             /**< Code for the F16 function key.*/
    public static final int EKeyF17 = ENonCharacterKeyBase + 38;                             /**< Code for the F17 function key.*/
    public static final int EKeyF18 = ENonCharacterKeyBase + 39;                             /**< Code for the F18 function key.*/
    public static final int EKeyF19 = ENonCharacterKeyBase + 40;                             /**< Code for the F19 function key.*/
    public static final int EKeyF20 = ENonCharacterKeyBase + 41;                             /**< Code for the F20 function key.*/
    public static final int EKeyF21 = ENonCharacterKeyBase + 42;                             /**< Code for the F21 function key.*/
    public static final int EKeyF22 = ENonCharacterKeyBase + 43;                             /**< Code for the F22 function key.*/
    public static final int EKeyF23 = ENonCharacterKeyBase + 44;                             /**< Code for the F23 function key.*/
    public static final int EKeyF24 = ENonCharacterKeyBase + 45;                             /**< Code for the F24 function key.*/
    public static final int EKeyOff = ENonCharacterKeyBase + 46;                             /**< Code for the Off key.*/
    public static final int EKeyIncContrast = ENonCharacterKeyBase + 47;                     /**< Code for the Increase contrast key.*/
    public static final int EKeyDecContrast = ENonCharacterKeyBase + 48;                     /**< Code for the Decrease contrast key.*/
    public static final int EKeyBacklightOn = ENonCharacterKeyBase + 49;                     /**< Code for the Backlight on key.*/
    public static final int EKeyBacklightOff = ENonCharacterKeyBase + 50;                    /**< Code for the Backlight off key.*/
    public static final int EKeyBacklightToggle = ENonCharacterKeyBase + 51;                 /**< Code for the Backlight toggle key.*/
    public static final int EKeySliderDown = ENonCharacterKeyBase + 52;                      /**< Code for the Slider down key.*/
    public static final int EKeySliderUp = ENonCharacterKeyBase + 53;                        /**< Code for the Slider up key.*/
    public static final int EKeyMenu = ENonCharacterKeyBase + 54;                            /**< Code for the Menu key.*/
    public static final int EKeyDictaphonePlay = ENonCharacterKeyBase + 55;                  /**< Code for the Dictaphone play key.*/
    public static final int EKeyDictaphoneStop = ENonCharacterKeyBase + 56;                  /**< Code for the Dictaphone stop key.*/
    public static final int EKeyDictaphoneRecord = ENonCharacterKeyBase + 57;                /**< Code for the Dictaphone record key.*/
    public static final int EKeyHelp = ENonCharacterKeyBase + 58;                            /**< Code for the Help key.*/
    public static final int EKeyDial = ENonCharacterKeyBase + 59;                            /**< Code for the Dial key.*/
    public static final int EKeyScreenDimension0 = ENonCharacterKeyBase + 60;                /**< Code for the first Screen dimension change key.*/
    public static final int EKeyScreenDimension1 = ENonCharacterKeyBase + 61;                /**< Code for the second Screen dimension change key.*/
    public static final int EKeyScreenDimension2 = ENonCharacterKeyBase + 62;                /**< Code for the third Screen dimension change key.*/
    public static final int EKeyScreenDimension3 = ENonCharacterKeyBase + 63;                /**< Code for the fourth Screen dimension change key.*/
    public static final int EKeyIncVolume = ENonCharacterKeyBase + 64;                       /**< Code for the increase colume key.*/
    public static final int EKeyDecVolume = ENonCharacterKeyBase + 65;                       /**< Code for the decrease volume key.*/
    public static final int EKeyDevice0 = ENonCharacterKeyBase + 66;                         /**< Code for a device specific key.*/
    public static final int EKeyDevice1 = ENonCharacterKeyBase + 67;                         /**< Code for a device specific key.*/
    public static final int EKeyDevice2 = ENonCharacterKeyBase + 68;                         /**< Code for a device specific key. */
    public static final int EKeyDevice3 = ENonCharacterKeyBase + 69;                         /**< Code for a device specific key.*/
    public static final int EKeyDevice4 = ENonCharacterKeyBase + 70;                         /**< Code for a device specific key.*/
    public static final int EKeyDevice5 = ENonCharacterKeyBase + 71;                         /**< Code for a device specific key.*/
    public static final int EKeyDevice6 = ENonCharacterKeyBase + 72;                         /**< Code for a device specific key.*/
    public static final int EKeyDevice7 = ENonCharacterKeyBase + 73;                         /**< Code for a device specific key.*/
    public static final int EKeyDevice8 = ENonCharacterKeyBase + 74;                         /**< Code for a device specific key.*/
    public static final int EKeyDevice9 = ENonCharacterKeyBase + 75;                         /**< Code for a device specific key.*/
    public static final int EKeyDeviceA = ENonCharacterKeyBase + 76;                         /**< Code for a device specific key.*/
    public static final int EKeyDeviceB = ENonCharacterKeyBase + 77;                         /**< Code for a device specific key.*/
    public static final int EKeyDeviceC = ENonCharacterKeyBase + 78;                         /**< Code for a device specific key.*/
    public static final int EKeyDeviceD = ENonCharacterKeyBase + 79;                         /**< Code for a device specific key.*/
    public static final int EKeyDeviceE = ENonCharacterKeyBase + 80;                         /**< Code for a device specific key.*/
    public static final int EKeyDeviceF = ENonCharacterKeyBase + 81;                         /**< Code for a device specific key.*/
    public static final int EKeyApplication0 = ENonCharacterKeyBase + 82;                    /**< Code for an Application launcher key.*/
    public static final int EKeyApplication1 = ENonCharacterKeyBase + 83;                    /**< Code for an Application launcher key.*/
    public static final int EKeyApplication2 = ENonCharacterKeyBase + 84;                    /**< Code for an Application launcher key.*/
    public static final int EKeyApplication3 = ENonCharacterKeyBase + 85;                    /**< Code for an Application launcher key.*/
    public static final int EKeyApplication4 = ENonCharacterKeyBase + 86;                    /**< Code for an Application launcher key.*/
    public static final int EKeyApplication5 = ENonCharacterKeyBase + 87;                    /**< Code for an Application launcher key.*/
    public static final int EKeyApplication6 = ENonCharacterKeyBase + 88;                    /**< Code for an Application launcher key.*/
    public static final int EKeyApplication7 = ENonCharacterKeyBase + 89;                    /**< Code for an Application launcher key.*/
    public static final int EKeyApplication8 = ENonCharacterKeyBase + 90;                    /**< Code for an Application launcher key.*/
    public static final int EKeyApplication9 = ENonCharacterKeyBase + 91;                    /**< Code for an Application launcher key.*/
    public static final int EKeyApplicationA = ENonCharacterKeyBase + 92;                    /**< Code for an Application launcher key.*/
    public static final int EKeyApplicationB = ENonCharacterKeyBase + 93;                    /**< Code for an Application launcher key.*/
    public static final int EKeyApplicationC = ENonCharacterKeyBase + 94;                    /**< Code for an Application launcher key.*/
    public static final int EKeyApplicationD = ENonCharacterKeyBase + 95;                    /**< Code for an Application launcher key.*/
    public static final int EKeyApplicationE = ENonCharacterKeyBase + 96;                    /**< Code for an Application launcher key.*/
    public static final int EKeyApplicationF = ENonCharacterKeyBase + 97;                    /**< Code for an Application launcher key.*/
    public static final int EKeyYes = ENonCharacterKeyBase + 98;                             /**< Code for the Yes key.*/
    public static final int EKeyNo = ENonCharacterKeyBase + 99;                              /**< Code for the No key.*/
    public static final int EKeyIncBrightness = ENonCharacterKeyBase + 100;                   /**< Code for the increase brightness key.*/
    public static final int EKeyDecBrightness = ENonCharacterKeyBase + 101;                   /**< Code for the decrease brightness key. */
    public static final int EKeyKeyboardExtend = ENonCharacterKeyBase + 102;                  /**< Code for flip actuated when keypad extends to full keyboard.*/
	public static final int EKeyDevice10 = ENonCharacterKeyBase + 103;                         /**< Diagonal LeftUpArrow.*/
	public static final int EKeyDevice11 = ENonCharacterKeyBase + 104;                         /**< Diagonal RightUpArrow.*/
	public static final int EKeyDevice12 = ENonCharacterKeyBase + 105;                         /**< Diagonal RightDownArrow.*/
	public static final int EKeyDevice13 = ENonCharacterKeyBase + 106;                         /**< Diagonal LeftDownArrow.*/

    /** map from logical key codes to symbian key codes
     *  (index is logical key codes above with opposite sign)
     * */
    private int logicalKeyCodes[] = {
            EKeyUpArrow, //UpArrow
            EKeyDownArrow,//DownArrow
            EKeyLeftArrow,//LeftArrow
            EKeyRightArrow,//RightArrow
            EStdKeyDelete, //Clear
            EKeyDevice3, //Select
            EKeyRightShift, //Edit
            EKeyYes, //Send
            EKeyDevice0, //CBA1
            EKeyDevice1, //CBA2
            EKeyNo, //End
            EKeyApplication0, //Applications
            EKeyDevice6, //Voice
            EKeyDevice7, //Power
            EKeyDelete, //Backspace
            EKeySliderDown, //GripOpen
            EKeySliderUp,   //GripClose
            EKeyDevice7, //Camera
            EKeyIncVolume, //VolumeUp
            EKeyDecVolume, //VolumeDown
            EKeyApplication2, //PlayPause
            EKeyApplication3, //Stop
            EKeyApplication4, //NextFF
            EKeyApplication5, //PreviousRew
            EKeyPageUp, // PageUp
            EKeyPageDown, // PageDown
            EKeyHome, // HomeKey
            EKeyEnd, // EndKey
            EKeyDevice3, // Activate
            EKeyInsert, // InsertKey
            EKeyEnter, // ReturnKey
            EKeyDelete, // DeleteKey
            EKeyEscape, // Escape
			EKeyDevice10, //Diagonal LeftUpArrow
			EKeyDevice11, //Diagonal RightUpArrow
			EKeyDevice12, //Diagonal RightDownArrow
			EKeyDevice13 //Diagonal LeftDownArrow
    };

    /** map from logical key codes to symbian scan codes
     * (index is logical key codes above with opposite sign)
     * */
    private int logicalScanCodes[] = {
            EStdKeyUpArrow, //UpArrow
            EStdKeyDownArrow, //DownArrow
            EStdKeyLeftArrow, //LeftArrow
            EStdKeyRightArrow, //RightArrow
            EStdKeyBackspace, //Clear
            EStdKeyDevice3, //Select
            EStdKeyRightShift, //Edit
            EStdKeyYes, //Send
            EStdKeyDevice0, //CBA1
            EStdKeyDevice1, //CBA2
            EStdKeyNo, //End
            EStdKeyApplication0, //Applications
            EStdKeyDevice6, //Voice
            EStdKeyDevice2, //Power
            EStdKeyBackspace, //Backspace
            EStdKeyNull, //GripOpen  //TODO
            EStdKeyNull, //GripClose    //TODO
            EStdKeyDevice7, //Camera
            EStdKeyIncVolume, //VolumeUp
            EStdKeyDecVolume, //VolumeDown
            EStdKeyApplication2, //PlayPause
            EStdKeyApplication3, //Stop
            EStdKeyApplication4, //NextFF
            EStdKeyApplication5, //PreviousRew
            EStdKeyPageUp,// PageUp
            EStdKeyPageDown,// PageDown
            EStdKeyHome,// HomeKey
            EStdKeyEnd,// EndKey
            EStdKeyDevice3,// Activate
            EStdKeyInsert,// InsertKey
            EStdKeyEnter,// ReturnKey
            EStdKeyDelete,// DeleteKey
            EStdKeyEscape,// Escape
			EStdKeyDevice10, //Diagonal LeftUpArrow
			EStdKeyDevice11, //Diagonal RightUpArrow
			EStdKeyDevice12, //Diagonal RightDownArrow
			EStdKeyDevice13 //Diagonal LeftDownArrow


    };

    /** Default constructor */
    public Key() {
        this.keyCode = 0;
        this.scanCode = 0;
    }

    /** Constructor accepting symbian key and scan codes defined above */
    public Key(int keyCode, int scanCode) {
        this.keyCode = keyCode;
        this.scanCode = scanCode;
    }

    /** Constructor accepting unicode or logical key codes defined above */
    public Key(int keyCode) {
    //JDEBUG("Key: Converting logical code " + keyCode);

        if (keyCode < 0) {
            this.keyCode = logicalKeyCodes[-keyCode -1];
            this.scanCode = logicalScanCodes[-keyCode -1];
        } else if (keyCode >= 'a' && keyCode <= 'z') {
            this.keyCode = keyCode;
            this.scanCode = keyCode - 'a' + 'A';
        } else if (keyCode == '*') {
            this.keyCode = keyCode;
            this.scanCode = EStdKeyNkpAsterisk;

        } else if (keyCode == '#') {
            this.keyCode = keyCode;
            this.scanCode = EStdKeyHash;
        }  else if (keyCode == '/' || keyCode == '?') {
            this.keyCode = keyCode;
            this.scanCode = EStdKeyForwardSlash;
        }
        else if (keyCode == EKeyBackspace) {
            this.keyCode = keyCode;
            this.scanCode = EStdKeyBackspace;
        }
        else if (keyCode == EKeyEnter) {
            this.keyCode = keyCode;
            this.scanCode = EStdKeyEnter;
        }
        else if (keyCode == EKeyEscape) {
            this.keyCode = keyCode;
            this.scanCode = EStdKeyEscape;
        }
        else if (keyCode == EKeyDelete) {
            this.keyCode = keyCode;
            this.scanCode = EStdKeyDelete;
        }
        else if (keyCode == EKeyTab) {
            this.keyCode = keyCode;
            this.scanCode = EStdKeyTab;
        }
        else if (keyCode == '-' || keyCode == '_') {
            this.keyCode = keyCode;
            this.scanCode = EStdKeyMinus;
        }
        else if (keyCode == '=') {
            this.keyCode = keyCode;
            this.scanCode = EStdKeyEquals;
        }
        else if (keyCode == '+') {
            this.keyCode = keyCode;
            this.scanCode = EStdKeyNkpPlus;
        }
        else if (keyCode == ';' || keyCode == ':') {
            this.keyCode = keyCode;
            this.scanCode = EStdKeySemiColon;
        }
        else if (keyCode == '\'' || keyCode == '\"' ) {
            this.keyCode = keyCode;
            this.scanCode = EStdKeySingleQuote;
        }
        else if (keyCode >= '0' && keyCode <= '9') {
            this.keyCode = keyCode;
            this.scanCode = keyCode;
        }
        else if (keyCode == EKeySpace) {
            this.keyCode = keyCode;
            this.scanCode = EStdKeySpace;
        }
        else if (keyCode == DownArrow) {
            this.keyCode = keyCode;
            this.scanCode = EStdKeyDownArrow;
        }
        else if (keyCode == UpArrow) {
            this.keyCode = keyCode;
            this.scanCode = EStdKeyUpArrow;
        }
        else if (keyCode == LeftArrow) {
            this.keyCode = keyCode;
            this.scanCode = EStdKeyLeftArrow;
        }
        else if (keyCode == RightArrow) {
            this.keyCode = keyCode;
            this.scanCode = EStdKeyRightArrow;
        }
        else if (keyCode == ',') {
            this.keyCode = keyCode;
            this.scanCode = EStdKeyComma;
        }
        else if (keyCode == '.') {
            this.keyCode = keyCode;
            this.scanCode = EStdKeyFullStop;
        } else {
            this.keyCode = keyCode;
            this.scanCode = keyCode;
        }
    }

    /** The symbian key code */
    public int keyCode;

    /** The symbian scan code */
    public int scanCode;

    /** Modifiers, for compatibility with XlibUtils **/
    public int modifiers;
}