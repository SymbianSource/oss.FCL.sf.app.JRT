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



#ifndef MDRIVEACCESSPOLICY_H
#define MDRIVEACCESSPOLICY_H

/**
@file
@publishedPartner
@interim
@see MIDP::MFileSystemAccessPolicy
*/

#include <e32base.h>
#include "j2me/midp2/core/mversionedinterface.h"


namespace MIDP
{

/**
Describes the access policy for a given drive. This includes both access control
and path mapping.
*/
class MDriveAccessPolicy : public MVersionedInterface
{
public:
    /**
     * Allowed access mode type.
     */
    enum TAccessMode
    {
        ENone = 0,
        ERead = 1,
        EWrite = 2,
        EReadWrite = 3
    };

    /**
     * Determine whether access to the drive is allowed or not. This may return
     * EFalse because of the access control settings or because there is no
     * media present, etc.
     *
     * @return ETrue if the MIDlet is allowed to access this drive and EFalse
     * otherwise.
     */
    virtual TBool DriveAccess() const = 0;

    /**
     * Get the root name for this drive. This is only valid if  DriveAccess()
     * returns ETrue.
     *
     * @param aRootName
     * The descriptor to receive the root name for the drive.
     */
    virtual void GetRootName(TDes& aRootName) const = 0;

    /**
     * Get the access mode allowed for the given path. By default this is ENone.
     * The path expected to be unmapped, that is, the same form of path as would
     * be passed to MapPathL().
     *
     * @param aPath
     * The unmapped path for which the access mode is desired.
     * @return The access mode for the given path.
     */
    virtual TAccessMode PathAccess(const TDesC& aPath) const = 0;

    /**
     * Get the allowed subdirectories from the root of this drive. If this is a
     * single, empty descriptor, all subdirectories are allowed. Otherwise the
     * list contains the only allowed subdirectories, although these may or may
     * not exist at the time of calling.
     * @return An array of mappable directory names that exist in the root, or
     * single, empty descriptor.
     */
    virtual const RPointerArray<HBufC>& RootDirectoryListL() const = 0;

    /**
     * Map a path and indicate whether the mapped path is partially formed from
     * the MIDlet home location, or not.
     *
     * @param aFrom
     * The unmapped path, to be mapped.
     * @param aTo
     * The mapped path.
     * @return  ETrue if the mapped path requires the current MIDlet's home
     * directory as a prefix, EFalse if the mapped path is complete.
     * @leave KErrNotFound If this path is not mappable.
     */
    virtual TBool MapPathL(const TDesC& aFrom, TDes& aTo) const = 0;
};

} // end of namespace MIDP

#endif // MDRIVEACCESSPOLICY_H
