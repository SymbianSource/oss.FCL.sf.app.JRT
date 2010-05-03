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
* Description:  mjavaregcacheupdater definition
*
*/


#ifndef MJAVAREGCACHEUPDATER_H
#define MJAVAREGCACHEUPDATER_H

namespace Java
{
namespace Manager
{
namespace Registry
{
class CJavaPropertyArray;

/**
 * This interface is intended to let handle the cache in the registry,
 * to add, and remove entries.
 * @since S60 v3.2
 */
class MJavaRegCacheUpdater
{
public:

    /**
     * Adds the given registry entry to the cache.
     *
     * @since S60 v3.2
     * @param aFile The path of the registry entry to be added
     *              to the cache.
     * @param aProperties property array or null if not available
     */
    virtual void CacheEntryL(const TDesC& aFile,
                             CJavaPropertyArray* aProperties) = 0;

    /**
     * Removes the given registry entry from the cache.
     *
     * @since S60 v3.2
     * @param aFile The path of the registry entry to be removed
     *              from the cache.
     */
    virtual void RemoveCachedEntry(const TDesC& aFile) = 0;
};

}//namespace Registry
}//namespace Manager
}//namespace Java

#endif // MJAVAREGCACHEUPDATER_H

// End of File
