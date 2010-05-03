/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
 * @file DriveAccessPolicy.h
 *
 * @internal
 *
 */

#ifndef DRIVEACCESSPOLICY_H
#define DRIVEACCESSPOLICY_H

#include <e32base.h>
#include <f32file.h>
#include "MDriveAccessPolicy.h"

namespace MIDP
{
class CPathAccessPolicy;

class CDriveAccessPolicy: public CBase, public MDriveAccessPolicy
{
public:
    static CDriveAccessPolicy* NewL(const RFs& aFs, TDriveNumber aDrive);
    virtual ~CDriveAccessPolicy();

    // MVersionedInterface interface
    inline virtual TUint32 InterfaceVersion(void) const;

    // MDriveAccessPolicy interface
    virtual TBool DriveAccess() const;
    virtual void GetRootName(TDes& aRootName) const;
    virtual TAccessMode PathAccess(const TDesC& aPath) const;
    virtual const RPointerArray<HBufC>& RootDirectoryListL() const;
    virtual TBool MapPathL(const TDesC& aFrom, TDes& aTo) const;

    void SetPathAccessPolicy(CPathAccessPolicy* aPathAccessPolicy);

private:
    CDriveAccessPolicy(const RFs& aFs, TDriveNumber aDriveNumber);

private:
    const RFs& iFs;
    TDriveNumber iDrive;
    CPathAccessPolicy* iPathAccessPolicy;
};

inline TUint32 CDriveAccessPolicy::InterfaceVersion(void) const
{
    return 0x0100;
}

} // end of namespace MIDP

#endif // DRIVEACCESSPOLICY_H
