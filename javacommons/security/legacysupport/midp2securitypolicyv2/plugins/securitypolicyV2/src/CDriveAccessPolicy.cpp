/*
* Copyright (c) 2003 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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
The reference implementation of the drive access policy.
@file
@internalComponent
@see MIDP::MDriveAccessPolicy
*/

#include <e32base.h>
#include <f32file.h>
#include "DriveAccessPolicy.h"
#include "PathAccessPolicy.h"

#include <j2me/jdebug.h>

namespace MIDP
{

/**
Creates a new drive access policy object. By default, the drive cannot be
accessed and has no path mappings.
@param aFs A reference to the file system resource.
@param aDrive The drive number for this policy object.
@return The drive access policy object.
*/
CDriveAccessPolicy* CDriveAccessPolicy::NewL(const RFs& aFs, TDriveNumber aDrive)
{
    return new(ELeave) CDriveAccessPolicy(aFs, aDrive);
}


CDriveAccessPolicy::CDriveAccessPolicy(const RFs& aFs, TDriveNumber aDrive)
        : iFs(aFs), iDrive(aDrive)
{
}


CDriveAccessPolicy::~CDriveAccessPolicy()
{
    SetPathAccessPolicy(NULL);
}


/**
Sets the path access policy for this drive. A new reference to the object is
taken. If there was a policy already set, it is released.
@param aPathAccessPolicy The new path access policy object.
*/
void CDriveAccessPolicy::SetPathAccessPolicy(
    CPathAccessPolicy* aPathAccessPolicy)
{
    if (iPathAccessPolicy)
    {
        iPathAccessPolicy->Release();
    }
    iPathAccessPolicy = aPathAccessPolicy;
    if (iPathAccessPolicy)
    {
        iPathAccessPolicy->AddRef();
    }
}


// MDriveAccessPolicy implementation

/**
Concrete implementation of GetRootName.
@see MIDP::MDriveAccessPolicy::GetRootName
*/
void CDriveAccessPolicy::GetRootName(TDes& aRootName) const
{
    TInt error = iFs.GetDriveName(iDrive, aRootName);

    if (error == KErrNone && aRootName.Length() == 0)
    {
        // If there is no drive name set, use the default letter-colon form.
        aRootName.Append(TChar(static_cast<TUint16>('A' + iDrive - EDriveA)));
        aRootName.Append(TChar(static_cast<TUint16>(':')));
    }
}


/**
Concrete implementation of DriveAccess.
@see MIDP::MDriveAccessPolicy::DriveAccess
*/
TBool CDriveAccessPolicy::DriveAccess() const
{
    DEBUG("+CDriveAccessPolicy::DriveAccess()");
    if (iPathAccessPolicy)
    {
        // Use Drive to check if the drive has media in it.
        TDriveInfo driveInfo;
        TBool ready = (iFs.Drive(driveInfo, iDrive) == KErrNone);
        if (!ready)
        {
            DEBUG("-CDriveAccessPolicy::DriveAccess() returns first false");
        }
        ready = ready && (driveInfo.iType != EMediaNotPresent) && (driveInfo.iType != EMediaUnknown);
        if (iDrive == EDriveF)
        {
            DEBUG_INT("drive info media type = %D", driveInfo.iType);
        }
        if (!ready)
        {
            DEBUG("-CDriveAccessPolicy::DriveAccess() returns second false");
        }
        ready = ready && iPathAccessPolicy->DriveAccess();
        if (ready)
        {
            DEBUG("-CDriveAccessPolicy::DriveAccess() returns true");
        }
        else
        {
            DEBUG("-CDriveAccessPolicy::DriveAccess() returns third false");
        }
        return ready;
    }

    DEBUG("-CDriveAccessPolicy::DriveAccess() returns last false");
    return EFalse;
}


/**
Concrete implementation of PathAccess.
@see MIDP::MDriveAccessPolicy::PathAccess
*/
MDriveAccessPolicy::TAccessMode CDriveAccessPolicy::PathAccess(
    const TDesC& aPath) const
{
    if (iPathAccessPolicy)
    {
        return iPathAccessPolicy->PathAccess(aPath);
    }

    return ENone;
}


/**
Concrete implementation of RootDirectoryListL.
@see MIDP::MDriveAccessPolicy::GetRootName
*/
const RPointerArray<HBufC>& CDriveAccessPolicy::RootDirectoryListL() const
{
    User::LeaveIfNull(iPathAccessPolicy);
    return iPathAccessPolicy->RootDirectoryListL();
}


/**
Concrete implementation of MapPathL.
@see MIDP::MDriveAccessPolicy::MapPathL
*/
TBool CDriveAccessPolicy::MapPathL(const TDesC& aFrom, TDes& aTo) const
{
    User::LeaveIfNull(iPathAccessPolicy);
    return iPathAccessPolicy->MapPathL(aFrom, aTo);
}

// End of MDriveAccessPolicy implementation

} // end of namespace MIDP

