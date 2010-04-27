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
* Description:  This class is used for setting volume to midi player
*
*/


#ifndef CMMAMIDIVOLUMECONTROL_H
#define CMMAMIDIVOLUMECONTROL_H

#include <mmf/common/mmfstandardcustomcommands.h>
#include "cmmavolumecontrol.h"

class CMMAMIDIPlayer;

//  CLASS DEFINITION
/*
-----------------------------------------------------------------------------

    DESCRIPTION
    This class is used for setting volume to audio player

-----------------------------------------------------------------------------
*/
NONSHARABLE_CLASS(CMMAMIDIVolumeControl): public CMMAVolumeControl
{
public:
    static CMMAMIDIVolumeControl* NewL(CMMAMIDIPlayer* aPlayer);

protected:
    CMMAMIDIVolumeControl(CMMAMIDIPlayer* aPlayer);
    void ConstructL();


public: // from CMMAVolumeControl
    void DoSetLevelL(TInt aLevel);
    TInt DoGetLevelL();

private:
    CMMAMIDIPlayer* iPlayer;
};

#endif // CMMAMIDIVOLUMECONTROL_H
