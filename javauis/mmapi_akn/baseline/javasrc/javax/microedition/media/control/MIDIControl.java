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

import javax.microedition.media.MediaException;

public interface MIDIControl extends javax.microedition.media.Control
{
    int NOTE_ON = 0x90;  // 144

    int CONTROL_CHANGE = 0xB0;  // 176

    boolean isBankQuerySupported();

    int[] getProgram(int aChannel)
    throws MediaException;

    int getChannelVolume(int aChannel);

    void setProgram(int aChannel, int aBank, int aProgram);

    void setChannelVolume(int aChannel, int aVolume);

    int[] getBankList(boolean aCustom)
    throws MediaException;

    int[] getProgramList(int aBank)
    throws MediaException;

    String getProgramName(int aBank, int aProg)
    throws MediaException;

    String getKeyName(int aBank, int aProg, int aKey)
    throws MediaException;

    void shortMidiEvent(int aType, int aData1, int aData2);

    int longMidiEvent(byte[] aData, int aOffset, int aLength);
}
