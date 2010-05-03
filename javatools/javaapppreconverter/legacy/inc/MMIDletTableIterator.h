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


#ifndef MMIDLETTABLEITERATOR_H
#define MMIDLETTABLEITERATOR_H

#include <e32std.h>

namespace MIDP
{
namespace DBv2
{

class MMIDletTableIterator
{
public:

    virtual ~MMIDletTableIterator() = 0;

    virtual TBool HasNext(void) = 0;

    virtual void NextL(
        TUint32& aMSId,
        TUint32& aOrdinal,
        TUint32& aUID,
        TPtrC&   aName,
        TPtrC&   aClassName,
        TPtrC&   aID) = 0;
};

inline MMIDletTableIterator::~MMIDletTableIterator()
{
}

} // end of namespace DBv2
} // end of namespace MIDP

#endif // MMIDLETTABLEITERATOR_H

