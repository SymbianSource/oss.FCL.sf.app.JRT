/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CMMADRMVOLUMECONTROL_H
#define CMMADRMVOLUMECONTROL_H

#include "cmmavolumecontrol.h"

class CMMADRMAudioPlayer;

//  CLASS DEFINITION
/*
-----------------------------------------------------------------------------

    DESCRIPTION
    This class is used for setting volume to DRM audio player

-----------------------------------------------------------------------------
*/
NONSHARABLE_CLASS(CMMADRMVolumeControl): public CMMAVolumeControl
{
public:
    static CMMADRMVolumeControl* NewL(CMMADRMAudioPlayer* aPlayer);

protected:
    CMMADRMVolumeControl(CMMADRMAudioPlayer* aPlayer);
    void ConstructL();


public: // from CMMAVolumeControl
    void DoSetLevelL(TInt aLevel);
    TInt DoGetLevelL();

private:
    CMMADRMAudioPlayer* iPlayer;
};

#endif // CMMADRMVOLUMECONTROL_H
