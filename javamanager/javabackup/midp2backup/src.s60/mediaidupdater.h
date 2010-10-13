/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: MediaIdUpdater
*
*/


#ifndef MEDIAIDUPDATER_H
#define MEDIAIDUPDATER_H

#include "javaosheaders.h"

#include <string>
#include <set>

namespace java
{
namespace storage
{
    class JavaStorage;
}
namespace backup
{

/**
* MediaIdUpdater is responsible for ensuring that java storage contains
* valid media ids after restore.
* Media id may change in following situations:
*   - memory card is formatted between backup and restore
*   - another memory card is used during restore than originally
*/

OS_NONSHARABLE_CLASS(MediaIdUpdater)
{
public:
    MediaIdUpdater();
    virtual ~MediaIdUpdater();

    /**
    * Updates media ids to valid values if needed
    * Note: this method must be called after java storage is restored
    */
    virtual void update();

protected:
    virtual std::set<std::wstring> getPresentMediaIds();
    virtual std::set<std::wstring> getPresentRemovableDriveMediaIds();
    virtual std::set<std::wstring> removePresentMediaIds(const std::set<std::wstring>& aStorageIds,
                                                         const std::set<std::wstring>& aPresentIds);
    virtual bool isMediaIdUpdateNeeded(const std::set<std::wstring>& aInvalidIds,
                                       const std::set<std::wstring>& aValidIds);
    virtual std::set<std::wstring> getMediaIdsFromStorage();
    virtual void updateMediaIds(const std::set<std::wstring>& aInvalidIds,
                                const std::set<std::wstring>& aValidIds);

    virtual void open();
    virtual void close();

    java::storage::JavaStorage* mStorage;
};

} // namespace backup
} // namespace java

#endif // MEDIAIDUPDATER_H
