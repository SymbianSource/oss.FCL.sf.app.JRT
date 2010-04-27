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



#ifndef PATHACCESSPOLICY_H
#define PATHACCESSPOLICY_H

/**
@file
@internalComponent
*/

#include <e32base.h>
#include "j2me/midp2/security/MDriveAccessPolicy.h"


namespace MIDP
{

class CPathAccessPolicy : public CBase
{
private:
    typedef MDriveAccessPolicy::TAccessMode TPathAccessMode;

public:
    static CPathAccessPolicy* NewL();
    virtual ~CPathAccessPolicy();

    void AddRef();
    void Release();

    TBool DriveAccess() const;
    TPathAccessMode PathAccess(const TDesC& aPath) const;
    const RPointerArray<HBufC>& RootDirectoryListL() const;
    TBool MapPathL(const TDesC& aFrom, TDes& aTo) const;

    void AddPathAccessL(const TDesC& aPath, TPathAccessMode aMode);
    void AddPathMappingL(const TDesC& aDir, const TDesC& aPath, TBool aInMidletHome);

private:
    CPathAccessPolicy();

    TPathAccessMode SubPathAccess(HBufC* aPath, TBool aAllowNonRecursiveMatch) const;

private:
    TInt iCount;    // Reference count to object.
    RPointerArray<HBufC> iAccessPathList;   // Kept in alphabetical order.
    RArray<TPathAccessMode> iAccessModeList;    // Kept in iAccessPathList order.
    RPointerArray<HBufC> iRootDirList;  // Kept in alphabetical order.
    RPointerArray<HBufC> iRootMapList;  // Kept in iRootDirList order.
    RArray<TBool> iUseMidletHomePrefix; // Kept in iRootDirList order.
};

inline TBool CPathAccessPolicy::DriveAccess() const
{
    return ETrue;
}

} // end of namespace MIDP

#endif // PATHACCESSPOLICY_H
