/*
* Copyright (c) 2003, 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class which allocates random UIDs from a given range
*
*/

#ifndef UIDALLOCATOR_H
#define UIDALLOCATOR_H

// INCLUDES
#include <e32base.h>

namespace Java
{
namespace Manager
{
namespace Registry
{

// CLASS DECLARATION

/**
 * This class allows clients to allocate a set of UIDs from a given range
 * @internalComponent
 * @since S60 v3.2
 */
class TUidAllocator
{
public:
    /**
     * Construct a new UID allocator.
     * @param aLowerBound The lowest value which can be allocated
     * @param aUpperBound Values can be allocated up to, but not including
     * the upper bound
     * @param aUsedUids The array of already used UIDs
     * @since S60 v3.2
     */
    TUidAllocator(TUid aLowerBound, TUid aUpperBound, const RArray<TUid>& aUsedUids);

    /**
     * Allocates a certain number of UIDs and appends them to the array. These
     * UIDs will be contiguously allocated
     * @since S60 v3.2
     */
    void AllocateL(TInt aNumberOfUids, RArray<TUid>& aArray);

    /**
     * Allocates a single UID and returns it
     * @return a TUid allocated by this method
     * @since S60 v3.2
     */
    TUid AllocateL();

private: //Data

    /**
     * The lowest value which can be allocated
     * @since S60 v3.2
     */
    TInt32 iLowerBound;

    /**
     * Values can be allocated up to, but not including
     * the upper bound
     * @since S60 v3.2
     */
    TInt32 iUpperBound;

    /**
     * The array of already used UIDs
     * @since S60 v3.2
     */
    const RArray<TUid>& iUsedUids;

    /**
     * Seed for the random number generator
     * @since S60 v3.2
     */
    TInt64 iSeed;
};

}//namespace Registry
}//namespace Manager
}//namespace Java

#endif // UIDALLOCATOR_H

// End of File

