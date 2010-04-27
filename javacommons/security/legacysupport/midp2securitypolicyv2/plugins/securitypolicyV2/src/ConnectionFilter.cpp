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
 * @file    ConnectionFilter.cpp
 *
 * @internal
 *
 */

#include "ConnectionFilter.h"

/*
 * @internal
 *
 * A concrete implementation of the MConnectionFilter 'interface'.
 *
 * @see MConnectionFilter
 */

namespace MIDP
{

CConnectionFilter* CConnectionFilter::NewLC(const TDesC&                aTypeName,
        TInt                        aType,
        const TDesC&                aScheme,
        const RPointerArray<HBufC>& aParams)
{
    CConnectionFilter* cf = new(ELeave) CConnectionFilter(aTypeName, aType);

    CleanupStack::PushL(cf);
    cf->ConstructL(aScheme, aParams);
    return cf;
}

CConnectionFilter::CConnectionFilter(const TDesC& aTypeName, TInt aType):
        iTypeName(aTypeName),
        iType(aType)
{
}

void CConnectionFilter::ConstructL(const TDesC& aScheme, const RPointerArray<HBufC>& aParams)
{
    iScheme = aScheme.AllocL();

    TInt count = aParams.Count();

    for (TInt i = 0; i < count; i++)
    {
        HBufC* param = aParams[i]->AllocLC();

        User::LeaveIfError(iParams.Append(param));
        CleanupStack::Pop(param);
    }
}

CConnectionFilter::~CConnectionFilter()
{
    delete iScheme;
    iParams.ResetAndDestroy();
    iParams.Close();
}

TInt CConnectionFilter::Type(void) const
{
    return iType;
}

const TDesC& CConnectionFilter::TypeName(void) const
{
    return iTypeName;
}

const TDesC& CConnectionFilter::Scheme(void) const
{
    return *iScheme;
}

const RPointerArray<HBufC>& CConnectionFilter::Params(void) const
{
    return iParams;
}

} // end of namespace MIDP

