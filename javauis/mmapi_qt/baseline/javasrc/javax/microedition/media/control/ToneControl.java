/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Please refer JSR 135 for more details.
*
*/


package javax.microedition.media.control;

public interface ToneControl extends javax.microedition.media.Control
{
    byte VERSION = -2;

    byte TEMPO = -3;

    byte RESOLUTION = -4;

    byte BLOCK_START = -5;

    byte BLOCK_END = -6;

    byte PLAY_BLOCK = -7;

    byte SET_VOLUME = -8;

    byte REPEAT = -9;

    byte C4 = 60;

    byte SILENCE = -1;

    void setSequence(byte[] aSequence);
}
