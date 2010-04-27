/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  APNSettings main class.
*
*/



#include "mapnsettings.h"
#include "apncenreptable.h"

namespace CENREP
{

/**
 * Returns an instance implementing the MAPNSettings interface.
 *
 * @param aFlags the version of the interface required. Currently 0x0000
 *        (KAPNSettingsVersion) for CAPNCenRepTable.
 * @return an instance implementing the MAPNSettings interface. Ownership
 *         is transferred to the caller.
 */
EXPORT_C MAPNSettings* GetAPNSettingsLC(TUint32 aFlags)
{
    if (aFlags != KAPNSettingsVersion)
    {
        User::Leave(KErrArgument);
    }
    return CAPNCenRepTable::NewLC();
}

} // end of namespace CENREP
