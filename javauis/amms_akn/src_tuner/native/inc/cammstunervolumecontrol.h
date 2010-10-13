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
* Description:  This class is used for setting volume to tuner player
*
*/


#ifndef CAMMSTUNERVOLUMECONTROL_H
#define CAMMSTUNERVOLUMECONTROL_H

//  INCLUDES
#include <e32base.h>

#include <CMMAVolumeControl.h>

// FORWARD DECLARATIONS
class CAMMSTunerPlayer;

// CLASS DECLARATION
/**
*  This class is used for setting volume to tuner player
*/

NONSHARABLE_CLASS(CAMMSTunerVolumeControl) : public CMMAVolumeControl
{
public:  // Constructor
    static CAMMSTunerVolumeControl* NewL(CAMMSTunerPlayer* aTunerPlayer);

public: // functions from base classes

    // from CMMAVolumeControl

    void DoSetLevelL(TInt aLevel);

private:

    /**
    * C++ default constructor.
    */
    CAMMSTunerVolumeControl(CAMMSTunerPlayer* aTunerPlayer);


private:    // Data

    CAMMSTunerPlayer* iTunerPlayer;
};

#endif // CAMMSTUNERVOLUMECONTROL_H

// End of File
