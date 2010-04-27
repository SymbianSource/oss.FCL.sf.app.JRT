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
* Description:  This interface is for getting metadata from a media.
*
*/


//  INCLUDE FILES
#include "cmmametadatacontrol.h"


_LIT(KControlName, "MetaDataControl");


CMMAMetaDataControl::CMMAMetaDataControl()
{
    // Nothing, OK.
}

void CMMAMetaDataControl::StaticKeysCountL(CMMAMetaDataControl* aMetaDataControl, TInt *aCount)
{
    *aCount = aMetaDataControl->KeyCountL();
}

void CMMAMetaDataControl::StaticKeyL(CMMAMetaDataControl* aMetaDataControl, HBufC** aKey, TInt aIndex)
{
    *aKey = aMetaDataControl->KeyL(aIndex);
}

void CMMAMetaDataControl::StaticKeyValueL(
    CMMAMetaDataControl* aMetaDataControl,
    HBufC** aValue,
    TDesC* aKey)
{
    /* Just pass the call through to the real implementation. The ownership of the returned data
     * is received from the implementation and passed to the caller as well.
     *
     */
    *aValue = aMetaDataControl->KeyValueL(*aKey);
}

const TDesC& CMMAMetaDataControl::ClassName() const
{
    return KControlName;
}

//  END OF FILE
