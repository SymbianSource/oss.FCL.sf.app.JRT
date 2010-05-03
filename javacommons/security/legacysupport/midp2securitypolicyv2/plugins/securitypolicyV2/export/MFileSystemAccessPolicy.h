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



#ifndef MFILESYSTEMACCESSPOLICY_H
#define MFILESYSTEMACCESSPOLICY_H

/**
 * @file
 * @publishedPartner
 * @interim
 */

#include <e32base.h>
#include <f32file.h>
#include "j2me/midp2/core/mversionedinterface.h"
#include "j2me/midp2/security/mdriveaccesspolicy.h"

namespace MIDP
{

/**
An abstract operational interface to a File System access policy. This enables
the determination of a drive number for a given root name and provides the drive
access policy for a drive.
*/

class MFileSystemAccessPolicy: public MVersionedInterface
{
public:
    /**
     * Gets the access policy object for the given drive.
     * @param aDrive
     * The drive for which the policy is desired.
     * @return
     * A reference to the access policy.
     */
    virtual const MDriveAccessPolicy& DriveAccessPolicyL(TDriveNumber aDrive) const = 0;

    /**
     * Maps a root name to a drive number. If more than one drive has the same
     * root name, the lowest of the drive numbers in the set will be returned.
     * @param aRootName
     * The root name for which the drive number is desired.
     * @return
     * The drive number for the given root name.
     */
    virtual TDriveNumber RootNameToDriveNumberL(const TDesC& aRootName) const = 0;
};

} // end of namespace MIDP

#endif // MFILESYSTEMACCESSPOLICY_H
