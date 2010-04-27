/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef MMIDLETTABLEV2_H
#define MMIDLETTABLEV2_H

#include <e32std.h>

class RDbDatabase;

namespace MIDP
{
namespace DBv2
{
class MMIDletTableIterator;

class MMIDletTable
{
public:

    virtual void CreateL(RDbDatabase&)  = 0;

    virtual void OpenL(RDbDatabase&)    = 0;

    virtual void Close(void)    = 0;

    virtual void AddL(
        TUint32      aMSId,
        TUint32      aOrdinal,
        TUint32      aUID,
        const TDesC& aName,
        const TDesC& aClassName,
        const TDesC& aID) = 0;

    // Update single midlet given by msid and name
    virtual void UpdateL(
        TUint32      aMSId,
        const TDesC& aName,
        TUint32      aOrdinal,
        TUint32      aUID,
        const TDesC& aClassName,
        const TDesC& aID) = 0;

    virtual void RemoveL(TUint32 aMSId) = 0;

    virtual MMIDletTableIterator* IteratorLC(void)  = 0;
};


} // end of namespace DBv2
} // end of namespace MIDP

#endif // MMIDLETTABLEV2_H

