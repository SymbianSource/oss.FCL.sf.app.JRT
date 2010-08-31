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


#ifndef CMMAEMCAUDIOVOLUMECONTROL_H
#define CMMAEMCAUDIOVOLUMECONTROL_H

#include <mmf/common/mmfstandardcustomcommands.h>
#include "VolumeControl.h"
#include "cmmavolumecontrol.h"

using multimedia::MVolumeControl;

class CMMAEMCAudioPlayer;

//  CLASS DEFINITION
/*
-----------------------------------------------------------------------------

    DESCRIPTION
    This class is used for setting volume to audio player

-----------------------------------------------------------------------------
*/

NONSHARABLE_CLASS(CMMAEMCAudioVolumeControl): public CMMAVolumeControl
{
public: // constructors and destructors
    IMPORT_C static CMMAEMCAudioVolumeControl* NewL(CMMAEMCAudioPlayer& aPlayer);

protected:
    IMPORT_C CMMAEMCAudioVolumeControl(CMMAEMCAudioPlayer& aPlayer);
    void ConstructL();

    ~CMMAEMCAudioVolumeControl();

public: // from CMMAVolumeControl
    IMPORT_C void DoSetLevelL(TInt aLevel);
    IMPORT_C TInt DoGetLevelL();

private: // data
    MVolumeControl* iVolCntrl;

    // not owned
    CMMAEMCAudioPlayer& iPlayer;
};

#endif // CMMAEMCAUDIOVOLUMECONTROL_H
