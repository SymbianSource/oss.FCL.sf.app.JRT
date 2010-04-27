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
 * @file    ConnectionFilter.h
 *
 * @internal
 *
 */

#ifndef CONNECTIONFILTER_H
#define CONNECTIONFILTER_H

#include <e32base.h>
#include "j2me/midp2/security/MConnectionFilter.h"

namespace MIDP
{

class CConnectionFilter: public CBase, public MConnectionFilter
{
public:

    static CConnectionFilter* NewLC(const TDesC&, TInt, const TDesC&, const RPointerArray<HBufC>&);

public:

    virtual ~CConnectionFilter();

    // MConnectionFilter interface

    virtual TInt         Type(void) const;
    virtual const TDesC& TypeName(void) const;
    virtual const TDesC& Scheme(void) const;
    virtual const RPointerArray<HBufC>& Params(void) const;

    // End of MConnectionFilter interface

private:

    CConnectionFilter(const TDesC&, TInt);

    void ConstructL(const TDesC&, const RPointerArray<HBufC>&);

private:

    const TDesC&            iTypeName;
    TInt                    iType;
    HBufC*                  iScheme;
    RPointerArray<HBufC>    iParams;
};

} // end of namespace MIDP

#endif // CONNECTIONFILTER_H
