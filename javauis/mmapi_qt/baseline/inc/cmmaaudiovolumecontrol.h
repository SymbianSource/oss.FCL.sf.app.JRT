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
* Description:  This class is used for setting volume to audio player
*
*/


#ifndef CMMAAUDIOVOLUMECONTROL_H
#define CMMAAUDIOVOLUMECONTROL_H

#include <mmf/common/mmfstandardcustomcommands.h>
#include "cmmavolumecontrol.h"
//#include <jutils.h>
class CMMAAudioPlayer;

//  CLASS DEFINITION
/*
-----------------------------------------------------------------------------

    DESCRIPTION
    This class is used for setting volume to audio player

-----------------------------------------------------------------------------
*/

NONSHARABLE_CLASS(CMMAAudioVolumeControl): public CMMAVolumeControl
{
public:
    IMPORT_C static CMMAAudioVolumeControl* NewL(CMMAAudioPlayer* aPlayer);

protected:
    CMMAAudioVolumeControl(CMMAAudioPlayer* aPlayer);
    void ConstructL();

public: // from CMMAVolumeControl
    void DoSetLevelL(TInt aLevel);
    TInt DoGetLevelL();

private:
    RMMFAudioPlayDeviceCustomCommands iAudioPlayDeviceCommands;
};

#endif // CMMAAUDIOVOLUMECONTROL_H
