/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Accelerometer sensor implementation
*
*/


#include "csensorproperties.h"
#include "logger.h"

const TPtrC SensorProperties::GetPropertyString(const TNameKey* aKey, TInt aValue)
{
    JELOG2(ESensor);
    for (TInt i = 0 ;  aKey[ i ].iName != NULL; i++)
    {
        if (aKey[ i ].iKey == aValue)
        {
            const TPtrC name = aKey[ i ].iName;
            return name;
        }
    }
    return KNullDesC();
}
