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
* Description:  This class is used for stoptime controlling
*
*/


#ifndef CMMAMIDISTOPTIMECONTROL_H
#define CMMAMIDISTOPTIMECONTROL_H

//  INCLUDES
#include "cmmastoptimecontrol.h"

//  CLASS DECLARATION
/**
*   This class is used for stoptime controlling for MIDI
*
*
*/

NONSHARABLE_CLASS(CMMAMIDIStopTimeControl): public CMMAStopTimeControl
{
public:
    static CMMAMIDIStopTimeControl* NewL(CMMAPlayer* aPlayer);
    ~CMMAMIDIStopTimeControl();

protected:
    CMMAMIDIStopTimeControl(CMMAPlayer* aPlayer);

public: // from CMMAStopTimeControl
    virtual void StopAtTimeL();
};

#endif // CMMAMIDISTOPTIMECONTROL_H
