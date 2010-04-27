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
The reference implementation of the file system access policy.
@file
@internalComponent
@see MIDP::MFileSystemAccessPolicy
*/

#include <e32base.h>
#include <j2me/midp2/security/IPCVersion.h>
#include "FileSystemAccessPolicy.h"
#include "DriveAccessPolicy.h"
#include "PathAccessPolicy.h"


namespace MIDP
{

/**
Creates a new file system access policy object.
@return The file system access policy object.
*/
CFileSystemAccessPolicy* CFileSystemAccessPolicy::NewL()
{
    CFileSystemAccessPolicy* fsap = new(ELeave) CFileSystemAccessPolicy();
    CleanupStack::PushL(fsap);
    fsap->ConstructL();
    CleanupStack::Pop(fsap);
    return fsap;
}


CFileSystemAccessPolicy::CFileSystemAccessPolicy()
        : iDriveAccessPolicies(TInt(KMaxDrives))
{
}


CFileSystemAccessPolicy::~CFileSystemAccessPolicy()
{
    iDriveAccessPolicies.ResetAndDestroy();
    delete iDefaultDriveAccessPolicy;
    iFs.Close();
}


void CFileSystemAccessPolicy::ConstructL()
{
    User::LeaveIfError(iFs.Connect(0));      // No asynchronous operations used.
    User::LeaveIfError(ShareSession(iFs));

    // Create the default policy with an invalid drive number.
    iDefaultDriveAccessPolicy = CDriveAccessPolicy::NewL(iFs,
                                TDriveNumber(EDriveA - 1));

    for (TInt ii = TInt(EDriveA); ii <= TInt(EDriveZ); ii++)
    {
        User::LeaveIfError(iDriveAccessPolicies.Append(NULL));
    }
}


/**
Returns the current drive policy if already created, or creates a new one if not
and returns a reference to it.
@param aDrive The drive number for which the policy is required.
@return A reference to the drive access policy for the given drive number.
*/
CDriveAccessPolicy& CFileSystemAccessPolicy::GetDriveAccessPolicyL(
    TDriveNumber aDrive)
{
    TInt index = static_cast<TInt>(aDrive) - static_cast<TInt>(EDriveA);
    CDriveAccessPolicy* policy = iDriveAccessPolicies[ index ];

    if (!policy)
    {
        policy = CDriveAccessPolicy::NewL(iFs, aDrive);
        iDriveAccessPolicies[ index ] = policy;
    }

    return *policy;
}


// MFileSystemAccessPolicy implementation

/**
Implements returning the current drive access policy for a given drive.
@see MIDP::MFileSystemAccessPolicy::DriveAccessPolicyL
*/
const MDriveAccessPolicy& CFileSystemAccessPolicy::DriveAccessPolicyL(
    TDriveNumber aDrive) const
{
    if (aDrive < EDriveA || aDrive > EDriveZ)
    {
        User::Leave(KErrBadName);
    }

    TInt index = static_cast<TInt>(aDrive) - static_cast<TInt>(EDriveA);
    MDriveAccessPolicy* policy = iDriveAccessPolicies[ index ];

    if (policy)
    {
        return *policy;
    }

    return *iDefaultDriveAccessPolicy;
}


/**
Implements returning the drive number for a given root name.
@see MIDP::MFileSystemAccessPolicy::RootNameToDriveNumberL
*/
TDriveNumber CFileSystemAccessPolicy::RootNameToDriveNumberL(
    const TDesC& aRootName) const
{
    // Check for standard letter-colon combination.
    if ((aRootName.Length() == 2) && (aRootName[1] == ':'))
    {
        TUint16 ch = aRootName[0];
        ch &= ~0x20;    // Mask out ASCII case bit to turn 'a'-'z' to 'A'-'Z'.

        if (ch >= 'A' && ch <= 'Z')
        {
            TInt ii = static_cast<TInt>(ch - 'A');
            MDriveAccessPolicy* policy = iDriveAccessPolicies[ii];

            if (policy && policy->DriveAccess())
            {
                return TDriveNumber(ii);
            }
        }
    }

    // Otherwise use access policy name.
    TBuf<KMaxFileName> name;

    for (TInt ii = TInt(EDriveA); ii <= TInt(EDriveZ); ii++)
    {
        MDriveAccessPolicy* policy = iDriveAccessPolicies[ii];

        if (policy && policy->DriveAccess())
        {
            policy->GetRootName(name);
            if (aRootName == name)
            {
                return TDriveNumber(ii);
            }
        }
    }

    User::Leave(KErrBadName);
    return TDriveNumber(TInt(EDriveA) - 1);       // Prevent warning about not returning.
}

// End of MFileSystemAccessPolicy implementation

} // end of namespace MIDP

