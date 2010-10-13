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
* Description:  CMMAAudioMetaDataControl is a concrete class for getting
*                metadata from an audio media.
*
*/


#ifndef CMMAAUDIOMETADATACONTROL_H
#define CMMAAUDIOMETADATACONTROL_H

//  INCLUDES
#include <mmfcontroller.h>

#include "cmmametadatacontrol.h"

//  CLASS DECLARATION
/**
*   This is a concrete class for getting metadata from an audio media.
*
*
*/

NONSHARABLE_CLASS(CMMAAudioMetaDataControl): public CMMAMetaDataControl
{
public:
    CMMAAudioMetaDataControl(RMMFController& aController);

protected: // from CMMAMetaDataControl

    TInt KeyCountL();
    HBufC* KeyL(TInt aIndex);

    HBufC* KeyValueL(const TDesC& aKey);

private:
    RMMFController& iController;
};

#endif // CMMAAUDIOMETADATACONTROL_H
