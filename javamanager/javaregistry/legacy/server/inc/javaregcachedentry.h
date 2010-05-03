/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  javaregcachedentry definition
*
*/


#ifndef JAVAREGCACHEDENTRY_H
#define JAVAREGCACHEDENTRY_H

// INCLUDES
#include <e32base.h>

namespace Java
{
namespace Manager
{
namespace Registry
{

class CJavaProperty;

/**
 * This is a storage class to cache some information about
 * the registry entries.
 *
 * @since S60 v3.2
 */
// CLASS DECLARATION
NONSHARABLE_CLASS(CJavaRegCachedEntry) :
        public CBase
{

public:

    /**
     * Constructor
     *
     * @since S60 v3.2
     * @param aUid The uid of the registry entry.
     * @param aPath The path to the file containing the registry entry.
     * @param aActive The entry is active, if the media is present,
     *                wich this entry is installed to.
     * @param aTemp This boolean means, that true:this is a temporary
     *              cacheentry (placeholder for the allocated uid)
     *              false: or a normal entry.
     */
    CJavaRegCachedEntry(const TUid& aUid,
    const TDesC& aPath,
    TBool aActive,
    TBool aTemp);

    /**
     * Constructor
     * @since S60 v3.2
     */
    CJavaRegCachedEntry();

    /**
     * Destructor
     * @since S60 v3.2
     */
    ~CJavaRegCachedEntry();

public:

    // The uid of the registry entry.
    TUid iUid;

    // The path to the file containing the registry entry.
    TPath iPath;

    // The entry is active, if the media is present,
    // wich this entry is installed to.
    TBool iActive;

    // This boolean means, that true:this is a temporary
    // cacheentry (placeholder for the allocated uid)
    // false: or a normal entry.
    TBool iTemp;

    // Stores the property value of entries.
    RPointerArray<TDesC> iIndexes;

};

}//namespace Registry
}//namespace Manager
}//namespace Java

#endif // JAVAREGCACHEDENTRY_H

// End of File
