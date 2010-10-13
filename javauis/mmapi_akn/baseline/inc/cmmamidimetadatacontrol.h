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
* Description:  CMMAMIDIMetaDataControl is a concrete class for getting
*                metadata from midi engine.
*
*/


#ifndef CMMAMIDIMETADATACONTROL_H
#define CMMAMIDIMETADATACONTROL_H

//  INCLUDES
#include <mmfcontroller.h>

#include "cmmamidiplayer.h"
#include "cmmametadatacontrol.h"

//  CLASS DECLARATION
/**
*   This is a concrete class for getting metadata from midi engine.
*
*
*/

NONSHARABLE_CLASS(CMMAMIDIMetaDataControl): public CMMAMetaDataControl
{
public:
    CMMAMIDIMetaDataControl(CMMAMIDIPlayer* aPlayer);

protected: // from CMMAMetaDataControl

    TInt KeyCountL();
    HBufC* KeyL(TInt aIndex);

    HBufC* KeyValueL(const TDesC& aKey);

private:
    /**
     * Used to query metadata
     */
    CMMAMIDIPlayer* iPlayer;
};

#endif // CMMAMIDIMETADATACONTROL_H
