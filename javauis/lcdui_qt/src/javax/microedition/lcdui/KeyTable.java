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
package javax.microedition.lcdui;

import org.eclipse.swt.SWT;

/**
 * Key table.
 */
final class KeyTable {

    static final int[][] KEY_TO_GAME_TABLE = {
        {-1,  Canvas.UP},
        {50,  Canvas.UP},
        {SWT.ARROW_UP,  Canvas.UP},

        {-2,  Canvas.DOWN},
        {56,  Canvas.DOWN},
        {SWT.ARROW_DOWN,  Canvas.DOWN},

        {-3,  Canvas.LEFT},
        {52,  Canvas.LEFT},
        {SWT.ARROW_LEFT,  Canvas.LEFT},

        {-4,  Canvas.RIGHT},
        {54,  Canvas.RIGHT},
        {SWT.ARROW_RIGHT,  Canvas.RIGHT},

        {-5,  Canvas.FIRE},
        {53,  Canvas.FIRE},
        {-10, Canvas.FIRE},

        {55,  Canvas.GAME_A},
        {57,  Canvas.GAME_B},
        {42,  Canvas.GAME_C},
        {35,  Canvas.GAME_D},
    };

    /**
     * Private constructor.
     */
    private KeyTable() {
    }

    static int getGameAction(int keyCode) {
        for (int i = 0; i < KEY_TO_GAME_TABLE.length; i++) {
            if (KEY_TO_GAME_TABLE[i][0] == keyCode) {
                return KEY_TO_GAME_TABLE[i][1];
            }
        }
        throw new IllegalArgumentException(
                MsgRepository.CANVAS_EXCEPTION_INVALID_KEY_CODE);
    }

    static int getKeyCode(int gameAction) {
        for (int i = 0; i < KEY_TO_GAME_TABLE.length; i++) {
            if (KEY_TO_GAME_TABLE[i][1] == gameAction) {
                return KEY_TO_GAME_TABLE[i][0];
            }
        }
        throw new IllegalArgumentException(
                MsgRepository.CANVAS_EXCEPTION_INVALID_GAME_ACTION);
    }


    static String getKeyName(int keyCode) {

        switch(keyCode) {
            /*case Canvas.KEY_NUM0:
                return "0";

            case Canvas.KEY_NUM1:
                return "1";

            case Canvas.KEY_NUM2:
                return "2";

            case Canvas.KEY_NUM3:
                return "3";

            case Canvas.KEY_NUM4:
                return "4";

            case Canvas.KEY_NUM5:
                return "5";

            case Canvas.KEY_NUM6:
                return "6";

            case Canvas.KEY_NUM7:
                return "7";

            case Canvas.KEY_NUM8:
                return "8";

            case Canvas.KEY_NUM9:
                return "9";


            case Canvas.KEY_STAR:
                return "*";

            case Canvas.KEY_POUND:
                return "#";
                */
            case -1:
                return OpenLcduiLocalization.getMessage("key_up");

            case -2:
                return OpenLcduiLocalization.getMessage("key_down");
                
            case -3:
                return OpenLcduiLocalization.getMessage("key_left");

            case -4:
                return OpenLcduiLocalization.getMessage("key_right");

            case -5:
                return OpenLcduiLocalization.getMessage("key_select");

            default:
                return "";
        }
    }

}
