/*
* Copyright (c) 2003 - 2005 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef MUSERSECURITYPREFERENCESTABLE_H
#define MUSERSECURITYPREFERENCESTABLE_H

#include <e32std.h>

class RDbDatabase;

namespace MIDP
{
namespace DBv2
{
class MUserSecurityPreferencesIterator;

class MUserSecurityPreferencesTable
{
public:

    virtual ~MUserSecurityPreferencesTable() = 0;

    virtual void CreateL(RDbDatabase&) = 0;

    virtual void OpenL(RDbDatabase&) = 0;
    virtual void Close(void) = 0;

    virtual void AddL(TUint32 aId, const TDesC& aName, const TDesC& aVendor, const TDesC& aVersion, const HBufC* aPD) = 0;
    virtual void RemoveL(TUint32) = 0;

    virtual HBufC8* PreferencesL(TUint32) = 0;
    virtual void    PreferencesL(TUint32, const TDesC8&) = 0;
    virtual HBufC*  ProtectionDomainL(TUint32) = 0;

    virtual MUserSecurityPreferencesIterator* IteratorL(void) = 0;
};

inline MUserSecurityPreferencesTable::~MUserSecurityPreferencesTable()
{
}

} // end of namespace DBv2
} // end of namespace MIDP

#endif // MUSERSECURITYPREFERENCESTABLE_H

