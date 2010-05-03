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
Holds a set of path access and mapping policies for one or more drives. These
are associated with a particular drive in a CDriveAccessPolicy object.
@file
@internalComponent
@see MIDP::MDriveAccessPolicy
*/

#include <e32base.h>
#include <f32file.h>
#include "PathAccessPolicy.h"


namespace MIDP
{

/**
Creates a new path access policy object. The default access is ENone for all
paths. When additional references to the object are taken, AddRef() must be
called. When a reference is no longer required, the object should not be deleted
but Release() called instead.
@return The path access policy object.
*/
CPathAccessPolicy* CPathAccessPolicy::NewL()
{
    return new(ELeave) CPathAccessPolicy();
}


CPathAccessPolicy::CPathAccessPolicy()
        : iCount(1)
{
}



CPathAccessPolicy::~CPathAccessPolicy()
{
    iAccessPathList.ResetAndDestroy();
    iAccessModeList.Close();
    iRootDirList.ResetAndDestroy();
    iRootMapList.ResetAndDestroy();
    iUseMidletHomePrefix.Close();
}


/**
Increments the reference count to the object.
*/
void CPathAccessPolicy::AddRef()
{
    iCount++;
}


/**
Decrements the reference count to the object. When this reaches zero, the object
is deleted.
*/
void CPathAccessPolicy::Release()
{
    __ASSERT_DEBUG(iCount > 0, User::Invariant());

    if (--iCount == 0)
    {
        delete this;
    }
}


/**
Case-insensitive descriptor comparator.
@param aFirst The first descriptor to compare.
@param aSecond The second descriptor to compare.
@return The result of the comparison.
@see TDesC::Compare
*/
LOCAL_C TInt HBufCCompare(const HBufC& aFirst, const HBufC& aSecond)
{
    // Note that since paths are case insensitive, the folded form of comparison
    // is used.
    return aFirst.CompareF(aSecond);
}


/**
Implements finding the path access mode for a drive access policy.
@see MIDP::MDriveAccessPolicy::PathAccess
*/
CPathAccessPolicy::TPathAccessMode CPathAccessPolicy::PathAccess(const TDesC& aPath) const
{
    // Create a copy of the path, with space for a trailing separator if needed.
    HBufC* pathCopy = HBufC::New(aPath.Length() + 1);
    if (!pathCopy)
    {
        return MDriveAccessPolicy::ENone;
    }
    TPtr pathPtr = pathCopy->Des();
    pathPtr = aPath;

    TPathAccessMode access = SubPathAccess(pathCopy, ETrue);
    delete pathCopy;

    return access;
}


_LIT(KSeparator, "\\");

/**
Used recursively when searching for the path access policy for a path.
@param aPath The path to examine. There must be at least space for one
additional character to be added at the end.
@param aAllowNonRecursiveMatch Set to ETrue when a non-recursive path specifier
(i.e. without a trailing separator) can be used to match the path. Otherwise,
only recursive path specifiers can match the given path.
@return The access mode for the path given.
*/
CPathAccessPolicy::TPathAccessMode CPathAccessPolicy::SubPathAccess(HBufC* aPath,
        TBool aAllowNonRecursiveMatch) const
{
    // Append a separator if one isn't present, since most path specifications
    // will include one and this will increase the likelihood of an exact match.
    TPtr pathPtr = aPath->Des();
    if (pathPtr.Right(1) != KSeparator)
    {
        pathPtr.Append(KSeparator);
    }

    TLinearOrder<HBufC> comparator(HBufCCompare);
    TInt index;
    TInt error = iAccessPathList.FindInOrder(aPath, index, comparator);

    if (error == KErrNone)
    {
        // Exact match found so use that access mode.
        return iAccessModeList[ index ];
    }

    if (error != KErrNotFound)
    {
        // Some unexpected error occurred, so prevent access.
        return MDriveAccessPolicy::ENone;
    }

    // Error is KErrNotFound - no exact match found. The index will be between
    // the two closest matches. Longer matches are irrelevant since they could
    // only be subdirectories or other paths, but the previous entry (if
    // present) may match up to the path's trailing separator and be acceptable
    // if the flag passed in is set.

    // Remove the trailing slash
    pathPtr.SetLength(pathPtr.Length() - 1);

    if (aAllowNonRecursiveMatch && (index > 0))
    {
        const TDesC& previous = *iAccessPathList[ index - 1 ];
        if (previous == pathPtr)
        {
            return iAccessModeList[ index ];
        }
    }

    // If the string is empty, there is nothing more to check.
    if (pathPtr.Length() == 0)
    {
        return MDriveAccessPolicy::ENone;
    }

    // Otherwise try checking the parent directory. This must match a recursive
    // path pattern (i.e. that ends with a separator).

    TInt separator = pathPtr.LocateReverse(TChar('\\'));
    // Modify path to remove this directory.
    pathPtr.SetLength((separator >= 0) ? separator : 0);

    return SubPathAccess(aPath, EFalse);
}



/**
Implements the root directory listing for a drive access policy.
@see MIDP::MDriveAccessPolicy::RootDirectoryListL
*/
const RPointerArray<HBufC>& CPathAccessPolicy::RootDirectoryListL() const
{
    return iRootDirList;
}


/**
Implements the path mapping for a drive access policy.
@see MIDP::MDriveAccessPolicy::MapPathL
*/
TBool CPathAccessPolicy::MapPathL(const TDesC& aFrom, TDes& aTo) const
{
    TInt mappingCount = iRootDirList.Count();

    if (mappingCount == 0)
    {
        // Special case 1: when no mappings are defined, direct mapping is used.
        aTo = aFrom;
        return EFalse;
    }

    if (mappingCount == 1 && iRootDirList[0]->Length() == 0)
    {
        // Special case 2: when one mapping is defined, and it is from the empty
        // string, everything is mapped by prepending the map string.
        aTo = *iRootMapList[0];
        aTo.Append(aFrom);
        return iUseMidletHomePrefix[0];
    }

    TInt separator = aFrom.Locate(TChar('\\'));
    HBufC* fromCopy;

    if (separator == KErrNotFound)
    {
        // No separator, this is just the directory name.
        fromCopy = aFrom.AllocL();
    }
    else
    {
        // Match up to, but not including, the separator if it is present.
        fromCopy = aFrom.Left(separator).AllocL();
    }

    if (fromCopy->Length() == 0)
    {
        // Special case 3: where the root mapping is requested. This must not
        // leave, otherwise the root could not be inspected. Just pass back
        // whatever was passed in (may include a separator or not).
        delete fromCopy;
        aTo = aFrom;
        return EFalse;
    }

    TLinearOrder<HBufC> comparator(HBufCCompare);
    TInt index;
    TInt error = iRootDirList.FindInOrder(fromCopy, index, comparator);

    delete fromCopy;
    User::LeaveIfError(error);

    // Found the directory in the list, so use the mapping.
    aTo = *iRootMapList[ index ];

    if (separator != KErrNotFound)
    {
        // Add the rest of the original path.
        aTo.Append(aFrom.Mid(separator));
    }

    return iUseMidletHomePrefix[ index ];
}


/**
Adds the access mode for a path specifier to the policy. If the specifier ends
in a separator, it applies to that directory and everything in it, otherwise it
applies only to that file or directory. A special case is the path specifier
consisting of just a separator. This means the access mode applies to all files
on the drive, unless overridden by a more specific path specifier.
@param aPath The path specifier.
@param aMode The access mode.
*/
void CPathAccessPolicy::AddPathAccessL(const TDesC& aPath, TPathAccessMode aMode)
{
    HBufC* path = aPath.AllocL();
    TInt index;
    TLinearOrder<HBufC> comparator(HBufCCompare);
    TInt error = iAccessPathList.FindInOrder(path, index, comparator);

    if (error == KErrNone)
    {
        // Already in list, so leave the path in the path list and replace the
        // existing access mode in the mode list.
        delete path;
        iAccessModeList[ index ] = aMode;
    }
    else if (error == KErrNotFound)
    {
        // Not found, so add the entries to each list at the same point. The
        // index returned by FindInOrder() will be the correct insertion point.
        User::LeaveIfError(iAccessPathList.Insert(path, index));
        User::LeaveIfError(iAccessModeList.Insert(aMode, index));
    }
    else
    {
        // An unhandled error occurred
        delete path;
        User::Leave(error);
    }
}


/**
Adds a mapping from one path to another. Also specifies whether the destination
path is complete in itself or needs to be prefixed by the MIDlet's home path.
The source directory cannot contain any separators or the function will leave.
@param aDir The sub-directory of the root to map.
@param aPath The destination path for this mapping.
@param aInMidletHome ETrue if the destination path should be prefixed by the
MIDlet's home path.
*/
void CPathAccessPolicy::AddPathMappingL(const TDesC& aDir, const TDesC& aPath,
                                        TBool aInMidletHome)
{
    if (aDir.Locate(TChar('\\')) != KErrNotFound)
    {
        User::Leave(KErrArgument);
    }

    HBufC* dir = aDir.AllocL();
    TInt index;
    TLinearOrder<HBufC> comparator(HBufCCompare);
    TInt error = iRootDirList.FindInOrder(dir, index, comparator);

    if (error == KErrNone)
    {
        // Already in root list, so leave the dir in the dir list and replace
        // the existing mapping in the map list.
        delete dir;
        HBufC* oldPath = iRootMapList[ index ];
        iRootMapList[ index ] = aPath.AllocL();
        iUseMidletHomePrefix[ index ] = aInMidletHome;
        delete oldPath;
    }
    else if (error == KErrNotFound)
    {
        // Not found, so add the entries to each list at the same point. The
        // index returned by FindInOrder() will be the correct insertion point.
        HBufC* path = aPath.AllocLC();
        CleanupStack::PushL(dir);
        User::LeaveIfError(iRootDirList.Insert(dir, index));
        CleanupStack::Pop(dir);
        User::LeaveIfError(iRootMapList.Insert(path, index));
        CleanupStack::Pop(path);
        User::LeaveIfError(iUseMidletHomePrefix.Insert(aInMidletHome, index));
    }
    else
    {
        // An unhandled error occurred
        delete dir;
        User::Leave(error);
    }
}

} // end of namespace MIDP

