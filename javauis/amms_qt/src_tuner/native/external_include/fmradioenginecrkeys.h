/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Definition of central repository keys for FM Radio Engine.
*
*/


#ifndef FMRADIOENGINECRKEYS_H
#define FMRADIOENGINECRKEYS_H

// FM Radio Engine UID
const TUid KCRUidFMRadioEngine = {0x101F7CBC};

// Radio headset volume
const TUint32 KRadioHeadsetVolume =  0x00000001;

// Radio speaker volume
const TUint32 KRadioSpeakerVolume = 0x00000002;

// The last played radio frequency
const TUint32 KFmRadioFrequency = 0x00000003;

// The last played radio channel
const TUint32 KFmRadioChannel = 0x00000004;

// The radio mode before radio off
const TUint32 KFmRadioMode = 0x00000005;

// The first preset channel's location in central repository
const TUint32 KFmRadioPresetChannel1 = 0x0000000A;

#endif // FMRADIOENGINECRKEYS_H

// End of file


