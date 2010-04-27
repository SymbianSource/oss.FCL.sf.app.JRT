/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/


/**
 *
 * @file    FunctionGroup.cpp
 *
 * @internal
 *
 */

#include "FunctionGroup.h"

/*
 *
 * @internal
 *
 * A concrete implementation of the MFunctionGroup 'interface'.
 *
 * @see MFunctionGroup
 *
 */

namespace MIDP
{

CFunctionGroup* CFunctionGroup::NewLC(const TDesC& aName)
{
    CFunctionGroup* fg = new(ELeave) CFunctionGroup;

    CleanupStack::PushL(fg);
    fg->ConstructL(aName);
    return fg;
}

CFunctionGroup::CFunctionGroup()
{
}

void CFunctionGroup::ConstructL(const TDesC& aName)
{
    iName = aName.AllocL();
}

CFunctionGroup::~CFunctionGroup()
{
    delete iName;
    iPermissions.Close();
}

const TDesC& CFunctionGroup::Name(void) const
{
    return *iName;
}

const RPointerArray<const HBufC>& CFunctionGroup::Permissions(void) const
{
    return iPermissions;
}

void CFunctionGroup::AddPermissionL(const HBufC* aPermission)
{
    User::LeaveIfError(iPermissions.Append(aPermission));
}

} // end of namespace MIDP
