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
 * @file FileSystemAccessPolicy.h
 *
 * @internal
 *
 */

#ifndef FILESYSTEMACCESSPOLICY_H
#define FILESYSTEMACCESSPOLICY_H

#include <e32base.h>
#include "MFileSystemAccessPolicy.h"
#include "DriveAccessPolicy.h"

namespace MIDP
{

class CFileSystemAccessPolicy: public CBase, public MFileSystemAccessPolicy
{
public:
    static CFileSystemAccessPolicy* NewL();
    virtual ~CFileSystemAccessPolicy();

    // MVersionedInterface interface
    inline virtual TUint32 InterfaceVersion(void) const;

    // MFileSystemAccessPolicy interface
    virtual const MDriveAccessPolicy& DriveAccessPolicyL(TDriveNumber aDrive) const;
    virtual TDriveNumber RootNameToDriveNumberL(const TDesC& aRootName) const;

    CDriveAccessPolicy& GetDriveAccessPolicyL(TDriveNumber aDrive);

private:
    CFileSystemAccessPolicy();
    void ConstructL();

private:
    RFs iFs;
    RPointerArray<CDriveAccessPolicy> iDriveAccessPolicies;
    CDriveAccessPolicy* iDefaultDriveAccessPolicy;
};

inline TUint32 CFileSystemAccessPolicy::InterfaceVersion(void) const
{
    return 0x0100;
}

} // end of namespace MIDP

#endif // FILESYSTEMACCESSPOLICY_H
