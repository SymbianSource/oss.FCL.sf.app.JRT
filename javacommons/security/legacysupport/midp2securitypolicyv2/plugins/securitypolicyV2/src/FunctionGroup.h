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
 * @file FunctionGroup.h
 *
 * @internal
 *
 */

#ifndef FUNCTIONGROUP_H
#define FUNCTIONGROUP_H

#include <e32base.h>
#include "MFunctionGroup.h"

namespace MIDP
{

class CFunctionGroup: public CBase, public MFunctionGroup
{
public:

    static CFunctionGroup* NewLC(const TDesC&);

public:

    virtual ~CFunctionGroup();

    // MFunctionGroup interface

    virtual const TDesC& Name(void) const;
    virtual const RPointerArray<const HBufC>& Permissions(void) const;

    // End of MFunctionGroup interface

    void AddPermissionL(const HBufC*);

private:

    CFunctionGroup();

    void ConstructL(const TDesC&);

private:

    HBufC*                      iName;
    RPointerArray<const HBufC>  iPermissions;
};

} // end of namespace MIDP

#endif // FUNCTIONGROUP_H
